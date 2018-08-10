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

EntryVector Entry::getParams() {
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

void Entry::addReturn() {
    error("Not a function");
    return;
}

/*******************************************************************************
 ******************************* Variable Class ********************************
 *******************************************************************************/

EntryVariable::EntryVariable(std::string id, TypePtr type) {
    this->id = std::move(id);
    this->type = std::move(type);
    this->eType = EntryType::VARIABLE;
}

int EntryVariable::getOffset() {
    return this->offset;
}

void EntryVariable::setOffset(int offset) {
    this->offset = offset;
}

void EntryVariable::print() {
    std::cout << "Variable : " << this->id << std::endl;
}

/*******************************************************************************
 ******************************* Function Class ********************************
 *******************************************************************************/

EntryFunction::EntryFunction(std::string id, TypePtr type) {
    this->id = std::move(id);
    this->type = std::move(type);
    this->returns = 0;
    this->eType = EntryType::FUNCTION;
}

int EntryFunction::getReturns() {
    return this->returns;
}

EntryVector EntryFunction::getParams() {
    return this->params;
}

void EntryFunction::addParam(EntryPtr param) {
    this->params.push_back(param);
}

void EntryFunction::addReturn() {
    this->returns++;
}

void EntryFunction::print() {
    std::cout << "Function : " << this->id << std::endl;
    std::cout << "-- with params :" << std::endl;
    for ( auto p : params ) {
        std::cout << "---- ";
        p->print();
    }
}

/*******************************************************************************
 ******************************* Parameter Class *******************************
 *******************************************************************************/

EntryParameter::EntryParameter(std::string id, TypePtr type, PassMode mode) {
    this->id = std::move(id);
    this->type = std::move(type);
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

void EntryParameter::print() {
    std::cout << "Parameter : " << this->id << std::endl;
}
