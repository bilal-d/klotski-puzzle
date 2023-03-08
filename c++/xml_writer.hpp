// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

#ifndef XML_WRITER_HPP_INCLUDED
#define XML_WRITER_HPP_INCLUDED

#include <iostream>
#include <stack>
#include <string>

struct XmlEscaped
{
    std::string_view value;
};

inline std::ostream&
operator<<(std::ostream& out, XmlEscaped const& escaped)
{
    for (char c : escaped.value) {
        switch (c) {
        case '\"': out << "&quot;"; break;
        case '\'': out << "&apos;"; break;
        case '<':  out << "&lt;";   break;
        case '>':  out << "&gt;";   break;
        case '&':  out << "&amp;";  break;
        default:   out << c;        break;
        }
    }
    return out;
}


struct XmlWriter
{
    enum State : unsigned int
    {
        Finished = 0,
        ExpectingDeclaration = 1 << 0,
        ExpectingRoot = 1 << 1,
        ExpectingAttribute = 1 << 2,
        ExpectingContent = 1 << 3,
        ExpectingElement = 1 << 4,
    };

    static constexpr std::string_view StandardXmlDecl =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

    explicit XmlWriter(std::ostream& out)
        : out(out), elements(), state(ExpectingDeclaration | ExpectingRoot) {}

    XmlWriter& decl(std::string_view xmlDecl = StandardXmlDecl) {
        if (!(state & ExpectingDeclaration))
            throw std::runtime_error("unexpected declaration");

        if (!xmlDecl.empty())
            out << xmlDecl << "\n";

        state = ExpectingRoot;
        return *this;
    }

    XmlWriter& root(std::string_view name) {
        if (!(state & ExpectingRoot))
            throw std::runtime_error("unexpected root element");

        if (!validName(name))
            throw std::runtime_error("invalid name for root element");

        out << "<" << name;
        elements.push(ElementStatus{std::string(name), true});
        state = ExpectingAttribute | ExpectingContent | ExpectingElement;
        return *this;
    }

    XmlWriter& attr(std::string_view name, std::string_view value) {
        if (!(state & ExpectingAttribute))
            throw std::runtime_error("unexpected attribute");

        if (!validName(name))
            throw std::runtime_error("invalid name for attribute");

        out << " " << name << "=\"" << XmlEscaped{value} << "\"";
        return *this;
    }

    XmlWriter& text(std::string_view data) {
        if (!(state & ExpectingContent))
            throw std::runtime_error("unexpected content");

        if (state & ExpectingAttribute)
            out << ">";
        out << XmlEscaped{data};

        elements.top().empty = false;
        state = ExpectingContent | ExpectingElement;
        return *this;
    }

    XmlWriter& elem(std::string_view name) {
        if (!(state & ExpectingElement))
            throw std::runtime_error("unexpected element");

        if (!validName(name))
            throw std::runtime_error("invalid name for element");

        if (state & ExpectingAttribute)
            out << ">";
        out << "<" << name;

        elements.top().empty = false;
        elements.push(ElementStatus{std::string(name), true});
        state = ExpectingAttribute | ExpectingContent | ExpectingElement;
        return *this;
    }

    XmlWriter& term() {
        if (elements.empty())
            throw std::runtime_error("no element to close");

        ElementStatus const& element = elements.top();
        if (element.empty)
            out << "/>";
        else
            out << "</" << element.name << ">";

        elements.pop();
        if (elements.empty())
            state = Finished;
        else
            state = ExpectingContent | ExpectingElement;

        return *this;
    }

    bool success() const {
        return state == Finished && out.good();
    }

private:
    static bool validFirstChar(char c) {
        return ('A' <= c && c <= 'Z')
            || ('a' <= c && c <= 'z')
            || c == '_'
            || c == ':';
    }

    static bool validNextChar(char c) {
        return ('A' <= c && c <= 'Z')
            || ('a' <= c && c <= 'z')
            || ('0' <= c && c <= '9')
            || c == '_'
            || c == ':'
            || c == '-'
            || c == '.';
    }

    static bool validName(std::string_view name) {
        if (name.empty())
            return false;
        if (!validFirstChar(name[0]))
            return false;
        for (size_t index = 1; index < name.size(); ++index)
            if (!validNextChar(name[index]))
                return false;
        return true;
    }

    struct ElementStatus
    {
        std::string name;
        bool empty;
    };

    std::ostream& out;
    std::stack<ElementStatus> elements;
    unsigned int state;
};

#endif  // XML_WRITER_HPP_INCLUDED
