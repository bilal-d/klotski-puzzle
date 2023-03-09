// SPDX-License-Identifier: MIT
// Copyright © 2023  Bilal Djelassi

package svg_renderer

import (
	"io"
	"fmt"
	"log"
	"bufio"
	"strings"
)

type xmlWriterState uint
const (
	finished xmlWriterState = 0
	expectingDeclaration = 1 << iota
	expectingRoot
	expectingAttribute
	expectingContent
	expectingElement
)

const (
	xmlStandardDecl = "<?xml version='1.0' encoding='UTF-8'?>\n"
)

type xmlElementStatus struct {
	name string
	empty bool
}

type XmlWriter struct {
	out *bufio.Writer
	state xmlWriterState
	nesting []xmlElementStatus
}

func NewXmlWriter(out io.Writer) *XmlWriter {
	return &XmlWriter{
		out: bufio.NewWriter(out),
		state: expectingDeclaration | expectingRoot,
		nesting: nil,
	}
}

func (writer *XmlWriter) Decl() *XmlWriter {
	if 0 == (writer.state & expectingDeclaration) {
		log.Fatalln("unexpected declaration")
	}
	_, err := writer.out.WriteString(xmlStandardDecl)
	if err != nil {
		log.Fatalf("error while writing declaration, %v\n", err)
	}
	writer.state = expectingRoot
	return writer
}

func (writer *XmlWriter) Root(name string) *XmlWriter {
	if 0 == (writer.state & expectingRoot) {
		log.Fatalln("unexpected root element")
	}
	if !isValidName(name) {
		log.Fatalln("invalid name for root element")
	}
	
	_, err := fmt.Fprintf(writer.out, "<%s", name)
	if err != nil {
		log.Fatalf("error while writing root element, %v\n", err)
	}
	writer.nesting = append(writer.nesting, xmlElementStatus{name, true})
	writer.state = expectingAttribute | expectingContent | expectingElement
	return writer
}

func (writer *XmlWriter) Attr(name string, value any) *XmlWriter {
	if 0 == (writer.state & expectingAttribute) {
		log.Fatalln("unexpected attribute")
	}
	if !isValidName(name) {
		log.Fatalln("invalid name for attribute")
	}
	_, err := fmt.Fprintf(writer.out, ` %s="%s"`, name,
		xmlEscaped(formatAttrValue(value)))
	if err != nil {
		log.Fatalf("error while writing attribute, %v\n", err)
	}
	return writer
}

func (writer *XmlWriter) Text(data string) *XmlWriter {
	if 0 == (writer.state & expectingContent) {
		log.Fatalln("unexpected content")
	}

	if 0 != (writer.state & expectingAttribute) {
		_, err := writer.out.WriteString(">")
		if err != nil {
			log.Fatalf("error while writing text content, %v\n", err)
		}
	}
	_, err := writer.out.WriteString(xmlEscaped(data))
	if err != nil {
		log.Fatalf("error while writing text content, %v\n", err)
	}

	writer.nesting[len(writer.nesting) - 1].empty = false
	writer.state = expectingContent | expectingElement
	return writer
}

func (writer *XmlWriter) Elem(name string) *XmlWriter {
	if 0 == (writer.state & expectingElement) {
		log.Fatalln("unexpected element")
	}
	if !isValidName(name) {
		log.Fatalln("invalid name for element")
	}
	if 0 != (writer.state & expectingAttribute) {
		_, err := writer.out.WriteString(">")
		if err != nil {
			log.Fatalf("error while writing element, %v\n", err)
		}
	}
	_, err := fmt.Fprintf(writer.out, "<%s", name)
	if err != nil {
		log.Fatalf("error while writing element, %v\n", err)
	}

	writer.nesting[len(writer.nesting) - 1].empty = false
	writer.nesting = append(writer.nesting, xmlElementStatus{name, true})
	writer.state = expectingAttribute | expectingContent | expectingElement
	return writer
}

func (writer *XmlWriter) Term() *XmlWriter {
	remaining := len(writer.nesting)
	if remaining == 0 {
		log.Fatalln("no element to close")
	}

	element := &writer.nesting[remaining - 1]
	if element.empty {
		_, err := writer.out.WriteString("/>")
		if err != nil {
			log.Fatalf("error while closing element, %v\n", err)
		}
	} else {
		_, err := fmt.Fprintf(writer.out, "</%s>", element.name)
		if err != nil {
			log.Fatalf("error while closing element, %v\n", err)
		}
	}

	writer.nesting = writer.nesting[:remaining - 1]
	if remaining == 1 {
		err := writer.out.Flush()
		if err != nil {
			log.Fatalf("error while closing element, %v\n", err)
		}
		writer.state = finished
	} else {
		writer.state = expectingContent | expectingElement
	}
	return writer
}

func formatAttrValue(value any) string {
	switch value := value.(type) {
	case int:
		return fmt.Sprintf("%d", value)
	case float64:
		return fmt.Sprintf("%g", value)
	case string:
		return value
	}
	panic("unsupported value type for attribute")
}

func isValidFirstChar(c rune) bool {
	return ('A' <= c && c <= 'Z') ||
		('a' <= c && c <= 'z') ||
		c == '_' ||
		c == ':'
}

func isValidNextChar(c rune) bool {
	return ('A' <= c && c <= 'Z') ||
		('a' <= c && c <= 'z') ||
		('0' <= c && c <= '9') ||
		c == '_' ||
		c == ':' ||
		c == '-' ||
		c == '.'
}

func isValidName(name string) bool {
	length := len(name)
	if length == 0 {
		return false
	}
	for index, character := range(name) {
		if index == 0 {
			if !isValidFirstChar(character) {
				return false
			}
		} else {
			if !isValidNextChar(character) {
				return false
			}
		}
	}
	return true
}

func xmlEscaped(attr string) string {
	escaped := strings.Builder{}
	for _, character := range(attr) {
		switch character {
		case '"':  escaped.WriteString("&quot;")
		case '\'': escaped.WriteString("&apos;")
		case '<':  escaped.WriteString("&lt;")
		case '>':  escaped.WriteString("&gt;")
		case '&':  escaped.WriteString("&amp;")
		default:   escaped.WriteRune(character)
		}
	}
	return escaped.String()
}
