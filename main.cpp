/*******************************************************************************
 *                                                                             *
 *  Filename    : main.cpp                                                     *
 *  Project     : Test Project                                                 *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : ...                                                          *
 *                                                                             *
 *******************************************************************************/

#include <iostream>
#include <memory>
#include <symbol/types.hpp>
#include <symbol/entry.hpp>
#include <symbol/scope.hpp>
#include <symbol/table.hpp>
#include <ast/ast.hpp>
#include <parser/parser.hpp>

using namespace std;

const char * filename = "TEST";

extern ast::astPtr parse();

int main() {
    auto root = parse();
    ast::semantic(root);
    ast::codegen(root);
    return 0;
}
