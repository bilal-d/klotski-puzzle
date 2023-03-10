// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

#ifndef PUZZLE_TYPES_HPP_INCLUDED
#define PUZZLE_TYPES_HPP_INCLUDED

#include <array>
#include <deque>
#include <format>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>


struct Vect2
{
    int x;
    int y;

    bool operator==(Vect2 const&) const = default;
    bool operator!=(Vect2 const&) const = default;

    Vect2& operator+=(Vect2 const& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

inline Vect2
operator+(Vect2 const& lhs, Vect2 const& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

struct PieceTag
{
    char symbol;
    int number;

    auto operator<=>(PieceTag const&) const = default;

    static constexpr PieceTag empty() {
        return {'*', 0};
    }
    static constexpr PieceTag obstacle() {
        return {'#', 0};
    }

    std::string toString() const {
        switch (symbol) {
        case empty().symbol:
            return "**";
        case obstacle().symbol:
            return "##";
        default:
            return std::format("{}{}", symbol, number);
        }
    }
};

struct PieceGeom
{
    Vect2 const* ptr;
    size_t len;

    constexpr PieceGeom()
    : ptr(nullptr), len(0) {}

    template<size_t size>
    constexpr PieceGeom(Vect2 const (&array)[size])
    : ptr(array), len(size) {}

    Vect2 const* begin() const { return ptr; }
    Vect2 const* end() const { return ptr + len; }

    bool empty() const { return len == 0; }
};

struct Piece
{
    PieceTag tag;
    Vect2 position;
    PieceGeom geom;

    std::string name() const {
        return tag.toString();
    }
};

struct Step
{
    Vect2 vector;

    static constexpr Step up()     { return {{0, -1}}; }
    static constexpr Step down()   { return {{0, +1}}; }
    static constexpr Step left()   { return {{-1, 0}}; }
    static constexpr Step right()  { return {{+1, 0}}; }

    static constexpr std::array<Step, 4> all() {
        return {up(), down(), left(), right()};
    }

    std::string toString() const {
        if (vector == Vect2{0, 0})
            return "•";

        auto const xCount = vector.x >= 0 ? vector.x : -vector.x;
        auto const yCount = vector.y >= 0 ? vector.y : -vector.y;
        auto const xSymbol = vector.x >= 0 ? "→" : "←";
        auto const ySymbol = vector.y >= 0 ? "↓" : "↑";

        if (xCount == 0) {
            if (yCount == 1) return ySymbol;
            else             return std::format("[{}{}]", yCount, ySymbol);
        }
        if (yCount == 0) {
            if (xCount == 1) return xSymbol;
            else             return std::format("[{}{}]", xCount, xSymbol);
        }
        return std::format("[{}{}{}{}]", xCount, xSymbol, yCount, ySymbol);
    }
};

struct Move
{
    size_t pieceIndex;
    Step step;
};


template<int SizeX, int SizeY>
struct Grid
{
    static_assert(SizeX > 0);
    static_assert(SizeY > 0);

    static constexpr int sizeX = SizeX;
    static constexpr int sizeY = SizeY;

    std::vector<Piece> pieces;
    PieceGeom obstacles;

    enum KeySymmetry {
        NoSymmetry,
        HorizontalSymmetry,
    };

    struct Cells
    {
        Cells() {
            cells.fill(PieceTag::empty());
        }

        PieceTag& operator[](Vect2 const& position) {
            if (!contains(position))
                throw std::runtime_error("out of bounds");

            return cells[position.y*sizeX + position.x];
        }

        PieceTag const& operator[](Vect2 const& position) const {
            if (!contains(position))
                throw std::runtime_error("out of bounds");

            return cells[position.y*sizeX + position.x];
        }

        std::string key(KeySymmetry symmetry = NoSymmetry) const {
            if (symmetry == NoSymmetry) {
                std::string result(cells.size(), '\0');
                for (size_t i = 0; i < result.size(); ++i)
                    result[i] = cells[i].symbol;
                return result;
            }
            if (symmetry == HorizontalSymmetry) {
                std::string key1(sizeX * sizeY, '\0');
                std::string key2(sizeX * sizeY, '\0');

                for (size_t y = 0; y < sizeY; ++y)
                    for (size_t x = 0; x < sizeX; ++x) {
                        size_t index1 = y * sizeX + x;
                        size_t index2 = y * sizeX + sizeX - 1 - x;

                        key1[index1] = key2[index2] = cells[index1].symbol;
                    }
                return key1 < key2 ? key1 : key2;
            }
            throw std::runtime_error("unsupported key symmetry");
        }

    private:
        std::array<PieceTag, sizeX * sizeY> cells;
    };

    void apply(Move const& move) {
        if (move.pieceIndex >= pieces.size())
            throw std::runtime_error("out of bounds piece index");

        pieces[move.pieceIndex].position += move.step.vector;
    }

    std::optional<Cells> validate() const {
        Cells result;
        for (auto const& obstacle : obstacles) {
            if (!contains(obstacle))
                return std::nullopt;

            if (result[obstacle] != PieceTag::empty())
                return std::nullopt;

            result[obstacle] = PieceTag::obstacle();
        }
        for (auto const& piece : pieces) {
            for (auto const& fill : piece.geom) {
                auto const position = fill + piece.position;

                if (!contains(position))
                    return std::nullopt;

                if (result[position] != PieceTag::empty())
                    return std::nullopt;

                result[position] = piece.tag;
            }
        }
        return result;
    }

private:
    static bool contains(Vect2 const& position) {
        return 0 <= position.x && position.x < sizeX
            && 0 <= position.y && position.y < sizeY;
    }
};

template<int SizeX, int SizeY>
inline std::ostream&
operator<<(std::ostream& out, Grid<SizeX, SizeY> const& grid) {
    auto const validated = grid.validate();
    if (!validated)
        return out << "\n| INVALID";

    auto const& cells = *validated;
    for (int y = 0; y < SizeY; ++y) {
        out << "\n|";
        for (int x = 0; x < SizeX; ++x)
            out << "  " << cells[{x, y}].toString();
    }
    return out;
}


struct IndexRange {
    size_t a;
    size_t b;

    bool isEmpty() const {
        return b <= a;
    }

    struct Iterator {
        size_t value;
        auto operator<=>(Iterator const&) const = default;

        Iterator& operator++() {
            value += 1;
            return *this;
        }
        size_t operator*() const {
            return value;
        }
    };

    Iterator begin() const {
        return {a};
    }
    Iterator end() const {
        return {b};
    }
};


template<typename Node, typename Edge, typename Key>
struct SearchTree
{
    bool append(Node const& node, Edge const& edge, Key const& key) {
        auto const [iter, inserted] = keys.insert(key);
        if (!inserted)
            return false;

        nodes.push_back(node);
        edges.push_back(edge);
        return true;
    }

    void incrementDepth() {
        size_t const a = levels.empty() ? 0 : levels.back().b;
        size_t const b = edges.size();

        levels.push_back({a, b});
        nodes.erase(nodes.begin(), nodes.end() - (b - a));
    }

    IndexRange currentDepth() const {
        return levels.empty() ? IndexRange{0, 0} : levels.back();
    }

    Node const& nodeAt(size_t index) const {
        size_t const offset = levels.empty() ? 0 : levels.back().a;
        if (index < offset)
            throw std::runtime_error("accessing optimized-out element");
        return nodes.at(index - offset);
    }

    Edge const& edgeAt(size_t index) const {
        return edges.at(index);
    }

    size_t lastIndex() const {
        if (edges.empty())
            throw std::runtime_error("no last index in an empty search tree");
        return edges.size() - 1;
    }

    struct Statistics {
        size_t keysCount;
        size_t edgesCount;
        size_t nodesCount;
        size_t levelsCount;
    };

    Statistics getStatistics() const {
        return {
            keys.size(),
            edges.size(),
            nodes.size(),
            levels.size(),
        };
    }

private:
    std::deque<Node> nodes;
    std::deque<Edge> edges;
    std::deque<IndexRange> levels;
    std::unordered_set<Key> keys;
};

#endif  // PUZZLE_TYPES_HPP_INCLUDED
