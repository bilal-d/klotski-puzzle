// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

#include "puzzle_types.hpp"
#include "svg_renderer.hpp"
#include "xml_writer.hpp"
#include <algorithm>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>


using KlotskiGrid = Grid<4, 5>;

struct SearchEdge
{
    size_t parentIndex;
    Move move;
};

using KlotskiSearchTree =
    SearchTree<KlotskiGrid, std::optional<SearchEdge>, std::string>;

struct KlotskiSolution
{
    KlotskiGrid grid;
    std::vector<Move> path;
};

KlotskiSolution solvePuzzle(
    KlotskiGrid const& initialGrid,
    std::function<bool (KlotskiGrid const&)> successCondition,
    KlotskiGrid::KeySymmetry symmetry)
{
    auto const validated = initialGrid.validate();
    if (!validated)
        throw std::runtime_error("initial grid is invalid");

    if (successCondition(initialGrid))
        return { initialGrid, {} };

    KlotskiSearchTree searchTree;
    searchTree.append(initialGrid, std::nullopt, validated->key(symmetry));

    while (true) {
        searchTree.incrementDepth();
        auto const indexRange = searchTree.currentDepth();

        if (indexRange.isEmpty())
            throw std::runtime_error("reached end of tree, no more solutions to explore");

        // loop over last reached grids ...
        for (size_t const parentIndex : indexRange)

        // ... for each piece ...
        for (size_t const pieceIndex : IndexRange{0, initialGrid.pieces.size()})

        // ... and by trying each step as a move
        for (auto const& step : Step::all()) {
            auto const move = Move{pieceIndex, step};
            KlotskiGrid grid = searchTree.nodeAt(parentIndex);
            grid.apply(move);

            auto const validated = grid.validate();
            if (!validated)
                continue;

            auto const key = validated->key(symmetry);
            if (!searchTree.append(grid, SearchEdge{parentIndex, move}, key))
                continue;

            if (successCondition(grid)) {
                KlotskiSolution solution = {};
                solution.grid = grid;

                for (auto edge = searchTree.edgeAt(searchTree.lastIndex());
                          edge != std::nullopt;
                          edge = searchTree.edgeAt(edge->parentIndex))
                    solution.path.push_back(edge->move);

                std::reverse(solution.path.begin(), solution.path.end());
                return solution;
            }
        }
    }
}

struct KlotskiSVGRenderer : public SVGRenderer<KlotskiGrid>
{
    KlotskiSVGRenderer() {
        properties = colorfulTheme();
        pieceDefs = {
            { 'A', 2 * unitSize + cellGap, 2 * unitSize + cellGap },
            { 'B', unitSize, 2 * unitSize + cellGap },
            { 'C', 2 * unitSize + cellGap, unitSize },
            { 'D', unitSize, unitSize },
        };
        stepDefs = {
            { Step::up().vector,    "stepUp",    "M 0 -6 L -6 4 L 6 4 Z" },
            { Step::down().vector,  "stepDown",  "M 0 6 L -6 -4 L 6 -4 Z" },
            { Step::left().vector,  "stepLeft",  "M -6 0 L 4 -6 L 4 6 Z" },
            { Step::right().vector, "stepRight", "M 6 0 L -4 -6 L -4 6 Z" },
        };
    }

private:
    struct PieceDef {
        char symbol;
        float sizeX;
        float sizeY;
    };
    struct StepDef {
        Vect2 vector;
        std::string id;
        std::string path;
    };

    std::vector<PieceDef> pieceDefs;
    std::vector<StepDef> stepDefs;

protected:
    PieceDef const& getPieceDef(char symbol) const {
        auto iter = std::find_if(pieceDefs.cbegin(), pieceDefs.cend(),
        [&](PieceDef const& pieceDef) {
            return pieceDef.symbol == symbol;
        });
        if (iter == pieceDefs.cend())
            throw std::runtime_error("cannot access requested piece definition");
        return *iter;
    }

    StepDef const& getStepDef(Vect2 const& vector) const {
        auto iter = std::find_if(stepDefs.cbegin(), stepDefs.cend(),
        [&](StepDef const& stepDef) {
            return stepDef.vector == vector;
        });
        if (iter == stepDefs.cend())
            throw std::runtime_error("cannot access requested step definition");
        return *iter;
    }

    void preRender(XmlWriter& svg, KlotskiGrid const& grid) const override {
        using std::format;
        svg.elem("defs");

        // title style
        svg.elem("style")
           .attr("type", "text/css")
           .text(format(
              "text.title {{"
              "text-anchor: middle;"
              "fill: {};"
              "font-family: {};"
              "font-size: {};"
              "}}",
               property("textColor"),
               property("fontFamily"),
               property("fontSize")))
           .term();

        // prerender grid
        svg.elem("rect")
           .attr("id", "grid")
           .attr("x", "0")
           .attr("y", "0")
           .attr("width", format("{}", getGridSizeX()))
           .attr("height", format("{}", getGridSizeY(false)))
           .attr("rx", format("{}", borderRadius))
           .attr("fill", property("fillColorGrid"))
           .attr("stroke", property("strokeColorGrid"))
           .attr("stroke-width", property("strokeWidthGrid"))
           .term();

        // if grid has obstacles, include prerendered obstacle
        if (!grid.obstacles.empty()) {
            svg.elem("pattern")
               .attr("id", "hachures")
               .attr("width", "4")
               .attr("height", "4")
               .attr("patternUnits", "userSpaceOnUse")
               .attr("patternTransform", "rotate(-45)")
                  .elem("line")
                  .attr("x1", "0")
                  .attr("y1", "1")
                  .attr("x2", "4")
                  .attr("y2", "1")
                  .attr("stroke", property("strokeColorGrid"))
                  .attr("stroke-width", "2")
                  .term()
               .term()
               .elem("rect")
               .attr("id", "obstacle")
               .attr("width", format("{}", unitSize))
               .attr("height", format("{}", unitSize))
               .attr("rx", format("{}", borderRadius))
               .attr("fill", "use(#hachures)")
               .term();
        }

        // prerender pieces and steps
        for (auto const& pieceDef : pieceDefs) {
            svg.elem("rect")
               .attr("id", format("piece{}", pieceDef.symbol))
               .attr("x", "0")
               .attr("y", "0")
               .attr("width", format("{}", pieceDef.sizeX))
               .attr("height", format("{}", pieceDef.sizeY))
               .attr("rx", format("{}", borderRadius))
               .attr("fill", property(format("fillColorLoPiece{}", pieceDef.symbol)))
               .attr("stroke", property(format("strokeColorLoPiece{}", pieceDef.symbol)))
               .attr("stroke-width", property("strokeWidthPiece"))
               .term()
               .elem("rect")
               .attr("id", format("piece{}_", pieceDef.symbol))
               .attr("x", "0")
               .attr("y", "0")
               .attr("width", format("{}", pieceDef.sizeX))
               .attr("height", format("{}", pieceDef.sizeY))
               .attr("rx", format("{}", borderRadius))
               .attr("fill", property(format("fillColorHiPiece{}", pieceDef.symbol)))
               .attr("stroke", property(format("strokeColorHiPiece{}", pieceDef.symbol)))
               .attr("stroke-width", property("strokeWidthPiece"))
               .term();
        }

        float const stepScale = unitSize / 20.0f;
        for (auto const& stepDef : stepDefs) {
            svg.elem("path")
               .attr("id", stepDef.id)
               .attr("d", stepDef.path)
               .attr("fill", property("arrowColor"));
            if (stepScale != 1.0f)
                svg.attr("transform", format("scale({})", stepScale));
            svg.term();
        }
        svg.term();
    }

    void render(XmlWriter& svg, KlotskiGrid const&,
        std::optional<std::string> title) const override {

        using std::format;

        svg.elem("use")
           .attr("xlink:href", "#grid")
           .term();

        if (title) {
            svg.elem("text")
               .attr("x", format("{}", getGridSizeX() * 0.5f))
               .attr("y", format("{}", titleHeight * -0.5f))
               .attr("class", "title")
               .text(title.value())
               .term();
        }
    }

    void render(XmlWriter& svg, Piece const& piece,
        std::optional<Step> step) const override {

        using std::format;

        std::string const pieceId =
            format("piece{}{}", piece.tag.symbol, step ? "_" : "");

        float const piecePosX = piecePositionToXCoord(piece.position.x);
        float const piecePosY = piecePositionToYCoord(piece.position.y);

        svg.elem("use")
           .attr("xlink:href", format("#{}", pieceId))
           .attr("transform", format("translate({} {})", piecePosX, piecePosY))
           .term();

        if (step) {
            StepDef const& stepDef = getStepDef(step->vector);
            PieceDef const& pieceDef = getPieceDef(piece.tag.symbol);

            float const stepPosX = piecePosX + pieceDef.sizeX * 0.5f;
            float const stepPosY = piecePosY + pieceDef.sizeY * 0.5f;

            svg.elem("use")
               .attr("xlink:href", format("#{}", stepDef.id))
               .attr("transform", format("translate({} {})", stepPosX, stepPosY))
               .term();
        }
    }
};


int main(int, char*[])
{
    Vect2 constexpr pieceCellsA[] = { {0, 0}, {1, 0}, {0, 1}, {1, 1}, };
    Vect2 constexpr pieceCellsB[] = { {0, 0}, {0, 1}, };
    Vect2 constexpr pieceCellsC[] = { {0, 0}, {1, 0}, };
    Vect2 constexpr pieceCellsD[] = { {0, 0}, };

    KlotskiGrid startingGrid = {};
    startingGrid.pieces = {
        { {'A', 1}, {1, 0}, pieceCellsA, },
        { {'B', 1}, {0, 0}, pieceCellsB, },
        { {'B', 2}, {3, 0}, pieceCellsB, },
        { {'B', 3}, {0, 2}, pieceCellsB, },
        { {'B', 4}, {3, 2}, pieceCellsB, },
        { {'C', 1}, {1, 2}, pieceCellsC, },
        { {'D', 1}, {0, 4}, pieceCellsD, },
        { {'D', 2}, {1, 3}, pieceCellsD, },
        { {'D', 3}, {2, 3}, pieceCellsD, },
        { {'D', 4}, {3, 4}, pieceCellsD, },
    };

    std::cout << "initial grid:" << startingGrid << "\n";

    try {
        KlotskiSolution solution = solvePuzzle(
            startingGrid,
            [](KlotskiGrid const& grid) {
                for (auto const& piece : grid.pieces)
                    if (piece.tag == PieceTag{'A', 1})
                        return piece.position == Vect2{1, 3};
                return false;
            },
            KlotskiGrid::HorizontalSymmetry);

        std::cout << "solved grid:" << solution.grid << "\n";
        std::cout << "list of moves (" << solution.path.size() << "):\n";
        for (auto const& move : solution.path) {
            auto const& piece = startingGrid.pieces[move.pieceIndex];
            std::cout << piece.name() << move.step.toString() << " ";
        }
        std::cout << "\n";

        auto const filename = "klotski_solution.svg";
        std::ofstream svgFile(filename, std::ios::out | std::ios::binary);
        if (!svgFile.is_open()) {
            std::cerr << "could not open svg file in write mode\n";
            return 1;
        }
        KlotskiSVGRenderer{}.renderGrids(svgFile, startingGrid, solution.path);
        svgFile.close();
        return 0;
    }
    catch (std::exception const& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}
