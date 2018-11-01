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

class FunctionBlock {
    private :
        ValTable vals;
        llvm::BasicBlock *currentBB;
    public :
        FunctionBlock();
        ~FunctionBlock();

        void addVar(std::string name, llvm::AllocaInst *val);
        void setCurrentBlock(llvm::BasicBlock *BB);

        llvm::AllocaInst* getVar(std::string name);
        llvm::BasicBlock* getCurrentBlock();
};

#endif
