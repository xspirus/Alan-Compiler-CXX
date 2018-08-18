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

using namespace sem;

/*******************************************************************************
 *********************************** General ***********************************
 *******************************************************************************/

TypePtr Type::getRef() {
    return nullptr;
}

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

TypeArray::TypeArray(int size, TypePtr type) {
    if (type->t == genType::VOID)
        error("Array cannot be of type void");
    this->t = genType::ARRAY;
    this->size = size;
    this->refType = type;
}

int TypeArray::getSize() {
    return ( this->size * refType->getSize() );
}

TypePtr TypeArray::getRef() {
    return this->refType;
}

void TypeArray::print() {
    std::cout << "Array of ";
    refType->print();
    std::cout << "[" << size << "]";
}

/*******************************************************************************
 *********************************** IARRAY ************************************
 *******************************************************************************/

TypeIArray::TypeIArray(TypePtr type) {
    if (type->t == genType::VOID)
        error("Array cannot be of type void");
    this->t = genType::IARRAY;
    this->refType = type;
}

int TypeIArray::getSize() {
    return INT_SIZE;
}

TypePtr TypeIArray::getRef() {
    return this->refType;
}

void TypeIArray::print() {
    std::cout << "Array of ";
    refType->print();
    std::cout << "[]";
}

/*******************************************************************************
 ***************************** Auxiliary Functions *****************************
 *******************************************************************************/

bool sem::equalType(TypePtr a, TypePtr b) {
    if ( a == b )
        return true;

    if ( a->t == b->t ) {
        switch( a->t ) {
            case genType::ARRAY :
                if ( a->getSize() != b->getSize() )
                    return false;
            case genType::IARRAY :
                return sem::equalType(a->getRef(), b->getRef());
        }
    }
    return false;
}

/*******************************************************************************
 ******************************* Constant Types ********************************
 *******************************************************************************/

TypePtr sem::typeInteger = std::make_shared<TypeInt>();
TypePtr sem::typeByte    = std::make_shared<TypeByte>();
TypePtr sem::typeVoid    = std::make_shared<TypeVoid>();
