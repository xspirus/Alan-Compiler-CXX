/*******************************************************************************
 *                                                                             *
 *  Filename    : codegen.cpp                                                  *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Codegen source file (llvm stuff)                             *
 *                                                                             *
 *******************************************************************************/

#include <codegen/codegen.hpp>
#include <memory>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/BasicBlock.h>

FunctionBlock::FunctionBlock() {
    this->currentBB = nullptr;
}

FunctionBlock::~FunctionBlock() {
}

void FunctionBlock::addVar(std::string name, llvm::AllocaInst *val) {
    this->vals[name] = val;
}

void FunctionBlock::setCurrentBlock(llvm::BasicBlock *BB) {
    this->currentBB = BB;
}

llvm::AllocaInst* FunctionBlock::getVar(std::string name) {
    return this->vals[name];
}

llvm::BasicBlock* FunctionBlock::getCurrentBlock() {
    return this->currentBB;
}
