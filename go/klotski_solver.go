// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package main

import (
	"os"
	"fmt"
	"log"
	. "klotski-puzzle/puzzle_types"
	. "klotski-puzzle/svg_renderer"
)

type SearchEdge struct {
	ParentIndex int
	Move Move
}

type SuccessCondition func (*Grid) bool

func solvePuzzle(
	initialGrid *Grid,
	successCondition SuccessCondition,
	symmetry KeySymmetry) (*Grid, []Move) {

	validated := initialGrid.Validate()
	if validated == nil {
		return nil, nil
	}
	if successCondition(initialGrid) {
		return initialGrid, []Move{}
	}

	searchTree := NewSearchTree()
	searchTree.Append(initialGrid, nil, validated.Key(symmetry))

	for {
		searchTree.IncrementDepth()
		indexRange := searchTree.CurrentDepth()

		if indexRange.IsEmpty() {
			return nil, nil
		}

		// loop over last reached grids ...
		for parentIndex := indexRange.A; parentIndex < indexRange.B; parentIndex++ {
			for pieceIndex := range(initialGrid.Pieces) {
				for _, step := range(StepAll()) {
					move := Move{pieceIndex, step}
					grid := searchTree.NodeAt(parentIndex).(*Grid).Copy()
					grid.Apply(move)

					validated := grid.Validate()
					if validated == nil {
						continue
					}
					key := validated.Key(symmetry)
					if !searchTree.Append(grid, &SearchEdge{parentIndex, move}, key) {
						continue
					}
					if successCondition(grid) {
						path := make([]Move, 0)
						for edge := searchTree.EdgeAt(searchTree.LastIndex());
							edge != nil;
							edge = searchTree.EdgeAt(edge.(*SearchEdge).ParentIndex) {
							path = append(path, edge.(*SearchEdge).Move)
						}
						for i, j := 0, len(path)-1; i < j; i, j = i+1, j-1 {
							path[i], path[j] = path[j], path[i]
						}
						return grid, path
					}
				}
			}
		}
	}
}


func main() {
	pieceGeometryA := []Vect2{{0, 0}, {0, 1}, {1, 0}, {1, 1}}
	pieceGeometryB := []Vect2{{0, 0}, {0, 1}}
	pieceGeometryC := []Vect2{{0, 0}, {1, 0}}
	pieceGeometryD := []Vect2{{0, 0}}

	startingGrid := &Grid{4, 5, []Piece{
		{PieceTag{'A', 1}, Vect2{1, 0}, pieceGeometryA },
		{PieceTag{'B', 1}, Vect2{0, 0}, pieceGeometryB },
		{PieceTag{'B', 2}, Vect2{3, 0}, pieceGeometryB },
		{PieceTag{'B', 3}, Vect2{0, 2}, pieceGeometryB },
		{PieceTag{'B', 4}, Vect2{3, 2}, pieceGeometryB },
		{PieceTag{'C', 1}, Vect2{1, 2}, pieceGeometryC },
		{PieceTag{'D', 1}, Vect2{0, 4}, pieceGeometryD },
		{PieceTag{'D', 2}, Vect2{1, 3}, pieceGeometryD },
		{PieceTag{'D', 3}, Vect2{2, 3}, pieceGeometryD },
		{PieceTag{'D', 4}, Vect2{3, 4}, pieceGeometryD },
	}}
	fmt.Printf("initial grid:\n%s\n", startingGrid.ToString())

	finalGrid, path := solvePuzzle(startingGrid, klotskiIsSolved, HorizontalKeySymmetry)
	if finalGrid == nil {
		fmt.Println("no solution found, exiting")
		return
	}
	fmt.Printf("solved grid:\n%s\n", finalGrid.ToString())
	fmt.Printf("list of moves (%d):\n", len(path))

	for moveIndex := range(path) {
		move := &path[moveIndex]
		piece := &startingGrid.Pieces[move.PieceIndex]
		fmt.Printf("%s%s ", piece.Name(), move.Step.Symbol)
	}
	fmt.Printf("\n")

	svgFile, err := os.Create("klotski_solution.svg")
	if err != nil {
		log.Fatalf("error while opening output file in write mode, %v\n", err)
	}
	defer svgFile.Close()
	RenderKlotskiGrids(svgFile, startingGrid, path, "colorful")
}


func klotskiIsSolved(grid *Grid) bool {
	for index := range(grid.Pieces) {
		piece := &grid.Pieces[index]
		if (piece.Tag == PieceTag{'A', 1}) {
			return piece.Position == Vect2{1, 3}
		}
	}
	return false
}
