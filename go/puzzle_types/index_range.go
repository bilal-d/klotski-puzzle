// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package puzzle_types

type IndexRange struct {
	A int
	B int
}

func (indexRange IndexRange) IsEmpty() bool {
	return indexRange.B <= indexRange.A
}
