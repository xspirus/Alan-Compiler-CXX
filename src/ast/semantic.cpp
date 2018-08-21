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

#include <error/error.hpp>
#include <ast/ast.hpp>

#include <iostream>
#include <vector>

namespace ast {

/*******************************************************************************
 ****************************** Integer Constants ******************************
 *******************************************************************************/

void Int::semantic(sem::SymbolTable symtable) {
    std::cerr << "Found an integer" << std::endl;
    return;
}

/*******************************************************************************
 ******************************* Byte Constants ********************************
 *******************************************************************************/

void Byte::semantic(sem::SymbolTable symtable) {
    std::cerr << "Found a byte" << std::endl;
    return;
}

/*******************************************************************************
 ******************************* String Literals *******************************
 *******************************************************************************/

void String::semantic(sem::SymbolTable symtable) {
    std::cerr << "Found a string literal" << std::endl;
    return;
}

/*******************************************************************************
 ********************************** Variables **********************************
 *******************************************************************************/

void Var::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found variable " << this->id << " usage" << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
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
    std::cerr << "Inferenced type of variable "
              << this->id
              << " : "
              << *(this->type)
              << std::endl;
    return;
}

/*******************************************************************************
 ****************************** Binary Operations ******************************
 *******************************************************************************/

void BinOp::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found an operation" << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
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
    std::cerr << "Inferenced type of operation : "
              << *(this->type)
              << std::endl;
    return;
}

/*******************************************************************************
 ********************************* Conditions **********************************
 *******************************************************************************/

void Condition::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found a condition" << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
    switch(this->op) {
        case Cond::NOT :
            this->right->semantic(symtable);
            if ( !sem::equalType(this->right->type, sem::typeByte) ) {
                error("Condition is not of type byte");
                return;
            }
            this->type = sem::typeByte;
            return;
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
            return;
        default :
            this->left->semantic(symtable);
            this->right->semantic(symtable);
            if ( !sem::equalType(this->left->type, this->right->type) ) {
                error("Expressions of different types");
                return;
            }
            this->type = sem::typeByte;
            return;
    }
    return;
}

/*******************************************************************************
 *********************************** IfElse ************************************
 *******************************************************************************/

void IfElse::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found a IfElse statement" << std::endl;
    std::cerr << "Performing semantic analysis..."
              << std::endl;
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
    std::cerr << "Condition checks out" << std::endl;
    std::cerr << "Performing semantic analysis of if body..." << std::endl;
    this->ifBody->semantic(symtable);
    std::cerr << "If checks out" << std::endl;
    if ( this->elseBody != nullptr ) {
        std::cerr << "Performing semantic analysis of else body..." << std::endl;
        this->elseBody->semantic(symtable);
        std::cerr << "Else checks out" << std::endl;
    }
    std::cerr << "IfElse completed" << std::endl;
    return;
}

/*******************************************************************************
 ************************************ While ************************************
 *******************************************************************************/

void While::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found a while statement" << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
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
    std::cerr << "Condition checks out" << std::endl;
    std::cerr << "Performing semantic analysis of while body..." << std::endl;
    this->body->semantic(symtable);
    std::cerr << "While completed" << std::endl;
}

/*******************************************************************************
 ******************************** Function Call ********************************
 *******************************************************************************/

void Call::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found a function call of function " << this->id << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
    auto entry = symtable->lookupEntry(this->id, sem::Lookup::ALL, true);
    if ( entry->eType != sem::EntryType::FUNCTION ) {
        error("%s is not a function", this->id.c_str());
        return;
    }
    /**
     * Check number of params
     */
    std::cerr << "Checking number of params..." << std::endl;
    if ( this->params.size() < entry->getParams().size() ) {
        error("Not enough arguments");
        return;
    } else if ( this->params.size() > entry->getParams().size() ) {
        error("Too many arguments");
        return;
    }
    std::cerr << "Check completed" << std::endl;
    /**
     * Complete semantic analysis of given parameters
     */
    std::cerr << "Performing semantic analysis of parameters given..." << std::endl;
    for ( auto p : this->params ) {
        p->semantic(symtable);
    }
    std::cerr << "Parameters completed" << std::endl;
    /**
     * Compare given with original parameters
     */
    std::cerr << "Checking parameter types..." << std::endl;
    auto& pars = entry->getParams();
    for ( int i = 0; i < this->params.size(); i++ ) {
        if ( !sem::compatibleType(this->params[i]->type, pars[i]->type) ) {
            error("Type mismatch in function call");
            return;
        }
    }
    std::cerr << "Type check completed" << std::endl;
    /**
     * Add parameters which need to be added
     * for the function to work properly
     */
    std::cerr << "Adding hidden parameters..." << std::endl;
    auto& hids = entry->getHidden();
    for ( int i = 0; i < hids.size(); i++ ) {
        auto v = std::make_shared<Var>(hids[i]->id, nullptr);
        v->type = hids[i]->type;
        this->hidden.push_back(v);
    }
    this->type = entry->type;
    std::cerr << "Call completed" << std::endl;
    return;
}

/*******************************************************************************
 ****************************** Function Returns *******************************
 *******************************************************************************/

void Ret::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found a return statement" << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
    this->expr->semantic(symtable);
    this->type = this->expr->type;
    std::cerr << "Checking type of return with function type..." << std::endl;
    if ( !sem::compatibleType(this->type, symtable->scopeType()) ) {
        error("Type mismatch in function return");
        return;
    }
    std::cerr << "Type checking completed" << std::endl;
    symtable->addReturn();
    std::cerr << "Return completed" << std::endl;
}

/*******************************************************************************
 ********************************* Assignments *********************************
 *******************************************************************************/

void Assign::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found an assignment" << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
    this->left->semantic(symtable);
    this->right->semantic(symtable);
    std::cerr << "Checking types..." << std::endl;
    if ( !sem::equalType(this->left->type, this->right->type) ) {
        error("Type mismatch in assignment");
        return;
    }
    std::cerr << "Type checking completed" << std::endl;
    this->type = this->left->type;
    std::cerr << "Assignment completed" << std::endl;
}

/*******************************************************************************
 **************************** Variable Declarations ****************************
 *******************************************************************************/

void VarDecl::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found variable declaration of " << this->id << " : " << *(this->type) << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
    std::cerr << "Checking if already existing..." << std::endl;
    auto entry = symtable->lookupEntry(this->id, sem::Lookup::CURRENT, false);
    if ( entry != nullptr ) {
        error("Duplicate identifier");
        return;
    }
    symtable->insertEntry(std::make_shared<sem::EntryVariable>(this->id, this->type));
    std::cerr << "Declaration completed" << std::endl;
}

/*******************************************************************************
 ********************************* Parameters **********************************
 *******************************************************************************/

void Param::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found parameter " << this->id << std::endl;
    auto p = std::make_shared<sem::EntryParameter>(this->id, this->type, this->mode);
    symtable->insertEntry(p);
    symtable->addParam(p);
    std::cerr << "Parameter added" << std::endl;
    return;
}

/*******************************************************************************
 ********************************** Functions **********************************
 *******************************************************************************/

void Func::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found function declaration : " << this->id << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
    std::cerr << "Checking if already existing..." << std::endl;
    auto entry = symtable->lookupEntry(this->id, sem::Lookup::ALL, false);
    if ( entry != nullptr ) {
        error("Duplicate identifier");
        return;
    }
    std::cerr << "Check completed" << std::endl;
    auto fun = std::make_shared<sem::EntryFunction>(this->id, this->type);
    symtable->insertEntry(fun);
    symtable->openScope(fun);
    std::cerr << "Adding parameters of function..." << std::endl;
    for ( auto p : this->params ) {
        p->semantic(symtable);
    }
    std::cerr << "Parameter adding completed" << std::endl;
    std::cerr << "Performing semantic analysis of declarations..." << std::endl;
    for ( auto d : this->decls ) {
        d->semantic(symtable);
    }
    std::cerr << "Declarations completed" << std::endl;
    std::cerr << "Performing semantic analysis on function body..." << std::endl;
    this->body->semantic(symtable);
    std::cerr << "Body completed" << std::endl;
    std::cerr << "Adding hidden variables before closing scope..." << std::endl;
    entry = symtable->lookupEntry(this->id, sem::Lookup::ALL, false);
    for ( auto hid : entry->getHidden() ) {
        this->hidden.push_back(std::make_shared<Param>(hid->id, sem::PassMode::REFERENCE, hid->type));
    }
    std::cerr << "Hiddens completed" << std::endl;
    symtable->closeScope();
    std::cerr << "Function completed" << std::endl;
    return;
}

/*******************************************************************************
 ***************************** Compound Statements *****************************
 *******************************************************************************/

void Block::semantic(sem::SymbolTable symtable) {
    linecount = this->line;
    std::cerr << "Found a block statement" << std::endl;
    std::cerr << "Performing semantic analysis..." << std::endl;
    for ( auto s : this->stmts ) {
        s->semantic(symtable);
    }
    std::cerr << "Block completed" << std::endl;
    return;
}

/*******************************************************************************
 ********************************** Semantic ***********************************
 *******************************************************************************/

void semantic(astPtr root) {
#ifndef DEBUG
    std::cerr.setstate(std::ios_base::failbit);
#endif
    std::cerr << "Preparing for semantic analysis..." << std::endl;
    auto symtable = sem::initSymbolTable();
    root->semantic(symtable);
    std::cerr.clear();
    std::cerr << "Semantic analysis completed" << std::endl;
    return;
}

} // end namespace ast
