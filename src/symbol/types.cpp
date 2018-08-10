/*******************************************************************************
 *                                                                             *
 *  Filename    : types.cpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Types source file for semantic analysis                      *
 *                                                                             *
 *******************************************************************************/

#include <iostream>

#include <symbol/types.hpp>
#include <error/error.hpp>

/*******************************************************************************
 ************************************ VOID *************************************
 *******************************************************************************/

TypeVoid::TypeVoid() {
    this->t = genType::VOID;
}

int TypeVoid::getSize() {
    error("Type void has no size");
    return -1;
}

void TypeVoid::print() {
    std::cout << "Void";
}

/*******************************************************************************
 ************************************* INT *************************************
 *******************************************************************************/

TypeInt::TypeInt() {
    this->t = genType::INT;
}

int TypeInt::getSize() {
    return INT_SIZE;
}

void TypeInt::print() {
    std::cout << "Int";
}

/*******************************************************************************
 ************************************ BYTE *************************************
 *******************************************************************************/

TypeByte::TypeByte() {
    this->t = genType::BYTE;
}

int TypeByte::getSize() {
    return BYTE_SIZE;
}

void TypeByte::print() {
    std::cout << "Byte";
}

/*******************************************************************************
 ************************************ ARRAY ************************************
 *******************************************************************************/

TypeArray::TypeArray(int size, std::unique_ptr<Type> type) {
    if (type->t == genType::VOID)
        error("Array cannot be of type void");
    this->t = genType::ARRAY;
    this->size = size;
    this->refType = std::move(type);
}

int TypeArray::getSize() {
    return ( this->size * refType->getSize() );
}

void TypeArray::print() {
    std::cout << "Array of ";
    refType->print();
    std::cout << "[" << size << "]";
}

/*******************************************************************************
 *********************************** IARRAY ************************************
 *******************************************************************************/

TypeIArray::TypeIArray(std::unique_ptr<Type> type) {
    if (type->t == genType::VOID)
        error("Array cannot be of type void");
    this->t = genType::IARRAY;
    this->refType = std::move(type);
}

int TypeIArray::getSize() {
    return INT_SIZE;
}

void TypeIArray::print() {
    std::cout << "Array of ";
    refType->print();
    std::cout << "[]";
}
