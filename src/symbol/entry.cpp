/*******************************************************************************
 *                                                                             *
 *  Filename    : entry.cpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Entries classes method implementations                       *
 *                                                                             *
 *******************************************************************************/

#include <iostream>
#include <memory>
#include <vector>

#include <error/error.hpp>
#include <symbol/types.hpp>
#include <symbol/entry.hpp>

namespace sem {

/*******************************************************************************
 ************************** General Class For Errors ***************************
 *******************************************************************************/

int Entry::getOffset() {
    error("Not a variable/parameter");
    return -1;
}

void Entry::setOffset(int offset) {
    error("Not a variable/parameter");
}

int Entry::getReturns() {
    error("Not a function");
    return -1;
}

const EntryVector& Entry::getParams() const {
    error("Not a function");
}

const EntryVector& Entry::getHidden() const {
    error("Not a function");
}

PassMode Entry::getMode() {
    error("Not a parameter");
    return PassMode::VALUE;
}

void Entry::addParam(EntryPtr param) {
    error("Not a function");
    return;
}

void Entry::addHidden(EntryPtr entry) {
    error("Not a function");
    return;
}

void Entry::addReturn() {
    error("Not a function");
    return;
}

/*******************************************************************************
 ******************************* Variable Class ********************************
 *******************************************************************************/

EntryVariable::EntryVariable(std::string id, TypePtr type) {
    this->id = std::move(id);
    this->type = type;
    this->eType = EntryType::VARIABLE;
}

int EntryVariable::getOffset() {
    return this->offset;
}

void EntryVariable::setOffset(int offset) {
    this->offset = offset;
}

void EntryVariable::print(std::string prefix) {
    std::cout << prefix << " Variable : " << this->id << std::endl;
}

/*******************************************************************************
 ******************************* Function Class ********************************
 *******************************************************************************/

EntryFunction::EntryFunction(std::string id, TypePtr type) {
    this->id = std::move(id);
    this->type = type;
    this->returns = 0;
    this->eType = EntryType::FUNCTION;
}

int EntryFunction::getReturns() {
    return this->returns;
}

const EntryVector& EntryFunction::getParams() const {
    return this->params;
}

const EntryVector& EntryFunction::getHidden() const {
    return this->hidden;
}

void EntryFunction::addParam(EntryPtr param) {
    this->params.push_back(param);
}

void EntryFunction::addHidden(EntryPtr entry) {
    this->hidden.push_back(entry);
}

void EntryFunction::addReturn() {
    this->returns++;
}

void EntryFunction::print(std::string prefix) {
    std::cout << prefix << " Function : " << this->id << std::endl;
    std::string mid(prefix.size(), ' ');
    mid += "`-";
    std::cout << mid << " with parameters :" << std::endl;
    std::string bot(mid.size(), ' ');
    bot += "`-";
    for ( auto p : params )
        p->print(bot);
    std::cout << mid << " with hiddens :" << std::endl;
    for ( auto h : hidden )
        h->print(bot);
}

/*******************************************************************************
 ******************************* Parameter Class *******************************
 *******************************************************************************/

EntryParameter::EntryParameter(std::string id, TypePtr type, PassMode mode) {
    if ( type->t == genType::IARRAY 
            && mode != PassMode::REFERENCE ) {
        error("Arrays must always be passed by reference");
    }
    this->id = std::move(id);
    this->type = type;
    this->mode = mode;
    this->eType = EntryType::PARAMETER;
}

int EntryParameter::getOffset() {
    return this->offset;
}

void EntryParameter::setOffset(int offset) {
    this->offset = offset;
}

PassMode EntryParameter::getMode() {
    return this->mode;
}

void EntryParameter::print(std::string prefix) {
    std::cout << prefix << " Parameter : " << this->id << std::endl;
}

} // end namespace sem
