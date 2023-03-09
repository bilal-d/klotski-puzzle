# SPDX-License-Identifier: MIT
# Copyright © 2023  Bilal Djelassi

from xml.etree import ElementTree as ET

def format_xml_attrib(value):
    if isinstance(value, str):
        return value
    if isinstance(value, int):
        return "{}".format(value)
    if isinstance(value, float):
        return "{:g}".format(value)
    raise TypeError("attribute must be str, int or float")

def create_xml_element(name, attribs={}, content="", sub_elements=[]):
    element = ET.Element(name)
    for key, value in attribs.items():
        element.set(key, format_xml_attrib(value))
    if content:
        element.text = content
    if sub_elements:
        element.extend(sub_elements)
    return element

def serialize_xml_document(document):
    return ET.tostring(document,
        xml_declaration=True,
        encoding="unicode")

class SVGRenderer:
    __slots__ = \
        'cell_gap', 'grid_gap', 'unit_size', 'title_height', \
        'offset_x', 'offset_y', 'border_radius', \
        'grids_per_row', 'grid_size_x', 'grid_size_y', \
        'include_background', 'properties'

    def __init__(self, grid):
        self.cell_gap = 4.0
        self.grid_gap = 16.0
        self.unit_size = 20.0
        self.title_height = 12.0
        self.offset_x = 0.5
        self.offset_y = 0.5
        self.border_radius = 2.0
        self.grids_per_row = 10
        self.grid_size_x = grid.size_x
        self.grid_size_y = grid.size_y
        self.include_background = False
        self.properties = {}

    @staticmethod
    def simple_theme():
        return {
            "fontFamily": "'Consolas','PT Mono','DejaVu Serif Mono',monospace",
            "fontSize": "16",
            "strokeWidthGrid": "1",
            "strokeWidthPiece": "1",
            "backgroundColor": "#ffffff",
            "textColor": "#0d1e26",
            "arrowColor": "#ffffff",
            "fillColorGrid": "#ecf4f9",
            "strokeColorGrid": "#8f9ca3",
            "fillColorHiPieceA": "#00aaff",
            "fillColorLoPieceA": "#8f9ca3",
            "fillColorHiPieceB": "#00aaff",
            "fillColorLoPieceB": "#8f9ca3",
            "fillColorHiPieceC": "#00aaff",
            "fillColorLoPieceC": "#8f9ca3",
            "fillColorHiPieceD": "#00aaff",
            "fillColorLoPieceD": "#8f9ca3",
            "strokeColorHiPieceA": "#0d1e26",
            "strokeColorLoPieceA": "#0d1e26",
            "strokeColorHiPieceB": "#0d1e26",
            "strokeColorLoPieceB": "#0d1e26",
            "strokeColorHiPieceC": "#0d1e26",
            "strokeColorLoPieceC": "#0d1e26",
            "strokeColorHiPieceD": "#0d1e26",
            "strokeColorLoPieceD": "#0d1e26",
        }

    @staticmethod
    def colorful_theme():
        return {
            "fontFamily": "'Consolas','PT Mono','DejaVu Serif Mono',monospace",
            "fontSize": "16",
            "strokeWidthGrid": "1",
            "strokeWidthPiece": "1",
            "backgroundColor": "#ffffff",
            "textColor": "#0d1e26",
            "arrowColor": "#ffffff",
            "fillColorGrid": "#ecf4f9",
            "strokeColorGrid": "#8f9ca3",
            "fillColorHiPieceA": "#ec433b",
            "fillColorLoPieceA": "#ec433b",
            "fillColorHiPieceB": "#93a707",
            "fillColorLoPieceB": "#93a707",
            "fillColorHiPieceC": "#3b98e0",
            "fillColorLoPieceC": "#3b98e0",
            "fillColorHiPieceD": "#e5b01a",
            "fillColorLoPieceD": "#e5b01a",
            "strokeColorHiPieceA": "#dc322f",
            "strokeColorLoPieceA": "#dc322f",
            "strokeColorHiPieceB": "#859900",
            "strokeColorLoPieceB": "#859900",
            "strokeColorHiPieceC": "#268bd2",
            "strokeColorLoPieceC": "#268bd2",
            "strokeColorHiPieceD": "#dba700",
            "strokeColorLoPieceD": "#dba700",
        }

    def get_grids_on_xy_axis(self, grid_count):
        if grid_count <= self.grids_per_row:
            grids_on_x_axis = grid_count
            grids_on_y_axis = 1
        else:
            grids_on_x_axis = self.grids_per_row
            grids_on_y_axis = 1 + (grid_count - 1) // self.grids_per_row
        return (grids_on_x_axis, grids_on_y_axis)

    def get_grid_size(self, include_title=False):
        grid_size_x = self.cell_gap \
             + self.grid_size_x * (self.unit_size + self.cell_gap)
        grid_size_y = self.cell_gap \
             + self.grid_size_y * (self.unit_size + self.cell_gap) \
             + self.title_height * include_title
        return (grid_size_x, grid_size_y)

    def get_svg_size(self, grid_count, include_title=False):
        grids_on_x_axis, grids_on_y_axis = self.get_grids_on_xy_axis(grid_count)
        grid_size_x, grid_size_y = self.get_grid_size(include_title)
        svg_size_x = self.grid_gap + grids_on_x_axis * (grid_size_x + self.grid_gap)
        svg_size_y = self.grid_gap + grids_on_y_axis * (grid_size_y + self.grid_gap)
        return (svg_size_x, svg_size_y)

    def grid_position_to_xy_coord(self, x, y, include_title=False):
        grid_size_x, grid_size_y = self.get_grid_size(include_title)
        grid_x = self.offset_x + self.grid_gap + x * (grid_size_x + self.grid_gap)
        grid_y = self.offset_y + self.grid_gap + self.title_height * include_title \
                + y * (grid_size_y + self.grid_gap)
        return (grid_x, grid_y)

    def piece_position_to_xy_coord(self, x, y):
        piece_x = self.cell_gap + x * (self.unit_size + self.cell_gap)
        piece_y = self.cell_gap + y * (self.unit_size + self.cell_gap)
        return (piece_x, piece_y)

    def next_grid_position(self, x, y):
        return (0, y+1) if x+1 >= self.grids_per_row else (x+1, y)

    def render_grid(self, grid):
        svg_size_x, svg_size_y = self.get_svg_size(1, False)

        svg = create_xml_element("svg", {
            "width": svg_size_x,
            "height": svg_size_y,
            "xmlns": "http://www.w3.org/2000/svg"
        })

        if self.include_background:
            svg.append(create_xml_element("rect", {
                "width": "100%",
                "height": "100%",
                "fill": self.properties["backgroundColor"],
}))

        self.render(svg, (0, 0), grid)
        return svg

    def render_grids(self, grid, moves):
        grid_count = 1 + len(moves) + 1
        svg_size_x, svg_size_y = self.get_svg_size(grid_count, include_title=True)

        svg = create_xml_element("svg", {
            "width": svg_size_x,
            "height": svg_size_y,
            "xmlns": "http://www.w3.org/2000/svg",
        })

        if self.include_background:
            svg.append(create_xml_element("rect", {
                "width": "100%",
                "height": "100%",
                "fill": self.properties["backgroundColor"],
            }))

        grid_pos_x, grid_pos_y = (0, 0)
        current_grid = grid.copy()
        self.render(svg, grid_pos_x, grid_pos_y, current_grid, None, "début")

        for index, move in enumerate(moves):
            title = "étape {}".format(index + 1)
            grid_pos_x, grid_pos_y = self.next_grid_position(grid_pos_x, grid_pos_y)
            self.render(svg, grid_pos_x, grid_pos_y, current_grid, move, title)
            current_grid.apply(move)

        grid_pos_x, grid_pos_y = self.next_grid_position(grid_pos_x, grid_pos_y)
        self.render(svg, grid_pos_x, grid_pos_y, current_grid, None, "fin")

        return svg

    def render(self, svg, grid_pos_x, grid_pos_y, grid, move, title):
        grid_coord_x, grid_coord_y = \
            self.grid_position_to_xy_coord(grid_pos_x, grid_pos_y, title is not None)

        svg_element = create_xml_element("g", {
            "transform": "translate({:g} {:g})".format(grid_coord_x, grid_coord_y),
        })
        self.custom_render_grid(svg_element, grid, title)

        for index, piece in enumerate(grid.pieces):
            if move is not None and move.piece_index == index:
                self.custom_render_piece(svg_element, piece, move.step)
            else:
                self.custom_render_piece(svg_element, piece, None)

        svg.append(svg_element)


    def custom_render_grid(self, svg_element, grid, title):
        raise NotImplemented("must be implemented in a derived class")

    def custom_render_piece(self, svg_element, piece, step):
        raise NotImplemented("must be implemented in a derived class")

