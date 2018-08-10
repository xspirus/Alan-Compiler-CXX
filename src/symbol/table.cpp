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
#include <stack>

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
        nestingLevel = scopes.top()->nestingLevel + 1;
    std::cout << "Opening scope of function \"" << fun->id << "\"" << std::endl;
    std::cout << "-- Nesting Level : " << nestingLevel << std::endl;
    std::cout << "-- Return Type : "; fun->type->print(); std::cout << std::endl;
    scopes.push(std::make_shared<Scope>(nestingLevel, fun));
}

void Table::closeScope() {
    if ( scopes.empty() ) {
        warning("No scopes to close");
        return;
    }
    unsigned int nestingLevel = scopes.top()->nestingLevel;
    for ( auto& e : this->entries ) {
        auto& s = e.second;
        while ( !s.empty() ) {
            if ( s.top()->nestingLevel != nestingLevel )
                break;
            s.pop();
        }
    }
}

TypePtr Table::scopeType() {
    return this->scopes.top()->fun->type;
}

void Table::addReturn() {
    this->scopes.top()->fun->addReturn();
}

void Table::insertEntry(EntryPtr entry) {
    entry->nestingLevel = scopes.top()->nestingLevel;
    switch( entry->eType ) {
        case EntryType::VARIABLE :
        case EntryType::PARAMETER :
            entry->setOffset(scopes.top()->currentOffset++);
            break;
        case EntryType::FUNCTION :
            break;
    }
    entries[entry->id].push(entry);
    std::cout << "Inserted ";
    entry->print();
    return;
}

EntryPtr Table::lookupEntry(std::string id, Lookup l) {
    auto exists = this->entries.find(id);
    if ( exists == this->entries.end() )
        return nullptr;
    auto s = this->entries[id];
    switch( l ) {
        case Lookup::CURRENT :
            if ( s.top()->nestingLevel != this->scopes.top()->nestingLevel )
                return nullptr;
            return s.top();
        case Lookup::ALL :
            return s.top();
    }
    return nullptr;
}

ScopePtr Table::getScope() {
    return scopes.top();
}
