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

namespace sem {

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
        virtual const EntryVector& getParams() const;
        virtual const EntryVector& getHidden() const;
        virtual PassMode getMode();
        virtual void addParam(EntryPtr param);
        virtual void addHidden(EntryPtr entry);
        virtual void addReturn();

        virtual void print(std::string prefix) = 0;
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

        void print(std::string prefix);
};

/*******************************************************************************
 ******************************* Function Class ********************************
 *******************************************************************************/

class EntryFunction : public Entry {
    public :
        // Variables
        int         returns;
        EntryVector params;
        EntryVector hidden;

        // Methods
        EntryFunction(std::string id, TypePtr type);
        virtual ~EntryFunction() {  }

        int getReturns();
        const EntryVector& getParams() const;
        const EntryVector& getHidden() const;
        void addParam(EntryPtr param);
        void addHidden(EntryPtr entry);
        void addReturn();

        void print(std::string prefix);
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

        void print(std::string prefix);
};

} // end namespace sem

#endif
