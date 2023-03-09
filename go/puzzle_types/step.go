// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package puzzle_types

type Step struct {
	Symbol string
	Vector Vect2
}

func StepUp() Step {
	return Step{"↑", Vect2{0, -1}}
}

func StepDown() Step {
	return Step{"↓", Vect2{0, +1}}
}

func StepLeft() Step {
	return Step{"←", Vect2{-1, 0}}
}

func StepRight() Step {
	return Step{"→", Vect2{+1, 0}}
}

func StepAll() [4]Step {
	return [4]Step{
		StepUp(),
		StepDown(),
		StepLeft(),
		StepRight(),
	}
}
