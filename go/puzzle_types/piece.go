// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package puzzle_types

type Piece struct {
	Tag PieceTag
	Position Vect2
	Geometry []Vect2
}

func (piece Piece) Name() string {
	return piece.Tag.ToString()
}
