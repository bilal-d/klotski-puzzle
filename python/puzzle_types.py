# SPDX-License-Identifier: MIT
# Copyright © 2023  Bilal Djelassi

import enum

class Vect2:
    __slots__ = 'x', 'y'

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __eq__(self, other):
        return isinstance(other, Vect2) \
            and self.x == other.x \
            and self.y == other.y

    def __add__(self, other):
        return Vect2(self.x + other.x, self.y + other.y)

    def __str__(self):
        return f"({self.x}, {self.y})"

    def __repr__(self):
        return f"Vect2(x={self.x}, y={self.y})"

    @staticmethod
    def coerce(value):
        if isinstance(value, Vect2):
            return value
        if isinstance(value, (tuple, list)) and len(value) == 2:
            return Vect2(value[0], value[1])
        raise TypeError(f"value of unexpected type")


class PieceTag:
    __slots__ = 'symbol', 'number'

    def __init__(self, symbol, number):
        self.symbol = symbol
        self.number = number

    def __eq__(self, other):
        return isinstance(other, PieceTag) \
            and self.symbol == other.symbol \
            and self.number == other.number

    def __str__(self):
        if self.symbol == PieceTag.EMPTY.symbol:
            return "**"
        if self.symbol == PieceTag.OBSTACLE.symbol:
            return "##"
        return f"{self.symbol}{self.number}"

    def __repr__(self):
        return f"PieceTag(symbol={self.symbol}, number={self.number})"

    @staticmethod
    def coerce(value):
        if isinstance(value, PieceTag):
            return value
        if isinstance(value, (tuple, list)) and len(value) == 2:
            return PieceTag(value[0], value[1])
        raise TypeError(f"value of unexpected type")


PieceTag.EMPTY = PieceTag('*', 0)
PieceTag.OBSTACLE = PieceTag('#', 0)

class Piece:
    __slots__ = 'tag', 'position', 'geometry'

    def __init__(self, tag, position, geometry):
        self.tag = PieceTag.coerce(tag)
        self.position = Vect2.coerce(position)
        self.geometry = geometry

    def __str__(self):
        return f"Piece {self.tag} @ {self.position}"

    def copy(self):
        return Piece(self.tag, self.position, self.geometry)

    def cells(self):
        return (self.position + cell for cell in self.geometry)


class Step(enum.Enum):
    __slots__ = 'symbol', 'vector'

    def __init__(self, symbol, vector):
        self.symbol = symbol
        self.vector = vector

    UP    = ("↑", Vect2(0, -1))
    DOWN  = ("↓", Vect2(0, +1))
    LEFT  = ("←", Vect2(-1, 0))
    RIGHT = ("→", Vect2(+1, 0))

Step.ALL = (Step.UP, Step.DOWN, Step.LEFT, Step.RIGHT)


class Move:
    __slots__ = 'piece_index', 'step'

    def __init__(self, piece_index, step):
        self.piece_index = piece_index
        self.step = step


class Grid:
    __slots__ = 'size_x', 'size_y', 'pieces'

    def __init__(self, size_x, size_y, pieces):
        self.size_x = size_x
        self.size_y = size_y
        self.pieces = [piece.copy() for piece in pieces]

    def copy(self):
        return Grid(self.size_x, self.size_y, self.pieces)

    def contains(self, position):
        return 0 <= position.x < self.size_x \
           and 0 <= position.y < self.size_y

    def validate(self):
        placement = Grid.Cells(self)
        return placement if placement.cells else None

    def apply(self, move):
        piece = self.pieces[move.piece_index]
        piece.position = piece.position + move.step.vector

    def __str__(self):
        placement = Grid.Cells(self)
        if placement.cells is None:
            return "| INVALID"

        return "\n".join("|  " +
            "  ".join(str(placement.cells[y][x])
                for x in range(self.size_x))
                for y in range(self.size_y))

    class KeySymmetry(enum.Enum):
        NO_SYMMETRY = enum.auto()
        HORIZOTAL_SYMMETRY = enum.auto()

    class Cells:
        __slots__ = 'size_x', 'size_y', 'cells'

        def __init__(self, grid):
            # create an empty rectangular grid of cells
            self.size_x = grid.size_x
            self.size_y = grid.size_y
            self.cells = [[PieceTag.EMPTY
                for x in range(self.size_x)]
                for y in range(self.size_y)]

            # place all pieces
            for piece in grid.pieces:

                # if a piece is outside the grid, bail out
                for position in piece.cells():
                    if not grid.contains(position):
                        self.cells = None
                        return

                    # if a piece collides with another, bail out
                    x, y = position.x, position.y
                    if self.cells[y][x] != PieceTag.EMPTY:
                        self.cells = None
                        return

                    self.cells[y][x] = piece.tag

        def key(self, symmetry=None):
            if symmetry is None or symmetry == Grid.KeySymmetry.NO_SYMMETRY:
                return "".join(self.cells[y][x].symbol
                    for x in range(self.size_x)
                    for y in range(self.size_y))

            if symmetry == Grid.KeySymmetry.HORIZOTAL_SYMMETRY:
                key1 = "".join(self.cells[y][x].symbol
                    for x in range(self.size_x)
                    for y in range(self.size_y))

                key2 = "".join(self.cells[y][x].symbol
                    for x in reversed(range(self.size_x))
                    for y in range(self.size_y))

                return min(key1, key2)

            raise ValueError(f"unsupported symmetry: {symmetry}")


class SearchTree:
    __slots__ = 'nodes', 'edges', 'levels', 'keys'

    def __init__(self):
        self.nodes = []
        self.edges = []
        self.levels = []
        self.keys = set()

    def append(self, node, edge, key):
        if key in self.keys:
            return False
        self.nodes.append(node)
        self.edges.append(edge)
        self.keys.add(key)
        return True

    def increment_depth(self):
        a = 0 if not self.levels else self.levels[-1][1]
        b = len(self.edges)
        self.levels.append((a, b))
        self.nodes = self.nodes[a-b:]

    def current_depth(self):
        return (0, 0) if not self.levels else self.levels[-1]

    def node_at(self, index):
        offset = 0 if not self.levels else self.levels[-1][0]
        if not (0 <= (index - offset) < len(self.nodes)):
            raise IndexError("out-of-bounds access to node")
        return self.nodes[index - offset]

    def edge_at(self, index):
        if not (0 <= index < len(self.edges)):
            raise IndexError("out-of-bound access to edge")
        return self.edges[index]

    def last_index(self):
        return None if not self.edges else len(self.edges) - 1

