//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint> // uint8_t


class path_lexer
{
public:
    /// token types for the parser
    enum class token_type : std::uint8_t
    {
        root,
        current,
        dot,
        all,
        select_open,
        select_close,
        function_open,
        function_close,
        comma,
        range,
        equal,
        exclamation_mark,
        question_mark,
        greather_than,
        less_than,
        arg_part
    };
    /// return name of values of type token_type (only used for errors)
    static const char* token_type_name(const token_type& t) noexcept
    {
        switch (t)
        {
            case token_type::root:
                return "$";
            case token_type::current:
                return "@";
            case token_type::dot:
                return ".";
            case token_type::all:
                return "*";
            case token_type::select_open:
                return "[";
            case token_type::select_close:
                return "]";
            case token_type::function_open:
                return "(";
            case token_type::function_close:
                return ")";
            case token_type::comma:
                return ",";
            case token_type::question_mark:
                return "?";
            case token_type::range:
                return ":";
            case token_type::equal:
                return "=";
            case token_type::exclamation_mark:
                return "!";
            case token_type::greather_than:
                return ">";
            case token_type::less_than:
                return "<";
            case token_type::arg_part:
                return "arg";
            default:
                return "unknown";
        }
    }
    static const token_type tokenize(const char& c) noexcept
    {
        switch (c)
        {
            case '$':
                return token_type::root;
            case '@':
                return token_type::current;
            case '.':
                return token_type::dot;
            case '*':
                return token_type::all;
            case '[':
                return token_type::select_open;
            case ']':
                return token_type::select_close;
            case '(':
                return token_type::function_open;
            case ')':
                return token_type::function_close;
            case ',':
                return token_type::comma;
            case '?':
                return token_type::question_mark;
            case ':':
                return token_type::range;
            case '=':
                return token_type::equal;
            case '!':
                return token_type::exclamation_mark;
            case '>':
                return token_type::greather_than;
            case '<':
                return token_type::less_than;
            default:
                return token_type::arg_part;
        }
    }
};