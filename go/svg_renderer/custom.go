// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package svg_renderer

import (
	"io"
	"fmt"
	"log"
	pt "klotski-puzzle/puzzle_types"
)

func klotskiCustomRenderGrid(renderer *SvgRenderer, svg *XmlWriter, grid *pt.Grid, title string) {
	gridSizeX, gridSizeY := renderer.getGridSize(false)
	svg.
		Elem("rect").
		Attr("x", 0).
		Attr("y", 0).
		Attr("width", gridSizeX).
		Attr("height", gridSizeY).
		Attr("rx", renderer.borderRadius).
		Attr("fill", renderer.Property("fillColorGrid")).
		Attr("stroke", renderer.Property("strokeColorGrid")).
		Attr("stroke-width", renderer.Property("strokeWidthGrid")).
		Term()

	if title != "" {
		svg.
			Elem("text").
			Attr("x", gridSizeX * 0.5).
			Attr("y", renderer.titleHeight * -0.5).
			Attr("text-anchor", "middle").
			Attr("fill", renderer.Property("textColor")).
			Attr("font-family", renderer.Property("fontFamily")).
			Attr("font-size", renderer.Property("fontSize")).
			Text(title).
			Term()
	}
}

func klotskiCustomRenderPiece(renderer *SvgRenderer, svg *XmlWriter, piece *pt.Piece, step *pt.Step) {
	var pieceSizeX, pieceSizeY float64
	switch piece.Tag.Symbol {
	case 'A':
		pieceSizeX = 2.0 * renderer.unitSize + renderer.cellGap
		pieceSizeY = 2.0 * renderer.unitSize + renderer.cellGap
	case 'B':
		pieceSizeX = renderer.unitSize
		pieceSizeY = 2.0 * renderer.unitSize + renderer.cellGap
	case 'C':
		pieceSizeX = 2.0 * renderer.unitSize + renderer.cellGap;
		pieceSizeY = renderer.unitSize;
	case 'D':
		pieceSizeX = renderer.unitSize
		pieceSizeY = renderer.unitSize
    default:
		log.Fatalln("cannot draw unknown piece")
	}

	var highlight string
	if step != nil {
		highlight = "Hi"
	} else {
		highlight = "Lo"
	}

	pieceFillColor := renderer.Property(
		fmt.Sprintf("fillColor%sPiece%c", highlight, piece.Tag.Symbol))
	pieceStrokeColor := renderer.Property(
		fmt.Sprintf("strokeColor%sPiece%c", highlight, piece.Tag.Symbol))

	piecePosX, piecePosY := renderer.piecePositionToXYCoord(
		piece.Position.X, piece.Position.Y)

	svg.
		Elem("rect").
		Attr("x", piecePosX).
		Attr("y", piecePosY).
		Attr("width", pieceSizeX).
		Attr("height", pieceSizeY).
		Attr("rx", renderer.borderRadius).
		Attr("fill", pieceFillColor).
		Attr("stroke", pieceStrokeColor).
		Attr("stroke-width", renderer.Property("strokeWidthPiece")).
		Term()

	if step != nil {
		var arrowPath string
		switch step.Vector {
		case pt.StepUp().Vector:    arrowPath = "M 0 -6 L -6 4 L 6 4 Z"
		case pt.StepDown().Vector:  arrowPath = "M 0 6 L -6 -4 L 6 -4 Z"
		case pt.StepLeft().Vector:  arrowPath = "M -6 0 L 4 -6 L 4 6 Z"
		case pt.StepRight().Vector: arrowPath = "M 6 0 L -4 -6 L -4 6 Z"
		default:
			log.Fatalln("cannot draw unknown step")
		}

		arrowPosX := piecePosX + pieceSizeX * 0.5
		arrowPosY := piecePosY + pieceSizeY * 0.5

		svg.
			Elem("path").
			Attr("d", arrowPath).
			Attr("fill", renderer.Property("arrowColor")).
			Attr("transform", fmt.Sprintf("translate(%g %g)", arrowPosX, arrowPosY)).
			Term()
	}
}

func RenderKlotskiGrids(out io.Writer, grid *pt.Grid, moves []pt.Move, theme string) {
	renderer := NewSvgRenderer(grid, theme)
	renderer.CustomRenderGrid = klotskiCustomRenderGrid
	renderer.CustomRenderPiece = klotskiCustomRenderPiece

	renderer.RenderGrids(out, grid, moves)
}
