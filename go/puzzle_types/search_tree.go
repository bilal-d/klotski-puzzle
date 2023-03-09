// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package puzzle_types

type SearchTree struct {
	nodes []any
	edges []any
	levels []IndexRange
	keys map[string]bool
}

func NewSearchTree() *SearchTree {
	return &SearchTree{
		nodes: make([]any, 0),
		edges: make([]any, 0),
		levels: make([]IndexRange, 0),
		keys: make(map[string]bool),
	}
}

func (searchTree *SearchTree) Append(node any, edge any, key string) bool {
	if searchTree.keys[key] {
		return false
	}

	searchTree.nodes = append(searchTree.nodes, node)
	searchTree.edges = append(searchTree.edges, edge)
	searchTree.keys[key] = true
	return true
}

func (searchTree *SearchTree) IncrementDepth() {
	a := 0
	if levelsCount := len(searchTree.levels); levelsCount != 0 {
		a = searchTree.levels[levelsCount - 1].B
	}
	b := len(searchTree.edges)
	searchTree.levels = append(searchTree.levels, IndexRange{a, b})
	nodesCount := len(searchTree.nodes)
	searchTree.nodes = searchTree.nodes[nodesCount-(b-a):nodesCount]
}

func (searchTree *SearchTree) CurrentDepth() IndexRange {
	if levelsCount := len(searchTree.levels); levelsCount != 0 {
		return searchTree.levels[levelsCount - 1];
	}
	return IndexRange{0, 0}
}

func (searchTree *SearchTree) NodeAt(index int) any {
	offset := 0
	if len(searchTree.levels) != 0 {
		offset = searchTree.levels[len(searchTree.levels) - 1].A
	}
	return searchTree.nodes[index - offset]
}

func (searchTree *SearchTree) EdgeAt(index int) any {
	return searchTree.edges[index]
}

func (searchTree *SearchTree) LastIndex() int {
	if elemsCount := len(searchTree.edges); elemsCount != 0 {
		return elemsCount - 1
	}
	panic("empty search tree, last index not available")
}
