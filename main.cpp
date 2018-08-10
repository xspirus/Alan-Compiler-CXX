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

using namespace std;

int linecount = 0;
char * filename = "test";

int main() {
    auto t = make_unique<Table>();
    t->openScope(make_shared<EntryFunction>("main", make_shared<TypeVoid>()));
    auto f = make_shared<EntryFunction>("test", make_shared<TypeInt>());
    t->insertEntry(f);
    t->openScope(f);
    auto p = make_shared<EntryParameter>("i", make_shared<TypeInt>(), PassMode::VALUE);
    t->insertEntry(p);
    t->getScope()->getFunction()->addParam(p);
    t->getScope()->getFunction()->print();
}
