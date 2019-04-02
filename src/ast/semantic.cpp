/*******************************************************************************
 *                                                                             *
 *  Filename    : semantic.cpp                                                 *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Semantic analysis of abstract syntax tree                    *
 *                                                                             *
 *******************************************************************************/

#include <message/message.hpp>
#include <ast/ast.hpp>
#include <general/general.hpp>

#include <iostream>
#include <vector>

static Debugger debugger;

/*******************************************************************************
 * Perform semantic analysis for the AST.
 * Calls semantic function for all nodes.
 * Every node of the ast does something different to check semantic.
 * Also can print AST via the Debugger class.
 *******************************************************************************/

namespace ast {

static bool first = true;

astVecMap hiddenMap;

void Node::fixCalls() {
    return;
}

/*******************************************************************************
 ****************************** Integer Constants ******************************
 *******************************************************************************/

void Int::semantic(sem::SymbolTable symtable) {
    debugger.newLevel();
    debugger.show("<Integer, ", this->val, ">");
    debugger.restoreLevel();
    return;
}

/*******************************************************************************
 ******************************* Byte Constants ********************************
 *******************************************************************************/

void Byte::semantic(sem::SymbolTable symtable) {
    debugger.newLevel();
    debugger.show("<Byte, ", (int)this->b, ">");
    debugger.restoreLevel();
    return;
}

/*******************************************************************************
 ******************************* String Literals *******************************
 *******************************************************************************/

void String::semantic(sem::SymbolTable symtable) {
    debugger.newLevel();
    debugger.show("<String Literal, ", this->s, ">");
    debugger.restoreLevel();
    return;
}

/*******************************************************************************
 ********************************** Variables **********************************
 *******************************************************************************/

void Var::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    /* Array Variable */
    if ( this->index != nullptr ) {
        this->index->semantic(symtable);
        if ( !sem::equalType(this->index->type, sem::typeInteger) ) {
            error("Array index must be of integer type");
            return;
        }
    }
    /**
     * Find corresponding table entry
     */
    auto entry = symtable->lookupEntry(this->id, sem::Lookup::ALL, true);
    if ( entry->eType == sem::EntryType::FUNCTION ) {
        error("Not a variable/parameter");
        return;
    }
    /**
     * If not found in current scope then add
     * as hidden variable for all previous functions
     */
    if ( entry->nestingLevel < symtable->getScope()->nestingLevel) {
        symtable->addHidden(entry);
    }
    /**
     * Fix type of variable
     *   if normal get type
     *   if array get refType
     */
    if ( this->index == nullptr ) {
        this->type = entry->type;
    } else {
        this->type = entry->type->getRef();
    }
    debugger.show("<Var, ", this->id, ", ", *this->type, ">");
    debugger.restoreLevel();
}

/*******************************************************************************
 ****************************** Binary Operations ******************************
 *******************************************************************************/

void BinOp::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<BinOp>");
    /**
     * Perform semantic analysis
     * of the two operands
     */
    this->left->semantic(symtable);
    this->right->semantic(symtable);
    /**
     * Check types of expressions
     *   - must be of same type
     */
    if ( !sem::equalType(this->left->type, this->right->type) ) {
        error("Binary operation operands must of same type");
        return;
    }
    this->type = this->left->type;
    debugger.restoreLevel();
}

/*******************************************************************************
 ********************************* Conditions **********************************
 *******************************************************************************/

void Condition::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<Condition>");
    switch(this->op) {
        case Cond::NOT :
            this->right->semantic(symtable);
            if ( !sem::equalType(this->right->type, sem::typeByte) ) {
                error("Condition is not of type byte");
                return;
            }
            this->type = sem::typeByte;
            break;
        case Cond::AND :
        case Cond::OR :
            this->left->semantic(symtable);
            this->right->semantic(symtable);
            if ( !sem::equalType(this->left->type, sem::typeByte)
                    || !sem::equalType(this->right->type, sem::typeByte)
               ) {
                error("Condition is not of type byte");
                return;
            }
            this->type = sem::typeByte;
            break;
        case Cond::TRU :
        case Cond::FALS :
            this->type = sem::typeByte;
            break;
        default :
            this->left->semantic(symtable);
            this->right->semantic(symtable);
            if ( !sem::equalType(this->left->type, this->right->type) ) {
                error("Expressions of different types");
                return;
            }
            this->type = sem::typeByte;
            break;
    }
    debugger.restoreLevel();
}

/*******************************************************************************
 *********************************** IfElse ************************************
 *******************************************************************************/

void IfElse::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<IfElse>");
    /**
     * Perform semantic analysis in the order :
     *   - condition (performs necessary checks)
     *   - if body
     *   - else body
     */
    this->cond->semantic(symtable);
    if ( !sem::equalType(this->cond->type, sem::typeByte) ) {
        error("If condition expects a boolean expression");
        return;
    }
    this->ifBody->semantic(symtable);
    if ( this->elseBody != nullptr ) {
        this->elseBody->semantic(symtable);
    }
    debugger.restoreLevel();
}

/*******************************************************************************
 ************************************ While ************************************
 *******************************************************************************/

void While::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<While>");
    /**
     * Perform semantic analysis in the order :
     *   - condition
     *   - body
     */
    this->cond->semantic(symtable);
    if ( !sem::equalType(this->cond->type, sem::typeByte) ) {
        error("While condition expects boolean expression");
        return;
    }
    this->body->semantic(symtable);
    debugger.restoreLevel();
}

/*******************************************************************************
 ******************************** Function Call ********************************
 *******************************************************************************/

void Call::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<FunctionCall, ", this->id, ">");
    auto entry = symtable->lookupEntry(this->id, sem::Lookup::ALL, true);
    if ( entry->eType != sem::EntryType::FUNCTION ) {
        error(this->id, " is not a function");
        return;
    }
    /**
     * Check number of params
     */
    if ( this->params.size() < entry->getParams().size() ) {
        error("Not enough arguments");
        return;
    } else if ( this->params.size() > entry->getParams().size() ) {
        error("Too many arguments");
        return;
    }
    /**
     * Complete semantic analysis of given parameters
     */
    for ( auto p : this->params ) {
        p->semantic(symtable);
    }
    /**
     * Compare given with original parameters
     */
    auto& pars = entry->getParams();
    for ( int i = 0; i < this->params.size(); i++ ) {
        if ( !sem::compatibleType(this->params[i]->type, pars[i]->type) ) {
            error("Type mismatch in parameter ", pars[i]->id, "\nExpected ", this->params[i]->type);
            return;
        }
    }
    this->type = entry->type;
    debugger.restoreLevel();
}

void Call::fixCalls() {
    for ( auto hid : hiddenMap[this->id] ) {
        auto temp = std::dynamic_pointer_cast<Param>(hid);
        auto v = newShared<Var>(temp->id, nullptr);
        v->type = temp->type;
        this->hidden.push_back(v);
    }
}

/*******************************************************************************
 ****************************** Function Returns *******************************
 *******************************************************************************/

void Ret::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<Return>");
    this->expr->semantic(symtable);
    this->type = this->expr->type;
    if ( !sem::compatibleType(this->type, symtable->scopeType()) ) {
        error("Type mismatch in function return");
        return;
    }
    symtable->addReturn();
    debugger.restoreLevel();
}

/*******************************************************************************
 ********************************* Assignments *********************************
 *******************************************************************************/

void Assign::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<Assignment>");
    this->left->semantic(symtable);
    this->right->semantic(symtable);
    if ( !sem::equalType(this->left->type, this->right->type) ) {
        error("Type mismatch in assignment");
        return;
    }
    this->type = this->left->type;
    debugger.restoreLevel();
}

/*******************************************************************************
 **************************** Variable Declarations ****************************
 *******************************************************************************/

void VarDecl::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<VarDecl, ", this->id, ", ", *this->type, ">");
    auto entry = symtable->lookupEntry(this->id, sem::Lookup::CURRENT, false);
    if ( entry != nullptr ) {
        error("Duplicate identifier ", this->id);
        return;
    }
    symtable->insertEntry(newShared<sem::EntryVariable>(this->id, this->type));
    debugger.restoreLevel();
}

/*******************************************************************************
 ********************************* Parameters **********************************
 *******************************************************************************/

void Param::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<Parameter, ", this->id, ", ", *this->type, ">");
    auto p = newShared<sem::EntryParameter>(this->id, this->type, this->mode);
    symtable->insertEntry(p);
    symtable->addParam(p);
    debugger.restoreLevel();
}

/*******************************************************************************
 ********************************** Functions **********************************
 *******************************************************************************/

void Func::semantic(sem::SymbolTable symtable) {
    if ( ast::first ) {
        this->main = true;
        ast::first = false;
    }
    if ( !this->main ) {
        debugger.newLevel();
    }
    linecount = this->line;
    debugger.show("<Function Declaration, ", this->id, ", ", *this->type, ">");
    auto entry = symtable->lookupEntry(this->id, sem::Lookup::CURRENT, false);
    if ( entry != nullptr ) {
        error("Duplicate identifier ", this->id);
        return;
    }
    auto fun = newShared<sem::EntryFunction>(this->id, this->type);
    symtable->insertEntry(fun);
    symtable->openScope(fun);
    for ( auto p : this->params ) {
        p->semantic(symtable);
    }
    for ( auto d : this->decls ) {
        d->semantic(symtable);
    }
    this->body->semantic(symtable);
    entry = symtable->lookupEntry(this->id, sem::Lookup::ALL, false);
    for ( auto hid : entry->getHidden() ) {
        this->hidden.push_back(newShared<Param>(hid->id, sem::PassMode::REFERENCE, hid->type));
    }
    symtable->closeScope();
    if ( !this->main ) {
        debugger.restoreLevel();
    }
}

void Func::fixCalls() {
    hiddenMap[this->id] = this->hidden;
    for ( auto d : this->decls )
        d->fixCalls();
    this->body->fixCalls();
}

/*******************************************************************************
 ***************************** Compound Statements *****************************
 *******************************************************************************/

void Block::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    debugger.newLevel();
    debugger.show("<Block Statement>");
    for ( auto s : this->stmts ) {
        s->semantic(symtable);
    }
    debugger.restoreLevel();
    return;
}

void Block::fixCalls() {
    for ( auto s : this->stmts )
        s->fixCalls();
}

/*******************************************************************************
 ********************************** Semantic ***********************************
 *******************************************************************************/

void semantic(astPtr root) {
    auto symtable = sem::initSymbolTable();
    root->semantic(symtable);
    root->fixCalls();
    return;
}

} // end namespace ast
