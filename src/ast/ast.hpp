/*******************************************************************************
 *                                                                             *
 *  Filename    : ast.hpp                                                      *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Abstract Syntax Tree classes                                 *
 *                                                                             *
 *******************************************************************************/

#ifndef __AST_HPP__
#define __AST_HPP__

#include <memory>
#include <vector>
#include <unordered_map>

#include <symbol/types.hpp>
#include <symbol/entry.hpp>
#include <symbol/table.hpp>

extern int linecount;

namespace ast {

/*******************************************************************************
 ********************************** Typedefs ***********************************
 *******************************************************************************/

class Node;

typedef std::shared_ptr<Node>                   astPtr;
typedef std::vector<astPtr>                     astVec;
typedef std::unordered_map<std::string, astVec> astVecMap;

/*******************************************************************************
 *************************** Conditions enumeration ****************************
 *******************************************************************************/

enum class Cond {
    TRU,
    FALS,
    LT,
    GT,
    LE,
    GE,
    EQ,
    NEQ,
    AND,
    OR,
    NOT
};

/*******************************************************************************
 ************************* Parent Class for All Nodes **************************
 *******************************************************************************/

class Node {
    public :
        sem::TypePtr type;
        int          line;

        Node();
        virtual ~Node() = default;

        virtual void semantic(sem::SymbolTable symtable) = 0;

        virtual void fixCalls(astVecMap &hiddenMap);
};

/*******************************************************************************
 ****************************** Integer Constants ******************************
 *******************************************************************************/

class Int : public Node {
    public :
        int val;

        Int(int val);
        virtual ~Int() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ******************************* Byte Constants ********************************
 *******************************************************************************/

class Byte : public Node {
    public :
        unsigned char b;

        Byte(unsigned char b);
        virtual ~Byte() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ******************************* String Literals *******************************
 *******************************************************************************/

class String : public Node {
    public :
        std::string s;

        String(std::string s);
        virtual ~String() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ********************************** Variables **********************************
 *******************************************************************************/

class Var : public Node {
    public :
        std::string id;
        astPtr      index;

        Var(std::string id, astPtr index);
        virtual ~Var() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ****************************** Binary Operations ******************************
 *******************************************************************************/

class BinOp : public Node {
    public :
        char   op;
        astPtr left;
        astPtr right;

        BinOp(char op, astPtr left, astPtr right);
        virtual ~BinOp() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ********************************* Conditions **********************************
 *******************************************************************************/

class Condition : public Node {
    public :
        Cond   op;
        astPtr left;
        astPtr right;

        Condition(Cond op, astPtr left, astPtr right);
        virtual ~Condition() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 *********************************** IfElse ************************************
 *******************************************************************************/

class IfElse : public Node {
    public :
        astPtr cond;
        astPtr ifBody;
        astPtr elseBody;

        IfElse(astPtr cond, astPtr ifBody, astPtr elseBody);
        virtual ~IfElse() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ************************************ While ************************************
 *******************************************************************************/

class While : public Node {
    public :
        astPtr cond;
        astPtr body;

        While(astPtr cond, astPtr body);
        virtual ~While() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ******************************** Function Call ********************************
 *******************************************************************************/

class Call : public Node {
    public :
        std::string id;
        astVec      params;
        astVec      hidden;

        Call(std::string id, astVec params);
        virtual ~Call() = default;

        void semantic(sem::SymbolTable symtable) override;

        void fixCalls(astVecMap &hiddenMap) override;
};

/*******************************************************************************
 ****************************** Function Returns *******************************
 *******************************************************************************/

class Ret : public Node {
    public :
        astPtr expr;

        Ret(astPtr expr);
        virtual ~Ret() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ********************************* Assignments *********************************
 *******************************************************************************/

class Assign : public Node {
    public :
        astPtr left;
        astPtr right;

        Assign(astPtr left, astPtr right);
        virtual ~Assign() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 **************************** Variable Declarations ****************************
 *******************************************************************************/

class VarDecl : public Node {
    public :
        std::string id;

        VarDecl(std::string id, sem::TypePtr type);
        virtual ~VarDecl() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ********************************* Parameters **********************************
 *******************************************************************************/

class Param : public Node {
    public :
        std::string   id;
        sem::PassMode mode;

        Param(std::string id, sem::PassMode mode, sem::TypePtr type);
        virtual ~Param() = default;

        void semantic(sem::SymbolTable symtable) override;
};

/*******************************************************************************
 ********************************** Functions **********************************
 *******************************************************************************/

class Func : public Node {
    public :
        std::string id;
        bool        main;
        astVec      params;
        astVec      hidden;
        astVec      decls;
        astPtr      body;

        Func(std::string id, astVec params, sem::TypePtr type, astVec decls, astPtr body);
        virtual ~Func() = default;

        void semantic(sem::SymbolTable symtable) override;

        void fixCalls(astVecMap &hiddenMap) override;
};

/*******************************************************************************
 ***************************** Compound Statements *****************************
 *******************************************************************************/

class Block : public Node {
    public :
        astVec stmts;

        Block(astVec stmts);
        virtual ~Block() = default;

        void semantic(sem::SymbolTable symtable) override;

        void fixCalls(astVecMap &hiddenMap) override;
};

/*******************************************************************************
 ***************************** Semantic & Compile ******************************
 *******************************************************************************/

void semantic(astPtr root);
void codegen(astPtr root);

} // namespace ast end

#endif
