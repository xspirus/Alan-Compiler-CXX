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

#include <general/general.hpp>
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

extern char * filename;

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;

static GenStack genBlocks;
static FuncMap functions;

/*******************************************************************************
 * Alan types for easier use.
 * Also easy to get pointers by calling `type->getPointerTo()`
 *******************************************************************************/
static llvm::Type *i32 = llvm::Type::getInt32Ty(TheContext);
static llvm::Type *i8 = llvm::Type::getInt8Ty(TheContext);
static llvm::Type *proc = llvm::Type::getVoidTy(TheContext);

static inline llvm::Constant* c32(int n) {
    return llvm::ConstantInt::get(i32, n);
}

static inline llvm::Constant* c8(unsigned char b) {
    return llvm::ConstantInt::get(i8, b);
}

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
    root->codegen();
    auto alanMain = std::dynamic_pointer_cast<ast::Func>(root);
    auto *alanMainFunc = functions[alanMain->id];
    std::vector<llvm::Value*> alanArgs;
    Builder.SetInsertPoint(mainBB);
    Builder.CreateCall(alanMainFunc, alanArgs);
    Builder.CreateRet(llvm::ConstantInt::get(i32, 0));
    TheModule->print(llvm::outs(), nullptr);
}

/*******************************************************************************
 ***************************** AST Codegen Methods *****************************
 *******************************************************************************/

llvm::Value* Int::codegen() {
    return c32(this->val);
}

llvm::Value* Byte::codegen() {
    return c8(this->b);
}

llvm::Value* String::codegen() {
    return Builder.CreateGlobalStringPtr(this->s);
}

llvm::Value* Var::codegen() {
    /* Normal Variable First */
    if ( this->index == nullptr ) {
        if ( genBlocks.front()->isRef(this->id) ) {
            auto *addr = Builder.CreateLoad(genBlocks.front()->getAddr(this->id));
            return Builder.CreateLoad(addr);
        } else {
            return Builder.CreateLoad(genBlocks.front()->getVal(this->id));
        }
    }
    /* Array Variable */
    else {
        auto *idx = this->index->codegen();
        if ( genBlocks.front()->isRef(this->id) ) {
            auto *ptr = Builder.CreateLoad(genBlocks.front()->getAddr(this->id));
            auto *addr = Builder.CreateGEP(ptr, idx);
            return Builder.CreateLoad(addr);
        } else {
            return Builder.CreateLoad(Builder.CreateGEP(genBlocks.front()->getVal(this->id),
                                                        std::vector<llvm::Value*>{ c32(0), idx } ));
        }
    }
    /* Fail */
    return nullptr;
}

llvm::Value* BinOp::codegen() {
    auto *lhs = this->left->codegen();
    auto *rhs = this->right->codegen();
    switch ( this->op ) {
        case '+' :
            return Builder.CreateAdd(lhs, rhs, "addtmp");
        case '-' :
            return Builder.CreateSub(lhs, rhs, "subtmp");
        case '*' :
            return Builder.CreateMul(lhs, rhs, "multmp");
        case '/' :
            return Builder.CreateSDiv(lhs, rhs, "divtmp");
        case '%' :
            return Builder.CreateSRem(lhs, rhs, "modtmp");
        default :
            return nullptr;
    }
    return nullptr;
}

llvm::Value* Condition::codegen() {
    llvm::Value *lhs, *rhs;
    lhs = nullptr;
    rhs = nullptr;
    if ( this->left != nullptr )
        lhs = Builder.CreateZExt(this->left->codegen(), i32);
    if ( this->right != nullptr )
        rhs = Builder.CreateZExt(this->right->codegen(), i32);
    switch ( this->op ) {
        case ast::Cond::TRU :
            return llvm::ConstantInt::get(i32, 1);
        case ast::Cond::FALS :
            return llvm::ConstantInt::get(i32, 0);
        case ast::Cond::EQ :
            return Builder.CreateICmpEQ(lhs, rhs, "eqtmp");
        case ast::Cond::NEQ :
            return Builder.CreateICmpNE(lhs, rhs, "neqtmp");
        case ast::Cond::LT :
            return Builder.CreateICmpSLT(lhs, rhs, "lttmp");
        case ast::Cond::LE :
            return Builder.CreateICmpSLE(lhs, rhs, "letmp");
        case ast::Cond::GT :
            return Builder.CreateICmpSGT(lhs, rhs, "gttmp");
        case ast::Cond::GE :
            return Builder.CreateICmpSGE(lhs, rhs, "getmp");
        case ast::Cond::AND :
            return Builder.CreateAnd(lhs, rhs, "andtmp");
        case ast::Cond::OR :
            return Builder.CreateOr(lhs, rhs, "ortmp");
        case ast::Cond::NOT :
            return Builder.CreateNot(rhs, "nottmp");
    }
    return nullptr;
}

llvm::Value* IfElse::codegen() {
    llvm::Function *TheFunction = genBlocks.front()->getFunc();

    /* condition */
    auto *CondV = this->cond->codegen();
    if ( !CondV->getType()->isIntegerTy(32) ) {
        CondV = Builder.CreateZExt(CondV, i32);
    }
    CondV = Builder.CreateICmpEQ(CondV, c32(1));
    
    llvm::BasicBlock *ThenBB = 
        llvm::BasicBlock::Create( TheContext,
                                  "then",
                                  TheFunction );
    llvm::BasicBlock *ElseBB =
        llvm::BasicBlock::Create( TheContext,
                                  "else" );
    llvm::BasicBlock *MergeBB =
        llvm::BasicBlock::Create( TheContext,
                                  "merge" );
    Builder.CreateCondBr(CondV, ThenBB, ElseBB);
    
    /* if block */
    Builder.SetInsertPoint(ThenBB);
    genBlocks.front()->setCurrentBlock(ThenBB);
    this->ifBody->codegen();
    if ( !genBlocks.front()->hasReturn() )
        Builder.CreateBr(MergeBB);
    
    /* else block */
    TheFunction->getBasicBlockList().push_back(ElseBB);
    Builder.SetInsertPoint(ElseBB);
    genBlocks.front()->setCurrentBlock(ElseBB);
    if ( this->elseBody != nullptr )
        this->elseBody->codegen();
    if ( !genBlocks.front()->hasReturn() )
        Builder.CreateBr(MergeBB);

    /* merge body */
    TheFunction->getBasicBlockList().push_back(MergeBB);
    Builder.SetInsertPoint(MergeBB);
    genBlocks.front()->setCurrentBlock(MergeBB);

    return nullptr;
}

llvm::Value* While::codegen() {
    llvm::Function *TheFunction = genBlocks.front()->getFunc();

    /* condition */
    auto *CondV = this->cond->codegen();
    if ( !CondV->getType()->isIntegerTy(32) ) {
        CondV = Builder.CreateZExt(CondV, i32);
    }
    CondV = Builder.CreateICmpEQ(CondV, c32(1));

    llvm::BasicBlock *LoopBB =
        llvm::BasicBlock::Create( TheContext,
                                  "loop",
                                  TheFunction );
    llvm::BasicBlock *AfterBB =
        llvm::BasicBlock::Create( TheContext,
                                  "after" );

    Builder.CreateCondBr(CondV, LoopBB, AfterBB);

    /* loop body */
    Builder.SetInsertPoint(LoopBB);
    genBlocks.front()->setCurrentBlock(LoopBB);
    this->body->codegen();
    auto *nextCond = this->cond->codegen();
    if ( !nextCond->getType()->isIntegerTy(32) ) {
        nextCond = Builder.CreateZExt(nextCond, i32);
    }
    nextCond = Builder.CreateICmpEQ(nextCond, c32(1));
    Builder.CreateCondBr(nextCond, LoopBB, AfterBB);

    /* after body */
    TheFunction->getBasicBlockList().push_back(AfterBB);
    Builder.SetInsertPoint(AfterBB);
    genBlocks.front()->setCurrentBlock(AfterBB);

    return nullptr;
}

llvm::Value* Call::codegen() {
    llvm::Function *TheFunction = functions[this->id];
    for ( auto &param : this->hidden ) {
        this->params.push_back(param);
    }
    std::vector<llvm::Value*> callArgs;

    int index = 0;
    for ( auto &Arg : TheFunction->args() ) {
        /* If argument by reference */
        if ( Arg.getType()->isPointerTy() ) {
            auto var = std::dynamic_pointer_cast<ast::Var>(this->params[index]);
            /* Found variable */
            if ( var ) {
                if ( var->index == nullptr ) {
                    if ( genBlocks.front()->isRef(var->id) ) {
                        auto par = Builder.CreateLoad(genBlocks.front()->getAddr(var->id));
                        callArgs.push_back(par);
                    } else {
                        llvm::Value *par;
                        if ( genBlocks.front()->getVar(var->id)->isArrayTy() )
                            par = Builder.CreateGEP( genBlocks.front()->getVal(var->id),
                                                     std::vector<llvm::Value*>{ c32(0), c32(0) } );
                        else
                            par = genBlocks.front()->getVal(var->id);
                        callArgs.push_back(par);
                    }
                } else {
                    auto idx = var->index->codegen();
                    if ( genBlocks.front()->isRef(var->id) ) {
                        llvm::Value *par = Builder.CreateLoad(genBlocks.front()->getAddr(var->id));
                        par = Builder.CreateGEP(par, idx);
                        callArgs.push_back(par);
                    } else {
                        llvm::Value *par = genBlocks.front()->getVal(var->id);
                        par = Builder.CreateGEP( par,
                                                 std::vector<llvm::Value*>{ c32(0), idx } );
                        callArgs.push_back(par);
                    }
                }
                index++;
                continue;
            }
            auto strlit = std::dynamic_pointer_cast<ast::String>(this->params[index]);
            /* Found string literal */
            if ( strlit ) {
                callArgs.push_back(strlit->codegen());
                index++;
                continue;
            }
            linecount = this->line;
            error("Expected variable or string literal");
        } else {
            auto par = this->params[index];
            callArgs.push_back(par->codegen());
            index++;
        }
    }
    return Builder.CreateCall(TheFunction, callArgs);
}

llvm::Value* Ret::codegen() {
    genBlocks.front()->addRet();
    return Builder.CreateRet(this->expr->codegen());
}

llvm::Value* Assign::codegen() {
    auto lval = std::dynamic_pointer_cast<ast::Var>(left);
    auto *rval = this->right->codegen();
    /* Normal Variable */
    if ( lval->index == nullptr ) {
        if ( genBlocks.front()->isRef(lval->id) ) {
            auto *addr = Builder.CreateLoad(genBlocks.front()->getAddr(lval->id));
            return Builder.CreateStore(rval, addr);
        } else {
            return Builder.CreateStore(rval, genBlocks.front()->getVal(lval->id));
        }
    }
    /* Array Variable */
    else {
        auto *idx = lval->index->codegen();
        llvm::Value *val;
        if ( genBlocks.front()->isRef(lval->id) ) {
            val = Builder.CreateLoad(genBlocks.front()->getAddr(lval->id));
            val = Builder.CreateGEP(val, idx);
        }
        else {
            val = Builder.CreateGEP(genBlocks.front()->getVal(lval->id),
                                    std::vector<llvm::Value*>{ c32(0), idx } );
        }
        return Builder.CreateStore(rval, val);
    }
    /* Fail */
    return nullptr;
}

llvm::Value* VarDecl::codegen() {
    auto *type = translateType(this->type);
    auto *alloca = Builder.CreateAlloca(type, nullptr, this->id);
    genBlocks.front()->addVar(this->id, this->type);
    genBlocks.front()->addVal(this->id, alloca);
    return nullptr;
}

llvm::Value* Param::codegen() {
    genBlocks.front()->addArg(this->id, this->type, this->mode);
    genBlocks.front()->addVar(this->id, this->type, this->mode);
    return nullptr;
}

llvm::Value* Func::codegen() {
    GenPtr newBlock = newShared<GenBlock>();
    genBlocks.push_front(newBlock);
    for ( auto par : this->params )
        par->codegen();
    for ( auto hid : this->hidden ) {
        auto hidpar = std::dynamic_pointer_cast<ast::Param>(hid);
        hidpar->codegen();
    }
    llvm::FunctionType *ftype =
        llvm::FunctionType::get( translateType(this->type),
                                 genBlocks.front()->getArgs(),
                                 false );
    llvm::Function *func =
        llvm::Function::Create( ftype,
                                llvm::Function::ExternalLinkage,
                                this->id,
                                TheModule.get() );
    genBlocks.front()->setFunc(func);
    functions[this->id] = func;

    int index = 0;
    int hindex = 0;
    for ( auto &Arg : func->args() ) {
        if ( index == this->params.size() ) {
            auto h = std::dynamic_pointer_cast<ast::Param>(this->hidden[hindex++]);
            Arg.setName(h->id);
        } else {
            auto p = std::dynamic_pointer_cast<ast::Param>(this->params[index++]);
            Arg.setName(p->id);
        }
    }

    llvm::BasicBlock *FuncBB =
        llvm::BasicBlock::Create( TheContext,
                                  "entry",
                                  func );
    Builder.SetInsertPoint(FuncBB);
    genBlocks.front()->setCurrentBlock(FuncBB);
    for ( auto &Arg : func->args() ) {
        auto *alloca = Builder.CreateAlloca( Arg.getType(),
                                             nullptr,
                                             Arg.getName() );
        if ( Arg.getType()->isPointerTy() )
            genBlocks.front()->addAddr(Arg.getName(), alloca);
        else
            genBlocks.front()->addVal(Arg.getName(), alloca);
        Builder.CreateStore(&Arg, alloca);
    }
    for ( auto &decl : this->decls )
        decl->codegen();
    this->body->codegen();

    if ( func->getReturnType()->isVoidTy() )
        Builder.CreateRetVoid();
    else {
        if ( !genBlocks.front()->hasReturn() ) {
            if ( func->getReturnType()->isIntegerTy(32) )
                Builder.CreateRet(c32(0));
            else
                Builder.CreateRet(c8(0));
        }
    }

    genBlocks.pop_front();

    if ( !main )
        Builder.SetInsertPoint(genBlocks.front()->getCurrentBlock());

    return nullptr;
}

llvm::Value* Block::codegen() {
    for ( auto &stmt : this->stmts )
        stmt->codegen();
    return nullptr;
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

llvm::Type* translateType(sem::TypePtr type, sem::PassMode mode) {
    llvm::Type *ret;
    switch( type->t ) {
        case sem::genType::INT :
            ret = i32;
            break;
        case sem::genType::BYTE :
            ret = i8;
            break;
        case sem::genType::VOID :
            ret = proc;
            break;
        case sem::genType::ARRAY :
            ret = llvm::ArrayType::get( translateType(type->getRef()), 
                                        type->getSize() );
            break;
        case sem::genType::IARRAY :
            ret = translateType(type->getRef());
            break;
    }
    if ( mode == sem::PassMode::REFERENCE )
        ret = ret->getPointerTo();
    return ret;
}
