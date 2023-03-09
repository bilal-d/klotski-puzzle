# SPDX-License-Identifier: MIT
# Copyright © 2023  Bilal Djelassi

from puzzle_types import Vect2, Step, PieceTag, Piece, Move, Grid, SearchTree
from svg_renderer import SVGRenderer, create_xml_element, serialize_xml_document

class SearchEdge:
    __slots__ = 'parent_index', 'move'

    def __init__(self, parent_index, move):
        self.parent_index = parent_index
        self.move = move


def solve_puzzle(initial_grid, success_condition, symmetry):
    validated = initial_grid.validate()
    if validated is None:
        raise ValueError("initial grid is invalid")
    if success_condition(initial_grid):
        return (initial_grid, [])

    search_tree = SearchTree()
    search_tree.append(initial_grid, None, validated.key(symmetry))

    while True:
        search_tree.increment_depth()
        a, b = search_tree.current_depth()

        if a == b:
            raise RuntimeError("reached end of tree, no more solutions to explore")

        for parent_index in range(a, b):
            for piece_index in range(len(initial_grid.pieces)):
                for step in Step.ALL:
                    move = Move(piece_index, step)
                    grid = search_tree.node_at(parent_index)
                    grid = grid.copy()
                    grid.apply(move)

                    validated = grid.validate()
                    if validated is None:
                        continue

                    key = validated.key(symmetry)
                    edge = SearchEdge(parent_index, move)
                    if not search_tree.append(grid, edge, key):
                        continue

                    if success_condition(grid):
                        edge = search_tree.edge_at(search_tree.last_index())
                        path = []
                        while edge is not None:
                            path.append(edge.move)
                            edge = search_tree.edge_at(edge.parent_index)
                        return (grid, list(reversed(path)))

def klotski_is_solved(grid):
    for piece in grid.pieces:
        if piece.tag == PieceTag('A', 1):
            return piece.position == Vect2(1, 3)
    return False

def format_move(grid, move):
    piece = grid.pieces[move.piece_index]
    return f"{piece.tag}{move.step.symbol}"


class CustomSVGRenderer(SVGRenderer):
    def __init__(self, grid):
        super().__init__(grid)
        self.properties = SVGRenderer.colorful_theme()

    def custom_render_grid(self, svg_element, grid, title):
        grid_size_x, grid_size_y = self.get_grid_size(False)
        svg_element.append(create_xml_element("rect", {
            "x": 0,
            "y": 0,
            "width": grid_size_x,
            "height": grid_size_y,
            "rx": "{:g}".format(self.border_radius),
            "fill": self.properties["fillColorGrid"],
            "stroke": self.properties["strokeColorGrid"],
            "stroke-width": self.properties["strokeWidthGrid"],
        }))
        if title:
            svg_element.append(create_xml_element("text", {
                "x": grid_size_x * 0.5,
                "y": self.title_height * -0.5,
                "text-anchor": "middle",
                "fill": self.properties["textColor"],
                "font-family": self.properties["fontFamily"],
                "font-size": self.properties["fontSize"],
            }, content=title))

    def custom_render_piece(self, svg_element, piece, step):
        if piece.tag.symbol == 'A':
            piece_size_x = 2 * self.unit_size + self.cell_gap
            piece_size_y = 2 * self.unit_size + self.cell_gap
        elif piece.tag.symbol == 'B':
            piece_size_x = self.unit_size
            piece_size_y = 2 * self.unit_size + self.cell_gap
        elif piece.tag.symbol == 'C':
            piece_size_x = 2 * self.unit_size + self.cell_gap
            piece_size_y = self.unit_size
        elif piece.tag.symbol == 'D':
            piece_size_x = self.unit_size
            piece_size_y = self.unit_size
        else:
            raise ValueError("cannot draw unknown piece")

        highlight = "Hi" if step else "Lo"
        piece_fill_color = self.properties[
            "fillColor{}Piece{}".format(highlight, piece.tag.symbol)]
        piece_stroke_color = self.properties[
            "strokeColor{}Piece{}".format(highlight, piece.tag.symbol)]

        piece_pos_x, piece_pos_y = self.piece_position_to_xy_coord(
            piece.position.x, piece.position.y)

        svg_element.append(create_xml_element("rect", {
            "x": piece_pos_x,
            "y": piece_pos_y,
            "width": piece_size_x,
            "height": piece_size_y,
            "rx": "{:g}".format(self.border_radius),
            "fill": piece_fill_color,
            "stroke": piece_stroke_color,
            "stroke-width": self.properties["strokeWidthPiece"],
        }))
        if step is not None:
            if step == Step.UP:
                arrow_path = "M 0 -6 L -6 4 L 6 4 Z"
            elif step == Step.DOWN:
                arrow_path = "M 0 6 L -6 -4 L 6 -4 Z"
            elif step == Step.LEFT:
                arrow_path = "M -6 0 L 4 -6 L 4 6 Z"
            elif step == Step.RIGHT:
                arrow_path = "M 6 0 L -4 -6 L -4 6 Z"
            else:
                raise ValueError("cannot draw unknown step")

            arrow_pos_x = piece_pos_x + piece_size_x * 0.5
            arrow_pos_y = piece_pos_y + piece_size_y * 0.5
            svg_element.append(create_xml_element("path", {
                "d": arrow_path,
                "fill": self.properties["arrowColor"],
                "transform": "translate({:g} {:g})".format(arrow_pos_x, arrow_pos_y),
            }))


def main():
    piece_cells_A = [Vect2.coerce(cell)
        for cell in ((0, 0), (1, 0), (0, 1), (1, 1))]
    piece_cells_B = [Vect2.coerce(cell)
        for cell in ((0, 0), (0, 1))]
    piece_cells_C = [Vect2.coerce(cell)
        for cell in ((0, 0), (1, 0))]
    piece_cells_D = [Vect2.coerce(cell)
        for cell in ((0, 0),)]

    initial_grid = Grid(4, 5, [
        Piece(('A', 1), (1, 0), piece_cells_A),
        Piece(('B', 1), (0, 0), piece_cells_B),
        Piece(('B', 2), (3, 0), piece_cells_B),
        Piece(('B', 3), (0, 2), piece_cells_B),
        Piece(('B', 4), (3, 2), piece_cells_B),
        Piece(('C', 1), (1, 2), piece_cells_C),
        Piece(('D', 1), (0, 4), piece_cells_D),
        Piece(('D', 2), (1, 3), piece_cells_D),
        Piece(('D', 3), (2, 3), piece_cells_D),
        Piece(('D', 4), (3, 4), piece_cells_D),
    ])

    print(f"initial grid:\n{initial_grid}")

    solved_grid, path = solve_puzzle(
            initial_grid,
            klotski_is_solved,
            Grid.KeySymmetry.HORIZOTAL_SYMMETRY)

    print(f"solved grid:\n{solved_grid}")

    formatted_moves = " ".join(format_move(initial_grid, move) for move in path)
    print(f"list of moves ({len(path)}):\n{formatted_moves}")

    svg = CustomSVGRenderer(initial_grid).render_grids(initial_grid, path)
    with open("klotski_solution.svg", "w") as svg_file:
        svg_file.write(serialize_xml_document(svg))

if __name__ == '__main__':
    main()
