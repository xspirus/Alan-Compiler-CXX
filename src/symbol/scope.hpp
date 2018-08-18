/*******************************************************************************
 *                                                                             *
 *  Filename    : scope.hpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Scope class for symbol table                                 *
 *                                                                             *
 *******************************************************************************/

#ifndef __SCOPE_HPP__
#define __SCOPE_HPP__

#include <symbol/entry.hpp>

namespace sem {

/*******************************************************************************
 ********************************** Typedefs ***********************************
 *******************************************************************************/

class Scope;

typedef std::shared_ptr<Scope> ScopePtr;

/*******************************************************************************
 ********************************* Scope Class *********************************
 *******************************************************************************/

class Scope {
    public :
        // Variables
        unsigned int nestingLevel;
        unsigned int currentOffset;
        EntryPtr     fun;

        // Methods
        Scope(unsigned int nestingLevel, EntryPtr fun);

        EntryPtr getFunction();
};

} // end namespace sem

#endif
