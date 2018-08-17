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

/*******************************************************************************
 ********************************** Typedefs ***********************************
 *******************************************************************************/

class NodeAST;

typedef std::shared_ptr<NodeAST> ast;
typedef std::vector<ast>         astVec;

extern int linecount;

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

class NodeAST {
    public :
        TypePtr type;
        int     line;

        NodeAST();
        virtual ~NodeAST() = default;
};

/*******************************************************************************
 ****************************** Integer Constants ******************************
 *******************************************************************************/

class IntAST : public NodeAST {
    public :
        int val;

        IntAST(int val);
        virtual ~IntAST() = default;
};

/*******************************************************************************
 ******************************* Byte Constants ********************************
 *******************************************************************************/

class ByteAST : public NodeAST {
    public :
        unsigned char b;

        ByteAST(unsigned char b);
        virtual ~ByteAST() = default;
};

/*******************************************************************************
 ******************************* String Literals *******************************
 *******************************************************************************/

class StringAST : public NodeAST {
    public :
        std::string s;

        StringAST(std::string s);
        virtual ~StringAST() = default;
};

/*******************************************************************************
 ********************************** Variables **********************************
 *******************************************************************************/

class VarAST : public NodeAST {
    public :
        std::string id;
        ast index;

        VarAST(std::string id, ast index);
        virtual ~VarAST() = default;
};

/*******************************************************************************
 ****************************** Binary Operations ******************************
 *******************************************************************************/

class BinOpAST : public NodeAST {
    public :
        char op;
        ast  left;
        ast  right;

        BinOpAST(char op, ast left, ast right);
        virtual ~BinOpAST() = default;
};

/*******************************************************************************
 ********************************* Conditions **********************************
 *******************************************************************************/

class CondAST : public NodeAST {
    public :
        Cond op;
        ast  left;
        ast  right;

        CondAST(Cond op, ast left, ast right);
        virtual ~CondAST() = default;
};

/*******************************************************************************
 *********************************** IfElse ************************************
 *******************************************************************************/

class IfElseAST : public NodeAST {
    public :
        ast cond;
        ast ifBody;
        ast elseBody;

        IfElseAST(ast cond, ast ifBody, ast elseBody);
        virtual ~IfElseAST() = default;
};

/*******************************************************************************
 ************************************ While ************************************
 *******************************************************************************/

class WhileAST : public NodeAST {
    public :
        ast cond;
        ast body;

        WhileAST(ast cond, ast body);
        virtual ~WhileAST() = default;
};

/*******************************************************************************
 ******************************** Function Call ********************************
 *******************************************************************************/

class CallAST : public NodeAST {
    public :
        std::string id;
        astVec params;
        astVec hidden;

        CallAST(std::string id, astVec params);
        virtual ~CallAST() = default;
};

/*******************************************************************************
 ****************************** Function Returns *******************************
 *******************************************************************************/

class RetAST : public NodeAST {
    public :
        ast expr;

        RetAST(ast expr);
        virtual ~RetAST() = default;
};

/*******************************************************************************
 ********************************* Assignments *********************************
 *******************************************************************************/

class AssignAST : public NodeAST {
    public :
        ast left;
        ast right;

        AssignAST(ast left, ast right);
        virtual ~AssignAST() = default;
};

/*******************************************************************************
 **************************** Variable Declarations ****************************
 *******************************************************************************/

class VarDeclAST : public NodeAST {
    public :
        std::string id;

        VarDeclAST(std::string id, TypePtr type);
        virtual ~VarDeclAST() = default;
};

/*******************************************************************************
 ********************************* Parameters **********************************
 *******************************************************************************/

class ParamAST : public NodeAST {
    public :
        std::string id;
        PassMode mode;

        ParamAST(std::string id, PassMode mode, TypePtr type);
        virtual ~ParamAST() = default;
};

/*******************************************************************************
 ********************************** Functions **********************************
 *******************************************************************************/

class FuncAST : public NodeAST {
    public :
        std::string id;
        astVec params;
        astVec hidden;
        astVec decls;
        ast body;

        FuncAST(std::string id, astVec params, TypePtr type, astVec decls, ast body);
        virtual ~FuncAST() = default;
};

/*******************************************************************************
 ***************************** Compound Statements *****************************
 *******************************************************************************/

class BlockAST : public NodeAST {
    public :
        astVec stmts;

        BlockAST(astVec stmts);
        virtual ~BlockAST() = default;
};

#endif
