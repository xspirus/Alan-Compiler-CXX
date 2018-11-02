/*******************************************************************************
 *                                                                             *
 *  Filename    : codegen.hpp                                                  *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Codegen header file (llvm stuff)                             *
 *                                                                             *
 *******************************************************************************/

#ifndef __CODEGEN_HPP__
#define __CODEGEN_HPP__

#include <memory>
#include <deque>
#include <unordered_map>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/BasicBlock.h>

#include <symbol/types.hpp>
#include <symbol/entry.hpp>

/*******************************************************************************
 * FuncStack :
 *   - used for checking variables and basicblocks.
 *   > When we find a function declaration we need to remember the previous
 *   > BasicBlock of the previous function so that we can continue to emit code
 *   > there after finishing up here
 * FuncMap :
 *   - used to fetch Function* for the "active" functions.
 *   > This will help us check which arguments need to be passed by reference
 *   > so that we know a var is needed to be passed there.
 *******************************************************************************/

/*******************************************************************************
 ****************************** Class Definitions ******************************
 *******************************************************************************/

class FunctionBlock;

/*******************************************************************************
 ********************************** Typedefs ***********************************
 *******************************************************************************/

typedef std::unordered_map<std::string, llvm::AllocaInst*> ValTable;
typedef std::shared_ptr<FunctionBlock> FuncPtr;
typedef std::deque<FuncPtr> FuncStack;
typedef std::unordered_map<std::string, llvm::Function*> FuncMap;
typedef std::vector<llvm::Type*> TypeVec;
typedef std::unordered_map<std::string, llvm::Type*> TypeTable;

/*******************************************************************************
 * FunctionBlock Class
 *   - args :
 *     > Used to hold argument types so that we can pass them to
 *     > `llvm::FunctionType::get( llvm::Type*,
 *                                 args,
 *                                 false )`
 *   - vars :
 *     > Used to hold the types of all scope variables.
 *     > Useful to know when we have referenced variables
 *     > ( as parameters that is ).
 *   - vals :
 *     > Used to hold values of AllocaInst.
 *   - addrs :
 *     > Used to hold addresses of referenced values.
 *   - currentBB :
 *     > The currentBasicBlock for this function.
 *******************************************************************************/
class FunctionBlock {
    private :
        TypeVec           args;
        TypeTable         vars;
        ValTable          vals;
        ValTable          addrs;
        llvm::BasicBlock *currentBB;
    public :
        FunctionBlock();
        ~FunctionBlock();

        void addArg(std::string name, sem::TypePtr type, sem::PassMode mode);
        void addVar(std::string name, sem::TypePtr type);
        void addVal(std::string name, llvm::AllocaInst *val);
        void addAddr(std::string name, llvm::AllocaInst *addr);
        void setCurrentBlock(llvm::BasicBlock *BB);

        TypeVec& getArgs();
        llvm::AllocaInst* getVal(std::string name);
        llvm::AllocaInst* getAddr(std::string name);
        bool isRef(std::string name);
        llvm::BasicBlock* getCurrentBlock();
};

extern llvm::Type* translateType(sem::TypePtr type, sem::PassMode mode = sem::PassMode::VALUE);

#endif
