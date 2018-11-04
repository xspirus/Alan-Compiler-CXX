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

#include <message/message.hpp>
#include <symbol/types.hpp>
#include <symbol/entry.hpp>
#include <symbol/table.hpp>
#include <general/general.hpp>

namespace sem {

/*******************************************************************************
 ******************************** Symbol Table *********************************
 *******************************************************************************/

void Table::openScope(EntryPtr fun) {
    unsigned int nestingLevel;
    if ( scopes.empty() )
        nestingLevel = 1;
    else
        nestingLevel = scopes.front()->nestingLevel + 1;
    scopes.push_front(newShared<Scope>(nestingLevel, fun));
}

/*******************************************************************************
 * For each entry name :
 * > remove the entry with the same nesting
 * > level as the scopes function
 *
 * We do not need to check what type of
 * entry we are deleting, as functions are
 * always inserted a level above the current
 * scope and its parameters are kept in its
 * own entry, so we can delete them.
 *******************************************************************************/
void Table::closeScope() {
    if ( scopes.empty() ) {
        warning("No scopes to close");
        return;
    }
    unsigned int nestingLevel = scopes.front()->nestingLevel;
    for ( auto& e : this->entries ) {
        auto& s = e.second;
        while ( !s.empty() ) {
            if ( s.front()->nestingLevel < nestingLevel )
                break;
            s.pop_front();
        }
    }
    scopes.pop_front();
}

TypePtr Table::scopeType() {
    return this->scopes.front()->fun->type;
}

void Table::addReturn() {
    this->scopes.front()->fun->addReturn();
}

void Table::addParam(EntryPtr entry) {
    scopes.front()->getFunction()->addParam(entry);
}

void Table::addHidden(EntryPtr entry) {
    unsigned int nestingLevel = entry->nestingLevel;
    for ( unsigned int i = 0; i < scopes.size(); i++ ) {
        if ( scopes[i]->nestingLevel > nestingLevel )
            scopes[i]->getFunction()->addHidden(entry);
        else
            break;
    }
    for ( unsigned int i = nestingLevel + 1; ; i++ ) {
        auto temp = newShared<EntryParameter>(entry->id, entry->type, PassMode::REFERENCE);
        temp->nestingLevel = i;
        entries[entry->id].push_front(temp);
        if ( i == scopes.front()->nestingLevel )
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
    return;
}

EntryPtr Table::lookupEntry(std::string id, Lookup l, bool err) {
    auto exists = this->entries.find(id);
    if ( exists == this->entries.end() ) {
        if (err)
            error("Unknown identifier ", id);
        return nullptr;
    }
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

/*******************************************************************************
 ***************************** Standard Libraries ******************************
 *******************************************************************************/

void Table::addLibs() {
    /**
     * Global scope pseudofunction
     */
    auto global = newShared<EntryFunction>("global", typeVoid);
    this->openScope(global);
    /**
     * Standard library functions :
     *   - void writeInteger(int n)
     *   - void writeByte(byte b)
     *   - void writeChar(byte b)
     *   - void writeString(reference byte s)
     *   - int readInteger()
     *   - byte readByte()
     *   - byte readChar()
     *   - void readString(int n, reference byte s)
     *   - int extend(byte b)
     *   - byte shrink(int i)
     *   - int strlen(reference byte s)
     *   - int strcmp(reference byte s1, reference byte s2)
     *   - void strcpy(reference byte trg, reference byte src)
     *   - void strcat(reference byte trg, reference byte src)
     */
    /* void writeInteger(int n) */
    auto writeInteger = newShared<EntryFunction>("writeInteger", typeVoid);
    writeInteger->addParam(newShared<EntryParameter>("n", typeInteger, PassMode::VALUE));
    this->insertEntry(writeInteger);
    /* void writeByte(byte b) */
    auto writeByte = newShared<EntryFunction>("writeByte", typeVoid);
    writeByte->addParam(newShared<EntryParameter>("b", typeByte, PassMode::VALUE));
    this->insertEntry(writeByte);
    /* void writeChar(byte b) */
    auto writeChar = newShared<EntryFunction>("writeChar", typeVoid);
    writeChar->addParam(newShared<EntryParameter>("b", typeByte, PassMode::VALUE));
    this->insertEntry(writeChar);
    /* void writeString(reference byte s) */
    auto writeString = newShared<EntryFunction>("writeString", typeVoid);
    writeString->addParam(newShared<EntryParameter>("s", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    this->insertEntry(writeString);
    /* int readInteger() */
    auto readInteger = newShared<EntryFunction>("readInteger", typeInteger);
    this->insertEntry(readInteger);
    /* byte readByte() */
    auto readByte = newShared<EntryFunction>("readByte", typeByte);
    this->insertEntry(readByte);
    /* byte readChar() */
    auto readChar = newShared<EntryFunction>("readChar", typeByte);
    this->insertEntry(readChar);
    /* void readString(int n, reference byte s) */
    auto readString = newShared<EntryFunction>("readString", typeVoid);
    readString->addParam(newShared<EntryParameter>("n", typeInteger, PassMode::VALUE));
    readString->addParam(newShared<EntryParameter>("s", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    this->insertEntry(readString);
    /* int extend(byte b) */
    auto extend = newShared<EntryFunction>("extend", typeInteger);
    extend->addParam(newShared<EntryParameter>("b", typeByte, PassMode::VALUE));
    this->insertEntry(extend);
    /* byte shrink(int i) */
    auto shrink = newShared<EntryFunction>("shrink", typeByte);
    shrink->addParam(newShared<EntryParameter>("i", typeInteger, PassMode::VALUE));
    this->insertEntry(shrink);
    /* int strlen(reference byte s) */
    auto astrlen = newShared<EntryFunction>("strlen", typeInteger);
    astrlen->addParam(newShared<EntryParameter>("s", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    this->insertEntry(astrlen);
    /* int strcmp(reference byte s1, reference byte s2) */
    auto astrcmp = newShared<EntryFunction>("strcmp", typeInteger);
    astrcmp->addParam(newShared<EntryParameter>("s1", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    astrcmp->addParam(newShared<EntryParameter>("s2", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    this->insertEntry(astrcmp);
    /* void strcpy(reference byte trg, reference byte src) */
    auto astrcpy = newShared<EntryFunction>("strcpy", typeInteger);
    astrcpy->addParam(newShared<EntryParameter>("trg", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    astrcpy->addParam(newShared<EntryParameter>("src", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    this->insertEntry(astrcpy);
    /* void strcat(reference byte trg, reference byte src) */
    auto astrcat = newShared<EntryFunction>("strcat", typeInteger);
    astrcat->addParam(newShared<EntryParameter>("trg", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    astrcat->addParam(newShared<EntryParameter>("src", newShared<TypeIArray>(typeByte), PassMode::REFERENCE));
    this->insertEntry(astrcat);
}

SymbolTable initSymbolTable() {
    SymbolTable ret = newShared<Table>();
    ret->addLibs();
    return ret;
}

} // end namespace sem
