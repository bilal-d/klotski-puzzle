// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package puzzle_types

type Vect2 struct {
	X int
	Y int
}

func (lhs Vect2) Add(rhs Vect2) Vect2 {
	return Vect2{
		lhs.X + rhs.X,
		lhs.Y + rhs.Y,
	}
}
