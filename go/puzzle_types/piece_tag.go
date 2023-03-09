// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package puzzle_types

import (
	"fmt"
)

type PieceTag struct {
	Symbol rune
	Number int
}

func PieceTagEmpty() PieceTag {
	return PieceTag{'*', 0}
}

func PieceTagObstacle() PieceTag {
	return PieceTag{'#', 0}
}

func (pieceTag PieceTag) ToString() string {
	if pieceTag.Symbol == PieceTagEmpty().Symbol {
		return "**"
	}
	if pieceTag.Symbol == PieceTagObstacle().Symbol {
		return "##"
	}
	return fmt.Sprintf("%c%d", pieceTag.Symbol, pieceTag.Number)
}
