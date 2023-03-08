// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

#ifndef SVG_RENDERER_HPP_INCLUDED
#define SVG_RENDERER_HPP_INCLUDED

#include "puzzle_types.hpp"
#include "xml_writer.hpp"
#include <format>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>


template<typename Grid>
struct SVGRenderer
{
    float cellGap;
    float gridGap;
    float unitSize;
    float titleHeight;
    float offsetX;
    float offsetY;
    float borderRadius;
    int   gridsPerRow;
    bool  includeBackground;
    std::unordered_map<std::string, std::string> properties;

    SVGRenderer() {
        cellGap = 4.0f;
        gridGap = 16.0f;
        unitSize = 20.0f;
        titleHeight = 12.0f;
        offsetX = 0.5f;
        offsetY = 0.5f;
        borderRadius = 2.0f;
        gridsPerRow = 10;
        includeBackground = false;
    }
    virtual ~SVGRenderer() {}

    static std::unordered_map<std::string, std::string> simpleTheme() {
        return {
            { "fontFamily", "'Consolas','PT Mono','DejaVu Serif Mono',monospace" },
            { "fontSize", "16" },
            { "strokeWidthGrid", "1" },
            { "strokeWidthPiece", "1" },
            { "backgroundColor", "#ffffff" },
            { "textColor", "#0d1e26" },
            { "arrowColor", "#ffffff" },
            { "fillColorGrid", "#ecf4f9" },
            { "strokeColorGrid", "#8f9ca3" },
            { "fillColorHiPieceA", "#00aaff" },
            { "fillColorLoPieceA", "#8f9ca3" },
            { "fillColorHiPieceB", "#00aaff" },
            { "fillColorLoPieceB", "#8f9ca3" },
            { "fillColorHiPieceC", "#00aaff" },
            { "fillColorLoPieceC", "#8f9ca3" },
            { "fillColorHiPieceD", "#00aaff" },
            { "fillColorLoPieceD", "#8f9ca3" },
            { "fillColorHiPieceE", "#00aaff" },
            { "fillColorLoPieceE", "#8f9ca3" },
            { "fillColorHiPieceF", "#00aaff" },
            { "fillColorLoPieceF", "#8f9ca3" },
            { "fillColorHiPieceG", "#00aaff" },
            { "fillColorLoPieceG", "#8f9ca3" },
            { "fillColorHiPieceH", "#00aaff" },
            { "fillColorLoPieceH", "#8f9ca3" },
            { "strokeColorHiPieceA", "#0d1e26" },
            { "strokeColorLoPieceA", "#0d1e26" },
            { "strokeColorHiPieceB", "#0d1e26" },
            { "strokeColorLoPieceB", "#0d1e26" },
            { "strokeColorHiPieceC", "#0d1e26" },
            { "strokeColorLoPieceC", "#0d1e26" },
            { "strokeColorHiPieceD", "#0d1e26" },
            { "strokeColorLoPieceD", "#0d1e26" },
            { "strokeColorHiPieceE", "#0d1e26" },
            { "strokeColorLoPieceE", "#0d1e26" },
            { "strokeColorHiPieceF", "#0d1e26" },
            { "strokeColorLoPieceF", "#0d1e26" },
            { "strokeColorHiPieceG", "#0d1e26" },
            { "strokeColorLoPieceG", "#0d1e26" },
            { "strokeColorHiPieceH", "#0d1e26" },
            { "strokeColorLoPieceH", "#0d1e26" },
        };
    }

    static std::unordered_map<std::string, std::string> colorfulTheme() {
        return {
            { "fontFamily", "'Consolas','PT Mono','DejaVu Serif Mono',monospace" },
            { "fontSize", "16" },
            { "strokeWidthGrid", "1" },
            { "strokeWidthPiece", "1" },
            { "backgroundColor", "#ffffff" },
            { "textColor", "#0d1e26" },
            { "arrowColor", "#ffffff" },
            { "fillColorGrid", "#ecf4f9" },
            { "strokeColorGrid", "#8f9ca3" },
            { "fillColorHiPieceA", "#ec433b" },
            { "fillColorLoPieceA", "#ec433b" },
            { "fillColorHiPieceB", "#93a707" },
            { "fillColorLoPieceB", "#93a707" },
            { "fillColorHiPieceC", "#3b98e0" },
            { "fillColorLoPieceC", "#3b98e0" },
            { "fillColorHiPieceD", "#e5b01a" },
            { "fillColorLoPieceD", "#e5b01a" },
            { "fillColorHiPieceE", "#f0429a" },
            { "fillColorLoPieceE", "#f0429a" },
            { "fillColorHiPieceF", "#f0429a" },
            { "fillColorLoPieceF", "#f0429a" },
            { "fillColorHiPieceG", "#f0429a" },
            { "fillColorLoPieceG", "#f0429a" },
            { "fillColorHiPieceH", "#f0429a" },
            { "fillColorLoPieceH", "#f0429a" },
            { "strokeColorHiPieceA", "#dc322f" },
            { "strokeColorLoPieceA", "#dc322f" },
            { "strokeColorHiPieceB", "#859900" },
            { "strokeColorLoPieceB", "#859900" },
            { "strokeColorHiPieceC", "#268bd2" },
            { "strokeColorLoPieceC", "#268bd2" },
            { "strokeColorHiPieceD", "#dba700" },
            { "strokeColorLoPieceD", "#dba700" },
            { "strokeColorHiPieceE", "#e8318c" },
            { "strokeColorLoPieceE", "#e8318c" },
            { "strokeColorHiPieceF", "#e8318c" },
            { "strokeColorLoPieceF", "#e8318c" },
            { "strokeColorHiPieceG", "#e8318c" },
            { "strokeColorLoPieceG", "#e8318c" },
            { "strokeColorHiPieceH", "#e8318c" },
            { "strokeColorLoPieceH", "#e8318c" },
        };
    }

    std::string property(std::string const& key) const {
        auto const keyValue = properties.find(key);
        if (keyValue != properties.end())
            return keyValue->second;
        throw std::runtime_error("cannot find property " + key);
    }

    void renderGrid(std::ostream& out, Grid const& grid) const {
        float const svgSizeX = getSVGSizeX(1);
        float const svgSizeY = getSVGSizeY(1, false);

        XmlWriter svg(out);
        svg.decl()
           .root("svg")
           .attr("width", std::format("{}", svgSizeX))
           .attr("height", std::format("{}", svgSizeY))
           .attr("xmlns", "http://www.w3.org/2000/svg")
           .attr("xmlns:xlink", "http://www.w3.org/1999/xlink");

        preRender(svg, grid);

        if (includeBackground) {
            svg.elem("rect")
               .attr("width", "100%")
               .attr("height", "100%")
               .attr("fill", property("backgroundColor"))
               .term();
        }

        render(svg, 0, 0, grid, std::nullopt, std::nullopt);
        svg.term();
        if (!svg.success())
            throw std::runtime_error("error while generating SVG");
    }

    void renderGrids(std::ostream& out, Grid const& grid, std::vector<Move> const& path) const {
        int const gridCount = 1 + path.size() + 1;
        int const gridsOnXAxis = getGridsOnXAxis(gridCount);
        int const gridsOnYAxis = getGridsOnYAxis(gridCount);

        float const svgSizeX = getSVGSizeX(gridsOnXAxis);
        float const svgSizeY = getSVGSizeY(gridsOnYAxis, true);

        XmlWriter svg(out);
        svg.decl()
           .root("svg")
           .attr("width", std::format("{}", svgSizeX))
           .attr("height", std::format("{}", svgSizeY))
           .attr("xmlns", "http://www.w3.org/2000/svg")
           .attr("xmlns:xlink", "http://www.w3.org/1999/xlink");

        preRender(svg, grid);


        if (includeBackground) {
            svg.elem("rect")
               .attr("width", "100%")
               .attr("height", "100%")
               .attr("fill", property("backgroundColor"))
               .term();
        }

        int gridPosX = 0;
        int gridPosY = 0;
        Grid currentGrid = grid;
        render(svg, gridPosX, gridPosY, currentGrid, std::nullopt, "début");

        for (size_t moveIndex = 0; moveIndex < path.size(); moveIndex += 1) {
            auto const& move = path[moveIndex];
            auto const& title = std::format("étape {}", moveIndex + 1);

            nextGridPosition(gridPosX, gridPosY);
            render(svg, gridPosX, gridPosY, currentGrid, move, title);
            currentGrid.apply(move);
        }

        nextGridPosition(gridPosX, gridPosY);
        render(svg, gridPosX, gridPosY, currentGrid, std::nullopt, "fin");

        svg.term();
        if (!svg.success())
            throw std::runtime_error("error while generating SVG");
    }

protected:
    virtual void preRender(XmlWriter&, Grid const&) const = 0;
    virtual void render(XmlWriter&, Grid const&, std::optional<std::string>) const = 0;
    virtual void render(XmlWriter&, Piece const&, std::optional<Step>) const = 0;

    int getGridsOnXAxis(int gridCount) const {
        return gridCount <= gridsPerRow ? gridCount : gridsPerRow;
    }
    int getGridsOnYAxis(int gridCount) const {
        return gridCount <= gridsPerRow ? 1 : 1 + (gridCount - 1) / gridsPerRow;
    }
    void nextGridPosition(int& x, int& y) const {
        if (++x >= gridsPerRow) {
            x = 0; ++y;
        }
    }
    float getGridSizeX() const {
        return cellGap + Grid::sizeX * (unitSize + cellGap);
    }
    float getGridSizeY(bool includeTitle = false) const {
        return cellGap + Grid::sizeY * (unitSize + cellGap) + includeTitle * titleHeight;
    }
    float getSVGSizeX(int gridsOnXAxis) const {
        return gridGap + (getGridSizeX() + gridGap) * gridsOnXAxis;
    }
    float getSVGSizeY(int gridsOnYAxis, bool includeTitle = false) const {
        return gridGap + (getGridSizeY(includeTitle) + gridGap) * gridsOnYAxis;
    }
    float gridPositionToXCoord(int x) const {
        return offsetX + gridGap + x * (getGridSizeX() + gridGap);
    }
    float gridPositionToYCoord(int y, bool includeTitle = false) const {
        return offsetY + includeTitle * titleHeight + gridGap
             + y * (getGridSizeY(includeTitle) + gridGap);
    }
    float piecePositionToXCoord(int x) const {
        return cellGap + x * (unitSize + cellGap);
    }
    float piecePositionToYCoord(int y) const {
        return cellGap + y * (unitSize + cellGap);
    }

private:
    void render(
            XmlWriter& svg,
            int gridPosX,
            int gridPosY,
            Grid const& grid,
            std::optional<Move> move,
            std::optional<std::string> title) const {

        float const gridCoordX = gridPositionToXCoord(gridPosX);
        float const gridCoordY = gridPositionToYCoord(gridPosY, bool(title));

        svg.elem("g")
           .attr("transform", std::format("translate({} {})", gridCoordX, gridCoordY));

        render(svg, grid, title);
        for (size_t pieceIndex = 0; pieceIndex < grid.pieces.size(); ++pieceIndex) {
            auto const& piece = grid.pieces[pieceIndex];

            if (move && move->pieceIndex == pieceIndex)
                render(svg, piece, move->step);
            else
                render(svg, piece, std::nullopt);
        }
        svg.term();
    }
};

#endif  // SVG_RENDERER_HPP_INCLUDED
