/*******************************************************************************
 *                                                                             *
 *  Filename    : table.cpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Symbol Table methods implementation                          *
 *                                                                             *
 *******************************************************************************/

#include <iostream>
#include <deque>

#include <error/error.hpp>
#include <symbol/table.hpp>

/*******************************************************************************
 ******************************** Symbol Table *********************************
 *******************************************************************************/

void Table::openScope(EntryPtr fun) {
    unsigned int nestingLevel;
    if ( scopes.empty() )
        nestingLevel = 1;
    else
        nestingLevel = scopes.front()->nestingLevel + 1;
    std::cout << "Opening scope of function \"" << fun->id << "\"" << std::endl;
    std::cout << "-- Nesting Level : " << nestingLevel << std::endl;
    std::cout << "-- Return Type : "; fun->type->print(); std::cout << std::endl;
    scopes.push_front(std::make_shared<Scope>(nestingLevel, fun));
}

void Table::closeScope() {
    if ( scopes.empty() ) {
        warning("No scopes to close");
        return;
    }
    unsigned int nestingLevel = scopes.front()->nestingLevel;
    /**
     * For each entry name :
     *   remove the entry with the same nesting
     *   level as the scopes function
     *
     * We do not need to check what type of
     * entry we are deleting, as functions are
     * always inserted a level above the current
     * scope and its parameters are kept in its
     * own entry, so we can delete them.
     */
    for ( auto& e : this->entries ) {
        auto& s = e.second;
        while ( !s.empty() ) {
            if ( s.front()->nestingLevel < nestingLevel )
                break;
            s.pop_front();
        }
    }
    /**
     * Finally remove scope from stack
     */
    scopes.pop_front();
}

TypePtr Table::scopeType() {
    return this->scopes.front()->fun->type;
}

void Table::addReturn() {
    this->scopes.front()->fun->addReturn();
}

void Table::addHidden(EntryPtr entry) {
    unsigned int nestingLevel = entry->nestingLevel;
    for ( unsigned int i = 0; i < scopes.size(); i++ ) {
        if ( scopes[i]->nestingLevel > nestingLevel )
            scopes[i]->getFunction()->addHidden(entry);
        else
            break;
    }
}

void Table::insertEntry(EntryPtr entry) {
    entry->nestingLevel = scopes.front()->nestingLevel;
    switch( entry->eType ) {
        case EntryType::VARIABLE :
        case EntryType::PARAMETER :
            entry->setOffset(scopes.front()->currentOffset++);
            break;
        case EntryType::FUNCTION :
            break;
    }
    entries[entry->id].push_front(entry);
    std::cout << "Inserted";
    entry->print("");
    return;
}

EntryPtr Table::lookupEntry(std::string id, Lookup l) {
    auto exists = this->entries.find(id);
    if ( exists == this->entries.end() )
        return nullptr;
    auto s = this->entries[id];
    switch( l ) {
        case Lookup::CURRENT :
            if ( s.front()->nestingLevel != this->scopes.front()->nestingLevel )
                return nullptr;
            return s.front();
        case Lookup::ALL :
            return s.front();
    }
    return nullptr;
}

ScopePtr Table::getScope() {
    return scopes.front();
}
