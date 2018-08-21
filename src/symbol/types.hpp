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

#include <memory>

namespace sem {

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
        virtual TypePtr getRef();

        virtual std::ostream& print(std::ostream &out) const = 0;

        friend std::ostream& operator<<(std::ostream &out, const Type &t);
};

/*******************************************************************************
 ********************************** VOID TYPE **********************************
 *******************************************************************************/

class TypeVoid : public Type {
    public :
        TypeVoid();
        virtual ~TypeVoid() {  }
        virtual int getSize() override;
        virtual std::ostream& print(std::ostream &out) const override;
};

/*******************************************************************************
 ********************************** INT TYPE ***********************************
 *******************************************************************************/

class TypeInt : public Type {
    public :
        TypeInt();
        virtual ~TypeInt() {  }
        virtual int getSize() override;
        virtual std::ostream& print(std::ostream &out) const override;
};

/*******************************************************************************
 ********************************** BYTE TYPE **********************************
 *******************************************************************************/

class TypeByte : public Type {
    public :
        TypeByte();
        virtual ~TypeByte() {  }
        virtual int getSize() override;
        virtual std::ostream& print(std::ostream &out) const override;
};

/*******************************************************************************
 ********************************* ARRAY TYPE **********************************
 *******************************************************************************/

class TypeArray : public Type {
    private :
        int size;
        TypePtr refType;
    public :
        TypeArray(int size, TypePtr type);
        virtual ~TypeArray() {  }
        virtual int getSize() override;
        virtual TypePtr getRef() override;
        virtual std::ostream& print(std::ostream &out) const override;
};

/*******************************************************************************
 ********************************* IARRAY TYPE *********************************
 *******************************************************************************/

class TypeIArray : public Type {
    private :
        TypePtr refType;
    public :
        TypeIArray(TypePtr type);
        virtual ~TypeIArray() {  }
        virtual int getSize() override;
        virtual TypePtr getRef() override;
        virtual std::ostream& print(std::ostream &out) const override;
};

/*******************************************************************************
 ***************************** Auxiliary Functions *****************************
 *******************************************************************************/

bool equalType(TypePtr a, TypePtr b);
bool compatibleType(TypePtr a, TypePtr b);

/*******************************************************************************
 ******************************* Constant Types ********************************
 *******************************************************************************/

extern TypePtr typeInteger;
extern TypePtr typeByte;
extern TypePtr typeVoid;

} // end namespace sem

#endif
