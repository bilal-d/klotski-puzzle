// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package puzzle_types

import (
	"strings"
)

type Grid struct {
	SizeX int
	SizeY int
	Pieces []Piece
}

func (grid *Grid) Copy() *Grid {
	newGrid := &Grid{
		grid.SizeX,
		grid.SizeY,
		make([]Piece, len(grid.Pieces)),
	}
	copy(newGrid.Pieces, grid.Pieces)
	return newGrid
}

func (grid *Grid) ToString() string {
	cells := grid.Validate()
	if cells == nil {
		return "|  INVALID"
	}
	lines := make([]string, grid.SizeY)
	for y := 0; y < grid.SizeY; y++ {
		names := make([]string, grid.SizeX)
		for x := 0; x < grid.SizeX; x++ {
			index := x + grid.SizeX * y
			names[x] = cells.tags[index].ToString()
		}
		lines[y] = "|  " + strings.Join(names, "  ")
	}
	return strings.Join(lines, "\n")
}

func (grid *Grid) Validate() *gridCells {
	// allocate a new gridCells structure
	cells := &gridCells{
		grid.SizeX,
		grid.SizeY,
		make([]PieceTag, grid.SizeX * grid.SizeY),
	}

	// mark all tags as empty
	for tagIndex := range(cells.tags) {
		cells.tags[tagIndex] = PieceTagEmpty()
	}

	// place each piece in the allocated cells,
	// while checking for collisions
	for pieceIndex := range(grid.Pieces) {
		piece := &grid.Pieces[pieceIndex]

		for geomIndex := range(piece.Geometry) {
			geom := &piece.Geometry[geomIndex]
			position := geom.Add(piece.Position)

			if !cells.contains(position) {
				return nil
			}

			tagIndex := position.X + cells.sizeX * position.Y
			tag := &cells.tags[tagIndex]

			if *tag != PieceTagEmpty() {
				return nil
			}

			*tag = piece.Tag
		}
	}
	return cells
}

func (grid *Grid) Apply(move Move) {
	piece := &grid.Pieces[move.PieceIndex]
	piece.Position = piece.Position.Add(move.Step.Vector)
}

