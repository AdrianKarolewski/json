//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <stack>

#include <cassert>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include <nlohmann/detail/path_lexer.hpp>

class path_parser
{
public:
    enum path_token
    {
        all_from,
        equal_to,
        not_equal_to,
        length
    };

    using lexer_token = path_lexer::token_type;
    using exec_token = std::variant<lexer_token, path_token, std::string>;

    static void parse(const std::string& _path)
    {
        auto tokens = tokenize(_path);
        if (!checkbrackets(tokens))
            return; // THROW TODO
        auto exec_path = translate(tokens, _path);
        parse(exec_path);
    }

    static void parse(const std::vector<exec_token>& /* exec_tokens*/)
    {

    }
    
    static std::vector<exec_token> translate(const std::vector<lexer_token>& _lex_tokens, const std::string& _path)
    {
        std::vector<exec_token> result;
        result.reserve(_lex_tokens.size() / 2);

        int pos = 0;
        std::string arg = "";

        for (const lexer_token& token : _lex_tokens)
        {
            switch (token)
            {
                case lexer_token::root:
                case lexer_token::current:
                case lexer_token::all:
                case lexer_token::comma:
                case lexer_token::question_mark:
                case lexer_token::select_open:
                case lexer_token::select_close:
                case lexer_token::function_open:
                case lexer_token::function_close:
                case lexer_token::greather_than:
                case lexer_token::less_than:
                case lexer_token::range:
                    result.push_back(token);
                    break;
                case lexer_token::dot:
                {
                    if (pos < _lex_tokens.size() - 1 && _lex_tokens.at(pos + 1) == token)
                        result.push_back(path_token::all_from);
                    else if (pos > 0 && _lex_tokens.at(pos - 1) == token)
                        break;
                    else
                        result.push_back(token);
                    break;
                }
                case lexer_token::equal:
                {
                    if (pos < _lex_tokens.size() - 1 && _lex_tokens.at(pos + 1) == token)
                        result.push_back(path_token::equal_to);
                    else if (pos > 0 && _lex_tokens.at(pos - 1) == token)
                        break;
                    break;
                }
                case lexer_token::exclamation_mark:
                {
                    if (pos < _lex_tokens.size() - 1 && _lex_tokens.at(pos + 1) == lexer_token::equal)
                        result.push_back(path_token::not_equal_to);
                    else if (pos > 0 && _lex_tokens.at(pos - 1) == token)
                        break;
                    break;
                }
                case lexer_token::arg_part:
                {
                    if (pos < _lex_tokens.size() - 1 && _lex_tokens.at(pos + 1) == token)
                    {
                        if (arg == "length")
                        {
                            result.push_back(path_token::length);
                            arg.clear();
                        }
                        arg += _path.at(pos);
                    }

                    else
                    {
                        arg += _path.at(pos);
                        if (arg == "length")
                            result.push_back(path_token::length);
                        else
                            result.push_back(arg);
                        arg.clear();
                    }
                    break;
                }
            }
            pos += 1;
        }
        return result;
    }

    static bool checkbrackets(const std::vector<lexer_token>& _tokens)
    {
        std::stack<lexer_token> bracketsStack;
        for (const lexer_token& token : _tokens)
        {
            if (token == lexer_token::function_open ||
                token == lexer_token::select_open)
            {
                bracketsStack.push(token);
            }
            else if (token == lexer_token::function_close)
            {
                if (bracketsStack.empty())
                    return false;
                if (bracketsStack.top() != lexer_token::function_open)
                    return false;
                bracketsStack.pop();
            }
            else if (token == lexer_token::select_close)
            {
                if (bracketsStack.empty())
                    return false;
                if (bracketsStack.top() != lexer_token::select_open)
                    return false;
                bracketsStack.pop();
            }
        }
        return bracketsStack.empty();
    }
    static std::vector<lexer_token> tokenize(const std::string& _path)
    {
        std::vector<lexer_token> result;
        result.reserve(_path.size());

        for (const char& c : _path)
        {
            result.push_back(path_lexer::tokenize(c));
        }
        return result;
    }
};