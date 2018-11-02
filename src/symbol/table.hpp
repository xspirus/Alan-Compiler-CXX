/*******************************************************************************
 *                                                                             *
 *  Filename    : table.hpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Symbol Table header file                                     *
 *                                                                             *
 *******************************************************************************/

#ifndef __TABLE_HPP__
#define __TABLE_HPP__

#include <memory>
#include <deque>
#include <unordered_map>

#include <symbol/types.hpp>
#include <symbol/entry.hpp>
#include <symbol/scope.hpp>

/*******************************************************************************
 * Symbol Table for Semantic Analysis :
 *   - HashTable of variables
 *     > Every symbol table entry is inserted here.
 *     > To emulate a normal HashTable ( that is allow more than one entry of
 *     > the same name ), we use a stack. However, one cannot add an entry of
 *     > the same scope to the table and an error occurs.
 *   - Stack of scopes
 *     > Every scope is linked to a function.
 *     > Also helps with nestingLevel determining which entries to clean up
 *     > when closing a scope.
 *******************************************************************************/

namespace sem {

/*******************************************************************************
 ********************************** Typedefs ***********************************
 *******************************************************************************/

typedef std::deque<ScopePtr> ScopeStack;
typedef std::deque<EntryPtr> EntryStack;
typedef std::unordered_map<std::string, EntryStack> HashTable;

/*******************************************************************************
 ******************************** Enumerations *********************************
 *******************************************************************************/

enum class Lookup {
    CURRENT,
    ALL
};

/*******************************************************************************
 ***************************** Symbol Table Class ******************************
 *******************************************************************************/

class Table {
    public :
        // Variables
        HashTable  entries;
        ScopeStack scopes;

        // Methods
        void openScope(EntryPtr fun);
        void closeScope();
        void addLibs();
        TypePtr scopeType();
        void addReturn();
        void addParam(EntryPtr entry);
        void addHidden(EntryPtr entry);
        void insertEntry(EntryPtr entry);
        EntryPtr lookupEntry(std::string id, Lookup l, bool err);

        ScopePtr getScope();
};

typedef std::shared_ptr<Table> SymbolTable;

/*******************************************************************************
 * Initializes the symbol table by creating a global scope.
 * It then inserts all lib functions to the table so that we
 * can perform semantic analysis ( calls addLibs() ).
 *******************************************************************************/
SymbolTable initSymbolTable();

} // end namespace sem

#endif
