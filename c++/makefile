# SPDX-License-Identifier: MIT
# Copyright © 2023  Bilal Djelassi

CXX := c++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Wpedantic -Wfatal-errors

.PHONY: all
all: klotski_solver.prg

klotski_solver.prg: klotski_solver.obj
	$(info c++.link $@)
	@$(CXX) $(CXXFLAGS) -o $@ $+

%.obj: %.cpp
	$(info c++.compile $<)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f klotski_solver.prg
	rm -f klotski_solver.obj
