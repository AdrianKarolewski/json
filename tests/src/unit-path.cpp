//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++ (supporting code)
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#include "doctest_compatibility.h"

#define JSON_TESTS_PRIVATE

#include <nlohmann/detail/path_parser.hpp>

TEST_CASE("path_lexer")
{
    SECTION("token_type_name")
    {
        using token = path_lexer::token_type;

        CHECK(std::string(path_lexer::token_type_name(token::root)) == "$");
        CHECK(std::string(path_lexer::token_type_name(token::current)) == "@");
        CHECK(std::string(path_lexer::token_type_name(token::dot)) == ".");
        CHECK(std::string(path_lexer::token_type_name(token::all)) == "*");
        CHECK(std::string(path_lexer::token_type_name(token::select_open)) == "[");
        CHECK(std::string(path_lexer::token_type_name(token::select_close)) == "]");
        CHECK(std::string(path_lexer::token_type_name(token::function_open)) == "(");
        CHECK(std::string(path_lexer::token_type_name(token::function_close)) == ")");
        CHECK(std::string(path_lexer::token_type_name(token::comma)) == ",");
        CHECK(std::string(path_lexer::token_type_name(token::question_mark)) == "?");
        CHECK(std::string(path_lexer::token_type_name(token::equal)) == "=");
        CHECK(std::string(path_lexer::token_type_name(token::exclamation_mark)) == "!");
        CHECK(std::string(path_lexer::token_type_name(token::greather_than)) == ">");
        CHECK(std::string(path_lexer::token_type_name(token::less_than)) == "<");
        CHECK(std::string(path_lexer::token_type_name(token::range)) == ":");
        CHECK(std::string(path_lexer::token_type_name(token::arg_part)) == "arg");
    }
    SECTION("tokenize")
    {
        using token = path_lexer::token_type;
        CHECK(path_lexer::tokenize('$') == token::root);
        CHECK(path_lexer::tokenize('@') == token::current);
        CHECK(path_lexer::tokenize('.') == token::dot);
        CHECK(path_lexer::tokenize('*') == token::all);
        CHECK(path_lexer::tokenize('[') == token::select_open);
        CHECK(path_lexer::tokenize(']') == token::select_close);
        CHECK(path_lexer::tokenize('(') == token::function_open);
        CHECK(path_lexer::tokenize(')') == token::function_close);
        CHECK(path_lexer::tokenize(',') == token::comma);
        CHECK(path_lexer::tokenize('?') == token::question_mark);
        CHECK(path_lexer::tokenize('=') == token::equal);
        CHECK(path_lexer::tokenize('!') == token::exclamation_mark);
        CHECK(path_lexer::tokenize('>') == token::greather_than);
        CHECK(path_lexer::tokenize('<') == token::less_than);
        CHECK(path_lexer::tokenize(':') == token::range);

        CHECK(path_lexer::tokenize('a') == token::arg_part);
        CHECK(path_lexer::tokenize('j') == token::arg_part);
        CHECK(path_lexer::tokenize('z') == token::arg_part);

        CHECK(path_lexer::tokenize('A') == token::arg_part);
        CHECK(path_lexer::tokenize('J') == token::arg_part);
        CHECK(path_lexer::tokenize('Z') == token::arg_part);

        CHECK(path_lexer::tokenize('0') == token::arg_part);
        CHECK(path_lexer::tokenize('5') == token::arg_part);
        CHECK(path_lexer::tokenize('9') == token::arg_part);

        CHECK(path_lexer::tokenize('/') == token::arg_part);
        CHECK(path_lexer::tokenize('#') == token::arg_part);
    }
}
TEST_CASE("path_parser")
{
    SECTION("tokenize")
    {
        using token = path_lexer::token_type;
        auto tokens = path_parser::tokenize("$@.*[](),?<>==!=qwerty123");
        CHECK(tokens.size() == 25);

        CHECK(tokens.at(0) == token::root);
        CHECK(tokens.at(1) == token::current);
        CHECK(tokens.at(2) == token::dot);
        CHECK(tokens.at(3) == token::all);
        CHECK(tokens.at(4) == token::select_open);
        CHECK(tokens.at(5) == token::select_close);
        CHECK(tokens.at(6) == token::function_open);
        CHECK(tokens.at(7) == token::function_close);
        CHECK(tokens.at(8) == token::comma);
        CHECK(tokens.at(9) == token::question_mark);
        CHECK(tokens.at(10) == token::less_than);
        CHECK(tokens.at(11) == token::greather_than);
        CHECK(tokens.at(12) == token::equal);
        CHECK(tokens.at(13) == token::equal);
        CHECK(tokens.at(14) == token::exclamation_mark);
        CHECK(tokens.at(15) == token::equal);

        CHECK(tokens.at(16) == token::arg_part);
        CHECK(tokens.at(17) == token::arg_part);
        CHECK(tokens.at(18) == token::arg_part);
        CHECK(tokens.at(19) == token::arg_part);
        CHECK(tokens.at(20) == token::arg_part);
        CHECK(tokens.at(21) == token::arg_part);
        CHECK(tokens.at(22) == token::arg_part);
        CHECK(tokens.at(23) == token::arg_part);
        CHECK(tokens.at(24) == token::arg_part);
    }
    SECTION("brackets check")
    {
        auto tokens = path_parser::tokenize("[]()");
        CHECK(path_parser::checkbrackets(tokens) == true);

        tokens = path_parser::tokenize("[sdf](asgd)");
        CHECK(path_parser::checkbrackets(tokens) == true);

        tokens = path_parser::tokenize("[(dd)]");
        CHECK(path_parser::checkbrackets(tokens) == true);

        tokens = path_parser::tokenize("[](dg[]sg[]sd)");
        CHECK(path_parser::checkbrackets(tokens) == true);

        tokens = path_parser::tokenize("[(])()");
        CHECK(path_parser::checkbrackets(tokens) == false);

        tokens = path_parser::tokenize("([]()");
        CHECK(path_parser::checkbrackets(tokens) == false);

        tokens = path_parser::tokenize("[](])]");
        CHECK(path_parser::checkbrackets(tokens) == false);

        tokens = path_parser::tokenize("[[[)))(((]]]");
        CHECK(path_parser::checkbrackets(tokens) == false);
    }
    SECTION("translate to exec_path")
    {
        using lexer_token = path_lexer::token_type;
        using path_token = path_parser::path_token;

        std::string path = "$.store.book[*].author";
        auto tokens = path_parser::tokenize(path);
        auto exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 10);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<lexer_token>(exec_path.at(1)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("store"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(4)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::select_open);
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::all);
        CHECK(std::get<lexer_token>(exec_path.at(7)) == lexer_token::select_close);
        CHECK(std::get<lexer_token>(exec_path.at(8)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(9)) == std::string("author"));

        path = "$..author";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 3);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("author"));

        path = "$.store.*";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 5);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<lexer_token>(exec_path.at(1)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("store"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::dot);
        CHECK(std::get<lexer_token>(exec_path.at(4)) == lexer_token::all);

        path = "$.store..price";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 5);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<lexer_token>(exec_path.at(1)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("store"));
        CHECK(std::get<path_token>(exec_path.at(3)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(4)) == std::string("price"));

        path = "$..book[2]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 6);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<std::string>(exec_path.at(4)) == std::string("2"));
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::select_close);


        path = "$..book[(@.length-1)]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 11);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<lexer_token>(exec_path.at(4)) == lexer_token::function_open);
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::current);
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::dot);
        CHECK(std::get<path_token>(exec_path.at(7)) == path_token::length);
        CHECK(std::get<std::string>(exec_path.at(8)) == std::string("-1"));
        CHECK(std::get<lexer_token>(exec_path.at(9)) == lexer_token::function_close);
        CHECK(std::get<lexer_token>(exec_path.at(10)) == lexer_token::select_close);

        path = "$..book[-1:]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 7);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<std::string>(exec_path.at(4)) == std::string("-1"));
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::range);
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::select_close);

        path = "$..book[0,1]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 8);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<std::string>(exec_path.at(4)) == std::string("0"));
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::comma);
        CHECK(std::get<std::string>(exec_path.at(6)) == std::string("1"));
        CHECK(std::get<lexer_token>(exec_path.at(7)) == lexer_token::select_close);
        
        path = "$..book[:2]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 7);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<lexer_token>(exec_path.at(4)) == lexer_token::range);
        CHECK(std::get<std::string>(exec_path.at(5)) == std::string("2"));
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::select_close);

        path = "$..book[?(@.isbn)]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 11);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<lexer_token>(exec_path.at(4)) == lexer_token::question_mark);
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::function_open);
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::current);
        CHECK(std::get<lexer_token>(exec_path.at(7)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(8)) == std::string("isbn"));
        CHECK(std::get<lexer_token>(exec_path.at(9)) == lexer_token::function_close);
        CHECK(std::get<lexer_token>(exec_path.at(10)) == lexer_token::select_close);

        path = "$..book[?(@.price<10)]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 13);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<lexer_token>(exec_path.at(4)) == lexer_token::question_mark);
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::function_open);
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::current);
        CHECK(std::get<lexer_token>(exec_path.at(7)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(8)) == std::string("price"));
        CHECK(std::get<lexer_token>(exec_path.at(9)) == lexer_token::less_than);
        CHECK(std::get<std::string>(exec_path.at(10)) == std::string("10"));
        CHECK(std::get<lexer_token>(exec_path.at(11)) == lexer_token::function_close);
        CHECK(std::get<lexer_token>(exec_path.at(12)) == lexer_token::select_close);

        path = "$..book[?(@.price!=10)]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 13);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<lexer_token>(exec_path.at(4)) == lexer_token::question_mark);
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::function_open);
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::current);
        CHECK(std::get<lexer_token>(exec_path.at(7)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(8)) == std::string("price"));
        CHECK(std::get<path_token>(exec_path.at(9)) == path_token::not_equal_to);
        CHECK(std::get<std::string>(exec_path.at(10)) == std::string("10"));
        CHECK(std::get<lexer_token>(exec_path.at(11)) == lexer_token::function_close);
        CHECK(std::get<lexer_token>(exec_path.at(12)) == lexer_token::select_close);

        path = "$..book[?(@.price==10)]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 13);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<lexer_token>(exec_path.at(4)) == lexer_token::question_mark);
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::function_open);
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::current);
        CHECK(std::get<lexer_token>(exec_path.at(7)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(8)) == std::string("price"));
        CHECK(std::get<path_token>(exec_path.at(9)) == path_token::equal_to);
        CHECK(std::get<std::string>(exec_path.at(10)) == std::string("10"));
        CHECK(std::get<lexer_token>(exec_path.at(11)) == lexer_token::function_close);
        CHECK(std::get<lexer_token>(exec_path.at(12)) == lexer_token::select_close);

        path = "$..book[?(@.price>10)]";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 13);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<std::string>(exec_path.at(2)) == std::string("book"));
        CHECK(std::get<lexer_token>(exec_path.at(3)) == lexer_token::select_open);
        CHECK(std::get<lexer_token>(exec_path.at(4)) == lexer_token::question_mark);
        CHECK(std::get<lexer_token>(exec_path.at(5)) == lexer_token::function_open);
        CHECK(std::get<lexer_token>(exec_path.at(6)) == lexer_token::current);
        CHECK(std::get<lexer_token>(exec_path.at(7)) == lexer_token::dot);
        CHECK(std::get<std::string>(exec_path.at(8)) == std::string("price"));
        CHECK(std::get<lexer_token>(exec_path.at(9)) == lexer_token::greather_than);
        CHECK(std::get<std::string>(exec_path.at(10)) == std::string("10"));
        CHECK(std::get<lexer_token>(exec_path.at(11)) == lexer_token::function_close);
        CHECK(std::get<lexer_token>(exec_path.at(12)) == lexer_token::select_close);

        path = "$..*";
        tokens = path_parser::tokenize(path);
        exec_path = path_parser::translate(tokens, path);

        CHECK(exec_path.size() == 3);
        CHECK(std::get<lexer_token>(exec_path.at(0)) == lexer_token::root);
        CHECK(std::get<path_token>(exec_path.at(1)) == path_token::all_from);
        CHECK(std::get<lexer_token>(exec_path.at(2)) == lexer_token::all);
    }
}