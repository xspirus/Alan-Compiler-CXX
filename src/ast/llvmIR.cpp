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

/*******************************************************************************
 ******************************** LLVM includes ********************************
 *******************************************************************************/

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Verifier.h>

/*******************************************************************************
 ****************************** Project Includes *******************************
 *******************************************************************************/

#include <symbol/types.hpp>
#include <symbol/entry.hpp>
#include <ast/ast.hpp>
#include <codegen/codegen.hpp>

/*******************************************************************************
 ****************************** General Includes *******************************
 *******************************************************************************/

#include <string>

/*******************************************************************************
 **************************** Global File Variables ****************************
 *******************************************************************************/

extern const char * filename;

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;

static FuncStack genBlocks;
static FuncMap functions;

static llvm::Type *i32 = llvm::Type::getInt32Ty(TheContext);
static llvm::Type *i8 = llvm::Type::getInt8Ty(TheContext);
static llvm::Type *proc = llvm::Type::getVoidTy(TheContext);

/*******************************************************************************
 **************************** Function Declarations ****************************
 *******************************************************************************/

static void codegenLibs();

namespace ast {

/*******************************************************************************
 **************************** Main CodeGen Function ****************************
 *******************************************************************************/

void codegen(astPtr root) {
    TheModule = llvm::make_unique<llvm::Module>(filename, TheContext);
    codegenLibs();
    auto *mainType =
        llvm::FunctionType::get( i32,
                                 std::vector<llvm::Type*>{  },
                                 false );
    auto *mainFunc =
        llvm::Function::Create( mainType,
                                llvm::Function::ExternalLinkage,
                                "main",
                                TheModule.get() );
    llvm::BasicBlock *mainBB = 
        llvm::BasicBlock::Create( TheContext,
                                  "entry",
                                  mainFunc );
    Builder.SetInsertPoint(mainBB);
    Builder.CreateRet(llvm::ConstantInt::get(i32, 0));
    TheModule->print(llvm::outs(), nullptr);
}

}

/**
 * Standard library functions :
 *   - void writeInteger(int n)
 *   - void writeByte(byte b)
 *   - void writeChar(byte b)
 *   - void writeString(reference byte s)
 *   - int readInteger()
 *   - byte readByte()
 *   - byte readChar()
 *   - void readString(int n, reference byte s)
 *   - int extend(byte b)
 *   - byte shrink(int i)
 *   - int strlen(reference byte s)
 *   - int strcmp(reference byte s1, reference byte s2)
 *   - void strcpy(reference byte trg, reference byte src)
 *   - void strcat(reference byte trg, reference byte src)
 */
void codegenLibs() {
    auto *writeIntegerType = 
        llvm::FunctionType::get( proc,
                                 std::vector<llvm::Type*>{ i32 },
                                 false );
    functions["writeInteger"] = 
        llvm::Function::Create( writeIntegerType,
                                llvm::Function::ExternalLinkage,
                                "writeInteger",
                                TheModule.get() );
    auto *writeByteType = 
        llvm::FunctionType::get( proc,
                                 std::vector<llvm::Type*>{ i8 },
                                 false );
    functions["writeByte"] = 
        llvm::Function::Create( writeByteType,
                                llvm::Function::ExternalLinkage,
                                "writeByte",
                                TheModule.get() );
    auto *writeCharType = 
        llvm::FunctionType::get( proc,
                                 std::vector<llvm::Type*>{ i8 },
                                 false );
    functions["writeChar"] = 
        llvm::Function::Create( writeCharType,
                                llvm::Function::ExternalLinkage,
                                "writeChar",
                                TheModule.get() );
    auto *writeStringType = 
        llvm::FunctionType::get( proc,
                                 std::vector<llvm::Type*>{ i8->getPointerTo() },
                                 false );
    functions["writeString"] = 
        llvm::Function::Create( writeStringType,
                                llvm::Function::ExternalLinkage,
                                "writeString",
                                TheModule.get() );
    auto *readIntegerType = 
        llvm::FunctionType::get( i32,
                                 std::vector<llvm::Type*>{  },
                                 false );
    functions["readInteger"] = 
        llvm::Function::Create( readIntegerType,
                                llvm::Function::ExternalLinkage,
                                "readInteger",
                                TheModule.get() );
    auto *readByteType = 
        llvm::FunctionType::get( i8,
                                 std::vector<llvm::Type*>{  },
                                 false );
    functions["readByte"] = 
        llvm::Function::Create( readByteType,
                                llvm::Function::ExternalLinkage,
                                "readByte",
                                TheModule.get() );
    auto *readCharType = 
        llvm::FunctionType::get( i8,
                                 std::vector<llvm::Type*>{  },
                                 false );
    functions["readChar"] = 
        llvm::Function::Create( readCharType,
                                llvm::Function::ExternalLinkage,
                                "readChar",
                                TheModule.get() );
    auto *readStringType =
        llvm::FunctionType::get( proc,
                                 std::vector<llvm::Type*>{ i32, i8->getPointerTo() },
                                 false );
    functions["readString"] = 
        llvm::Function::Create( readStringType,
                                llvm::Function::ExternalLinkage,
                                "readString",
                                TheModule.get() );
    auto *extendType =
        llvm::FunctionType::get( i32,
                                 std::vector<llvm::Type*>{ i8 },
                                 false );
    functions["extend"] = 
        llvm::Function::Create( extendType,
                                llvm::Function::ExternalLinkage,
                                "extend",
                                TheModule.get() );
    auto *shrinkType =
        llvm::FunctionType::get( i8,
                                 std::vector<llvm::Type*>{ i32 },
                                 false );
    functions["shrink"] = 
        llvm::Function::Create( shrinkType,
                                llvm::Function::ExternalLinkage,
                                "shrink",
                                TheModule.get() );
    auto *strlenType =
        llvm::FunctionType::get( i32,
                                 std::vector<llvm::Type*>{ i8->getPointerTo() },
                                 false );
    functions["strlen"] = 
        llvm::Function::Create( strlenType,
                                llvm::Function::ExternalLinkage,
                                "strlen",
                                TheModule.get() );
    auto *strcmpType =
        llvm::FunctionType::get( i32,
                                 std::vector<llvm::Type*>{ i8->getPointerTo(), i8->getPointerTo() },
                                 false );
    functions["strcmp"] = 
        llvm::Function::Create( strcmpType,
                                llvm::Function::ExternalLinkage,
                                "strcmp",
                                TheModule.get() );
    auto *strcpyType =
        llvm::FunctionType::get( proc,
                                 std::vector<llvm::Type*>{ i8->getPointerTo(), i8->getPointerTo() },
                                 false );
    functions["strcpy"] = 
        llvm::Function::Create( strcpyType,
                                llvm::Function::ExternalLinkage,
                                "strcpy",
                                TheModule.get() );
    auto *strcatType =
        llvm::FunctionType::get( proc,
                                 std::vector<llvm::Type*>{ i8->getPointerTo(), i8->getPointerTo() },
                                 false );
    functions["strcat"] = 
        llvm::Function::Create( strcatType,
                                llvm::Function::ExternalLinkage,
                                "strcat",
                                TheModule.get() );
}

llvm::Type* translateType(sem::TypePtr type, sem::PassMode) {
    return llvm::Type::getVoidTy(TheContext);
}
