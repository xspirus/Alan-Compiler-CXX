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

GenBlock::GenBlock() {
    this->func = nullptr;
    this->currentBB = nullptr;
}

GenBlock::~GenBlock() {
}

void GenBlock::setFunc(llvm::Function *func) {
    this->func = func;
}

void GenBlock::addArg(std::string name, sem::TypePtr type, sem::PassMode mode) {
    auto *t = translateType(type, mode);
    args.push_back(t);
    vars[name] = t;
}

void GenBlock::addVar(std::string name, sem::TypePtr type) {
    vars[name] = translateType(type);
}

void GenBlock::addVal(std::string name, llvm::AllocaInst *val) {
    this->vals[name] = val;
}

void GenBlock::addAddr(std::string name, llvm::AllocaInst *addr) {
    this->addrs[name] = addr;
}

void GenBlock::setCurrentBlock(llvm::BasicBlock *BB) {
    this->currentBB = BB;
}

const TypeVec& GenBlock::getArgs() const {
    return this->args;
}

llvm::AllocaInst* GenBlock::getVal(std::string name) {
    return this->vals[name];
}

llvm::AllocaInst* GenBlock::getAddr(std::string name) {
    return this->addrs[name];
}

bool GenBlock::isRef(std::string name) {
    return this->vars[name]->isPointerTy();
}

llvm::Function* GenBlock::getFunc() {
    return this->func;
}

llvm::BasicBlock* GenBlock::getCurrentBlock() {
    return this->currentBB;
}
