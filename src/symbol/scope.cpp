/*******************************************************************************
 *                                                                             *
 *  Filename    : scope.cpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Scope class methods                                          *
 *                                                                             *
 *******************************************************************************/

#include <symbol/scope.hpp>

using namespace sem;

/*******************************************************************************
 ************************************ Scope ************************************
 *******************************************************************************/

Scope::Scope(unsigned int nestingLevel, EntryPtr fun) {
    this->nestingLevel = nestingLevel;
    this->currentOffset = 0;
    this->fun = fun;
}

EntryPtr Scope::getFunction() {
    return fun;
}
