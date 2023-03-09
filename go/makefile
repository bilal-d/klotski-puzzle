# SPDX-License-Identifier: MIT
# Copyright © 2023  Bilal Djelassi

GO := go

.PHONY: all
all: klotski_solver.prg

%.prg: %.go
	$(info go.compile $<)
	@$(GO) build -o $@ $<

.PHONY: clean
clean:
	rm -f klotski_solver.prg
