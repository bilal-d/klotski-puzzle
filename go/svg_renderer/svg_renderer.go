// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package svg_renderer

import (
	"io"
	"fmt"
	"log"
	pt "klotski-puzzle/puzzle_types"
)

type CustomRenderGridCallable func (
	renderer *SvgRenderer,
	svg *XmlWriter,
	grid *pt.Grid,
	title string)

type CustomRenderPieceCallable func (
	renderer *SvgRenderer,
	svg *XmlWriter,
	grid *pt.Piece,
	step *pt.Step)

type SvgRenderer struct {
	cellGap float64
	gridGap float64
	unitSize float64
	titleHeight float64
	offsetX float64
	offsetY float64
	borderRadius float64
	gridsPerRow int
	gridSizeX int
	gridSizeY int
	includeBackground bool
	properties map[string]string

	CustomRenderGrid CustomRenderGridCallable
	CustomRenderPiece CustomRenderPieceCallable
}

func NewSvgRenderer(grid *pt.Grid, theme string) *SvgRenderer {
	return &SvgRenderer{
		cellGap: 4.0,
		gridGap: 16.0,
		unitSize: 20.0,
		titleHeight: 12.0,
		offsetX: 0.5,
		offsetY: 0.5,
		borderRadius: 2.0,
		gridsPerRow: 10,
		gridSizeX: grid.SizeX,
		gridSizeY: grid.SizeY,
		includeBackground: false,
		properties: GetRendererTheme(theme),
	}
}

func GetRendererTheme(name string) map[string]string {
	simpleTheme := map[string]string{
		"fontFamily": "'Consolas','PT Mono','DejaVu Serif Mono',monospace",
		"fontSize": "16",
		"strokeWidthGrid": "1",
		"strokeWidthPiece": "1",
		"backgroundColor": "#ffffff",
		"textColor": "#0d1e26",
		"arrowColor": "#ffffff",
		"fillColorGrid": "#ecf4f9",
		"strokeColorGrid": "#8f9ca3",
		"fillColorHiPieceA": "#00aaff",
		"fillColorLoPieceA": "#8f9ca3",
		"fillColorHiPieceB": "#00aaff",
		"fillColorLoPieceB": "#8f9ca3",
		"fillColorHiPieceC": "#00aaff",
		"fillColorLoPieceC": "#8f9ca3",
		"fillColorHiPieceD": "#00aaff",
		"fillColorLoPieceD": "#8f9ca3",
		"strokeColorHiPieceA": "#0d1e26",
		"strokeColorLoPieceA": "#0d1e26",
		"strokeColorHiPieceB": "#0d1e26",
		"strokeColorLoPieceB": "#0d1e26",
		"strokeColorHiPieceC": "#0d1e26",
		"strokeColorLoPieceC": "#0d1e26",
		"strokeColorHiPieceD": "#0d1e26",
		"strokeColorLoPieceD": "#0d1e26",
	}

	colorfulTheme := map[string]string{
		"fontFamily": "'Consolas','PT Mono','DejaVu Serif Mono',monospace",
		"fontSize": "16",
		"strokeWidthGrid": "1",
		"strokeWidthPiece": "1",
		"backgroundColor": "#ffffff",
		"textColor": "#0d1e26",
		"arrowColor": "#ffffff",
		"fillColorGrid": "#ecf4f9",
		"strokeColorGrid": "#8f9ca3",
		"fillColorHiPieceA": "#ec433b",
		"fillColorLoPieceA": "#ec433b",
		"fillColorHiPieceB": "#93a707",
		"fillColorLoPieceB": "#93a707",
		"fillColorHiPieceC": "#3b98e0",
		"fillColorLoPieceC": "#3b98e0",
		"fillColorHiPieceD": "#e5b01a",
		"fillColorLoPieceD": "#e5b01a",
		"strokeColorHiPieceA": "#dc322f",
		"strokeColorLoPieceA": "#dc322f",
		"strokeColorHiPieceB": "#859900",
		"strokeColorLoPieceB": "#859900",
		"strokeColorHiPieceC": "#268bd2",
		"strokeColorLoPieceC": "#268bd2",
		"strokeColorHiPieceD": "#dba700",
		"strokeColorLoPieceD": "#dba700",
	}

	themes := map[string]map[string]string{
		"simple": simpleTheme,
		"colorful": colorfulTheme,
	}
	return themes[name]
}

func (renderer *SvgRenderer) Property(name string) string {
	value, found := renderer.properties[name]
	if !found {
		log.Fatalf("cannot find property: %s\n", name)
	}
	return value
}

func (renderer *SvgRenderer) getGridsOnXYAxis(gridCount int) (gridsOnXAxis, gridsOnYAxis int) {
	if gridCount <= renderer.gridsPerRow {
		gridsOnXAxis = gridCount
		gridsOnYAxis = 1
	} else {
		gridsOnXAxis = renderer.gridsPerRow
		gridsOnYAxis = 1 + (gridCount - 1) / renderer.gridsPerRow
	}
	return
}

func (renderer *SvgRenderer) getGridSize(includeTitle bool) (gridSizeX, gridSizeY float64) {
	gridSizeX = renderer.cellGap +
		float64(renderer.gridSizeX) * (renderer.unitSize + renderer.cellGap)

    gridSizeY = renderer.cellGap +
		float64(renderer.gridSizeY) * (renderer.unitSize + renderer.cellGap)

	if (includeTitle) {
		gridSizeY += renderer.titleHeight
	}
	return
}

func (renderer *SvgRenderer) getSvgSize(gridCount int, includeTitle bool) (svgSizeX, svgSizeY float64) {
	gridsOnXAxis, gridsOnYAxis := renderer.getGridsOnXYAxis(gridCount)
	gridSizeX, gridSizeY := renderer.getGridSize(includeTitle)

	svgSizeX = renderer.gridGap + float64(gridsOnXAxis) * (gridSizeX + renderer.gridGap)
	svgSizeY = renderer.gridGap + float64(gridsOnYAxis) * (gridSizeY + renderer.gridGap)
	return
}

func (renderer *SvgRenderer) gridPositionToXYCoord(x int, y int, includeTitle bool) (gridX, gridY float64) {
	gridSizeX, gridSizeY := renderer.getGridSize(includeTitle)

	gridX = renderer.offsetX + renderer.gridGap +
		float64(x) * (gridSizeX + renderer.gridGap)

    gridY = renderer.offsetY + renderer.gridGap +
		float64(y) * (gridSizeY + renderer.gridGap)
	
	if (includeTitle) {
		gridY += renderer.titleHeight
	}
	return
}

func (renderer *SvgRenderer) piecePositionToXYCoord(x, y int) (pieceX, pieceY float64) {
	pieceX = renderer.cellGap + float64(x) * (renderer.unitSize + renderer.cellGap)
	pieceY = renderer.cellGap + float64(y) * (renderer.unitSize + renderer.cellGap)
	return
}

func (renderer *SvgRenderer) nextGridPosition(x, y int) (int, int) {
	if x+1 >= renderer.gridsPerRow {
		return 0, y+1
	}
	return x+1, y
}

func (renderer *SvgRenderer) RenderGrid(out io.Writer, grid *pt.Grid) {
	svgSizeX, svgSizeY := renderer.getSvgSize(1, false)

	svg := NewXmlWriter(out)
	svg.
		Decl().
		Root("svg").
		Attr("width", svgSizeX).
		Attr("height", svgSizeY).
		Attr("xmlns", "http://www.w3.org/2000/svg")

	if renderer.includeBackground {
		svg.
			Elem("rect").
			Attr("width", "100%").
			Attr("height", "100%").
			Attr("fill", renderer.Property("backgroundColor")).
			Term()
	}

	renderer.render(svg, 0, 0, grid, nil, "")
	svg.Term()
}

func (renderer *SvgRenderer) RenderGrids(out io.Writer, grid *pt.Grid, moves []pt.Move) {
	gridCount := 1 + len(moves) + 1
	svgSizeX, svgSizeY := renderer.getSvgSize(gridCount, true)
	
	svg := NewXmlWriter(out)
	svg.
		Decl().
		Root("svg").
		Attr("width", svgSizeX).
		Attr("height", svgSizeY).
		Attr("xmlns", "http://www.w3.org/2000/svg")

	if renderer.includeBackground {
		svg.
			Elem("rect").
			Attr("width", "100%").
			Attr("height", "100%").
			Attr("fill", renderer.Property("backgroundColor")).
			Term()
	}

	gridX, gridY := 0, 0
	currentGrid := grid.Copy()
	renderer.render(svg, gridX, gridY, currentGrid, nil, "début")

	for moveIndex, move := range(moves) {
		title := fmt.Sprintf("étape %d", moveIndex + 1)

		gridX, gridY = renderer.nextGridPosition(gridX, gridY)
		renderer.render(svg, gridX, gridY, currentGrid, &move, title)
		currentGrid.Apply(move)
	}

	gridX, gridY = renderer.nextGridPosition(gridX, gridY)
	renderer.render(svg, gridX, gridY, currentGrid, nil, "fin")
	svg.Term()
}

func (renderer *SvgRenderer) render(
	svg *XmlWriter,
	gridX int,
	gridY int,
	grid *pt.Grid,
	move *pt.Move,
	title string) {

	includeTitle := title != ""
	gridCoordX, gridCoordY := renderer.gridPositionToXYCoord(gridX, gridY, includeTitle)
	
	svg.
		Elem("g").
		Attr("transform", fmt.Sprintf("translate(%g %g)", gridCoordX, gridCoordY))

	renderer.CustomRenderGrid(renderer, svg, grid, title)

	for pieceIndex := range(grid.Pieces) {
		piece := &grid.Pieces[pieceIndex]
		if move != nil && move.PieceIndex == pieceIndex {
			renderer.CustomRenderPiece(renderer, svg, piece, &move.Step)
		} else {
			renderer.CustomRenderPiece(renderer, svg, piece, nil)
		}
	}
	svg.Term()
}

