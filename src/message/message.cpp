/*******************************************************************************
 *                                                                             *
 *  Filename    : message.cpp                                                  *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Messager class source file                                   *
 *                                                                             *
 *******************************************************************************/

#include <message/message.hpp>

#include <iostream>
#include <deque>


void Debugger::newLevel() {
    this->prefixes.push_front(this->prefix);
    this->prefix.append("| ");
}

void Debugger::restoreLevel() {
    this->prefix = this->prefixes.front();
    this->prefixes.pop_front();
}
