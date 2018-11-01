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
#include <message/message.hpp>
#include <general/general.hpp>

namespace sem {

/*******************************************************************************
 *********************************** General ***********************************
 *******************************************************************************/

TypePtr Type::getRef() {
    return nullptr;
}

std::ostream& operator<<(std::ostream &out, const Type &t) {
    return t.print(out);
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

std::ostream& TypeVoid::print(std::ostream &out) const {
    return ( out << "proc" );
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

std::ostream& TypeInt::print(std::ostream &out) const {
    return ( out << "int" );
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

std::ostream& TypeByte::print(std::ostream &out) const {
    return ( out << "byte" );
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

std::ostream& TypeArray::print(std::ostream &out) const {
    return ( out << "array of " << *this->refType << "[" << this->size << "]" );
}

/*******************************************************************************
 *********************************** IARRAY ************************************
 *******************************************************************************/

TypeIArray::TypeIArray(TypePtr type) {
    if (type->t == genType::VOID)
        error("Array cannot be of type void\n");
    this->t = genType::IARRAY;
    this->refType = type;
}

int TypeIArray::getSize() {
    return INT_SIZE;
}

TypePtr TypeIArray::getRef() {
    return this->refType;
}

std::ostream& TypeIArray::print(std::ostream &out) const {
    return ( out << "iarray of " << *this->refType );
}

/*******************************************************************************
 ***************************** Auxiliary Functions *****************************
 *******************************************************************************/

bool equalType(TypePtr a, TypePtr b) {
    if ( a == b )
        return true;

    if ( a->t == b->t ) {
        switch( a->t ) {
            case genType::ARRAY :
                if ( a->getSize() != b->getSize() )
                    return false;
            case genType::IARRAY :
                return equalType(a->getRef(), b->getRef());
        }
    }
    return false;
}

bool compatibleType(TypePtr a, TypePtr b) {
    if ( a == b )
        return true;

    if ( a->t == b->t )
        return true;

    if ( (a->t == genType::ARRAY && b->t == genType::IARRAY) 
            || (a->t == genType::IARRAY && b->t == genType::ARRAY)
       ) {
        return compatibleType(a->getRef(), b->getRef());
    }

    return false;
}

/*******************************************************************************
 ******************************* Constant Types ********************************
 *******************************************************************************/

TypePtr typeInteger = newShared<TypeInt>();
TypePtr typeByte    = newShared<TypeByte>();
TypePtr typeVoid    = newShared<TypeVoid>();

} // end namespace sem
