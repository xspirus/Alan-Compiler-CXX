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

#include <llvm/IR/Value.h>

#include <symbol/types.hpp>
#include <symbol/entry.hpp>
#include <symbol/table.hpp>

extern int linecount;

/*******************************************************************************
 * Alan AST or Abstract Syntax Tree
 * > Consists of a lot of different types of nodes :
 *   - Int -> integer constant
 *     > contains value
 *   - Byte -> byte constant
 *     > contains value
 *   - String -> string literal
 *     > contains value
 *   - Var -> variable ( e.g. in `x = 1`, x is the variable )
 *     > name
 *     > index ( null for normal variables, set for array variables )
 *   - BinOp -> binary operator ( +, -, *, /, % )
 *     > operator
 *     > lhs ( maybe null )
 *     > rhs
 *   - Condition -> TRUE, FALSE or operator ( &&, ||, !, <, <=, >, >=, ==, != )
 *     > operator
 *     > lhs ( maybe null )
 *     > rhs ( maybe null, in case of TRUE, FALSE )
 *   - IfElse -> if else statement ( else may be null )
 *     > condition
 *     > if body
 *     > else body
 *   - While -> while statement
 *     > condition
 *     > loop body
 *   - Call -> function call
 *     > name of function
 *     > normal parameters
 *     > hidden parameters ( i.e. needed -> check entry.hpp )
 *   - Ret -> return statement
 *     > expression of the return stmt ( e.g. return x + 5 -> expr = x + 5 )
 *   - Assign -> variable assignment
 *     > lvalue
 *     > rvalue
 *   - VarDecl -> variable declaration
 *     > name
 *   - Param -> typical parameter
 *     > name
 *     > mode ( by value or by reference )
 *   - Func -> function
 *     > main -> bool to tell first function
 *     > name
 *     > parameters
 *     > hidden parameters
 *     > declarations ( funcs and vars )
 *     > body ( statements )
 *   - Block -> compound statement
 *     > vector of statements
 *******************************************************************************/

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
        virtual llvm::Value* codegen() = 0;

        virtual void fixCalls();
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;

        void fixCalls() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;
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
        llvm::Value* codegen() override;

        void fixCalls() override;
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
        llvm::Value* codegen() override;

        void fixCalls() override;
};

/*******************************************************************************
 ***************************** Semantic & Compile ******************************
 *******************************************************************************/

void semantic(astPtr root);
void codegen(astPtr root);

} // namespace ast end

#endif
