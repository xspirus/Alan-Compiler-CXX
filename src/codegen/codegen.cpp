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

void FunctionBlock::addArg(std::string name, sem::TypePtr type, sem::PassMode mode) {
    auto *t = translateType(type, mode);
    args.push_back(t);
    vars[name] = t;
}

void FunctionBlock::addVar(std::string name, sem::TypePtr type) {
    vars[name] = translateType(type);
}

void FunctionBlock::addVal(std::string name, llvm::AllocaInst *val) {
    this->vals[name] = val;
}

void FunctionBlock::addAddr(std::string name, llvm::AllocaInst *addr) {
    this->addrs[name] = addr;
}

void FunctionBlock::setCurrentBlock(llvm::BasicBlock *BB) {
    this->currentBB = BB;
}

TypeVec& FunctionBlock::getArgs() {
    return this->args;
}

llvm::AllocaInst* FunctionBlock::getVal(std::string name) {
    return this->vals[name];
}

llvm::AllocaInst* FunctionBlock::getAddr(std::string name) {
    return this->addrs[name];
}

bool FunctionBlock::isRef(std::string name) {
    return this->vars[name]->isPointerTy();
}

llvm::BasicBlock* FunctionBlock::getCurrentBlock() {
    return this->currentBB;
}
