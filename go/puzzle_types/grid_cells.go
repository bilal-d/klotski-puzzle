// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package puzzle_types

type gridCells struct {
	sizeX int
	sizeY int
	tags []PieceTag
}

type KeySymmetry int
const (
	NoKeySymmetry KeySymmetry = iota
	HorizontalKeySymmetry
)

func (cells *gridCells) contains(position Vect2) bool {
	return 0 <= position.X && position.X < cells.sizeX &&
		0 <= position.Y && position.Y < cells.sizeY
}

func (cells *gridCells) Key(symmetry KeySymmetry) string {
	switch symmetry {

	case NoKeySymmetry:
		key := make([]rune, len(cells.tags))
		for index := range(key) {
			key[index] = cells.tags[index].Symbol
		}
		return string(key)

	case HorizontalKeySymmetry:
		key1 := make([]rune, len(cells.tags))
		key2 := make([]rune, len(cells.tags))
		for y := 0; y < cells.sizeY; y++ {
			for x := 0; x < cells.sizeX; x++ {
				index1 := x + cells.sizeX * y
				index2 := cells.sizeX - 1 - x + cells.sizeX * y

				key1[index1] = cells.tags[index1].Symbol
				key2[index2] = cells.tags[index1].Symbol
			}
		}
		keyStr1 := string(key1)
		keyStr2 := string(key2)
		if keyStr1 < keyStr2 {
			return keyStr1
		}
		return keyStr2

	default:
		panic("unsupported key symmetry")
	}
}

