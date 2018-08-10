/*******************************************************************************
 *                                                                             *
 *  Filename    : entry.hpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Entries classes for symbol table                             *
 *                                                                             *
 *******************************************************************************/

#ifndef __ENTRY_HPP__
#define __ENTRY_HPP__

#include <memory>
#include <vector>

#include <symbol/types.hpp>

/*******************************************************************************
 ********************************** Typedefs ***********************************
 *******************************************************************************/

class Entry;

typedef std::shared_ptr<Entry> EntryPtr;
typedef std::vector<EntryPtr>  EntryVector;

/*******************************************************************************
 ******************************** Enumerations *********************************
 *******************************************************************************/

enum class EntryType {
    VARIABLE,
    FUNCTION,
    PARAMETER
};

enum class PassMode {
    VALUE,
    REFERENCE
};

/*******************************************************************************
 ***************************** Parent Entry Class ******************************
 *******************************************************************************/

class Entry {
    public :
        // Variables
        std::string  id;
        EntryType    eType;
        TypePtr      type;
        unsigned int nestingLevel;

        // Methods
        virtual ~Entry() {  }

        virtual int getOffset();
        virtual void setOffset(int offset);
        virtual int getReturns();
        virtual EntryVector getParams();
        virtual PassMode getMode();
        virtual void addParam(EntryPtr param);
        virtual void addReturn();

        virtual void print() = 0;
};

/*******************************************************************************
 ******************************* Variable Class ********************************
 *******************************************************************************/

class EntryVariable : public Entry {
    public :
        // Variables
        int offset;

        // Methods
        EntryVariable(std::string id, TypePtr type);
        virtual ~EntryVariable() {  }

        int getOffset();
        void setOffset(int offset);

        void print();
};

/*******************************************************************************
 ******************************* Function Class ********************************
 *******************************************************************************/

class EntryFunction : public Entry {
    public :
        // Variables
        int         returns;
        EntryVector params;

        // Methods
        EntryFunction(std::string id, TypePtr type);
        virtual ~EntryFunction() {  }

        int getReturns();
        EntryVector getParams();
        void addParam(EntryPtr param);
        void addReturn();

        void print();
};

/*******************************************************************************
 ******************************* Parameter Class *******************************
 *******************************************************************************/

class EntryParameter : public Entry {
    public :
        // Variables
        int      offset;
        PassMode mode;

        // Methods
        EntryParameter(std::string id, TypePtr type, PassMode mode);
        virtual ~EntryParameter() {  }

        int getOffset();
        void setOffset(int offset);
        PassMode getMode();

        void print();
};

#endif
