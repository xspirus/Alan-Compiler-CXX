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

#include <symbol/types.hpp>
#include <symbol/entry.hpp>

extern int linecount;

namespace ast {

/*******************************************************************************
 ********************************** Typedefs ***********************************
 *******************************************************************************/

class Node;

typedef std::shared_ptr<Node> astPtr;
typedef std::vector<astPtr>   astVec;

/*******************************************************************************
 *************************** Conditions enumeration ****************************
 *******************************************************************************/

enum class Cond {
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
        int     line;

        Node();
        virtual ~Node() = default;
};

/*******************************************************************************
 ****************************** Integer Constants ******************************
 *******************************************************************************/

class Int : public Node {
    public :
        int val;

        Int(int val);
        virtual ~Int() = default;
};

/*******************************************************************************
 ******************************* Byte Constants ********************************
 *******************************************************************************/

class Byte : public Node {
    public :
        unsigned char b;

        Byte(unsigned char b);
        virtual ~Byte() = default;
};

/*******************************************************************************
 ******************************* String Literals *******************************
 *******************************************************************************/

class String : public Node {
    public :
        std::string s;

        String(std::string s);
        virtual ~String() = default;
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
};

/*******************************************************************************
 ****************************** Function Returns *******************************
 *******************************************************************************/

class Ret : public Node {
    public :
        astPtr expr;

        Ret(astPtr expr);
        virtual ~Ret() = default;
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
};

/*******************************************************************************
 **************************** Variable Declarations ****************************
 *******************************************************************************/

class VarDecl : public Node {
    public :
        std::string id;

        VarDecl(std::string id, sem::TypePtr type);
        virtual ~VarDecl() = default;
};

/*******************************************************************************
 ********************************* Parameters **********************************
 *******************************************************************************/

class Param : public Node {
    public :
        std::string id;
        sem::PassMode    mode;

        Param(std::string id, sem::PassMode mode, sem::TypePtr type);
        virtual ~Param() = default;
};

/*******************************************************************************
 ********************************** Functions **********************************
 *******************************************************************************/

class Func : public Node {
    public :
        std::string id;
        astVec      params;
        astVec      hidden;
        astVec      decls;
        astPtr      body;

        Func(std::string id, astVec params, sem::TypePtr type, astVec decls, astPtr body);
        virtual ~Func() = default;
};

/*******************************************************************************
 ***************************** Compound Statements *****************************
 *******************************************************************************/

class Block : public Node {
    public :
        astVec stmts;

        Block(astVec stmts);
        virtual ~Block() = default;
};

} // namespace ast end

#endif
