/*******************************************************************************
 *                                                                             *
 *  Filename    : types.hpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Types header file for semantic analysis                      *
 *                                                                             *
 *******************************************************************************/

#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <cstdlib>
#include <memory>

#define INT_SIZE 4
#define BYTE_SIZE 1

/*******************************************************************************
 ********************************** Typedefs ***********************************
 *******************************************************************************/

class Type;

typedef std::shared_ptr<Type> TypePtr;

/*******************************************************************************
 *************************** TYPES ENUMERATION CLASS ***************************
 *******************************************************************************/

enum class genType {
    VOID,
    INT,
    BYTE,
    ARRAY,
    IARRAY
};

/*******************************************************************************
 ****************************** TYPE PARENT CLASS ******************************
 *******************************************************************************/

class Type {
    public :
        // Variables
        genType t;

        // Methods
        virtual ~Type() {  }

        // To be inherited
        virtual int getSize() = 0;

        virtual void print() = 0;
};

/*******************************************************************************
 ********************************** VOID TYPE **********************************
 *******************************************************************************/

class TypeVoid : public Type {
    public :
        TypeVoid();
        virtual ~TypeVoid() {  }
        virtual int getSize() override;
        virtual void print() override;
};

/*******************************************************************************
 ********************************** INT TYPE ***********************************
 *******************************************************************************/

class TypeInt : public Type {
    public :
        TypeInt();
        virtual ~TypeInt() {  }
        virtual int getSize() override;
        virtual void print() override;
};

/*******************************************************************************
 ********************************** BYTE TYPE **********************************
 *******************************************************************************/

class TypeByte : public Type {
    public :
        TypeByte();
        virtual ~TypeByte() {  }
        virtual int getSize() override;
        virtual void print() override;
};

/*******************************************************************************
 ********************************* ARRAY TYPE **********************************
 *******************************************************************************/

class TypeArray : public Type {
    private :
        int size;
        std::unique_ptr<Type> refType;
    public :
        TypeArray(int size, std::unique_ptr<Type> type);
        virtual ~TypeArray() {  }
        virtual int getSize() override;
        virtual void print() override;
};

/*******************************************************************************
 ********************************* IARRAY TYPE *********************************
 *******************************************************************************/

class TypeIArray : public Type {
    private :
        std::unique_ptr<Type> refType;
    public :
        TypeIArray(std::unique_ptr<Type> type);
        virtual ~TypeIArray() {  }
        virtual int getSize() override;
        virtual void print() override;
};

#endif
