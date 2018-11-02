/*******************************************************************************
 *                                                                             *
 *  Filename    : llvmIR.cpp                                                   *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Code generation source file                                  *
 *                                                                             *
 *******************************************************************************/

#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>
#include <symbol/types.hpp>
#include <symbol/entry.hpp>
#include <string>

static llvm::LLVMContext TheContext;

llvm::Type* translateType(sem::TypePtr type, sem::PassMode = sem::PassMode::VALUE) {
    return llvm::Type::getVoidTy(TheContext);
}
