%{
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <vector>

#include <symbol/types.hpp>
#include <ast/ast.hpp>

template<typename T, typename ... Args>
inline std::shared_ptr<T> SHARED(Args ... args) {
   return std::make_shared<T>(args...);
}

void yyerror (const char *msg);
extern int yylex();

ast::astPtr * t;
extern int    linecount;

std::vector<ast::astPtr*> nodes;
std::vector<ast::astVec*> vecs;
std::vector<sem::TypePtr*> types;

ast::astPtr doParse();
%}

%union {
    ast::astPtr   * a;
    int             n;
    unsigned char   c;
    char          * s;
    sem::TypePtr       * t;
    ast::astVec   * v;
}

%token T_int   "int"
%token T_byte  "byte"
%token T_proc  "proc"
%token T_ref   "reference"
%token T_ret   "return"
%token T_if    "if"
%token T_else  "else"
%token T_while "while"
%token T_true  "true"
%token T_false "false"
%token T_eq    "=="
%token T_neq   "!="
%token T_le    "<="
%token T_ge    ">="

%token<s> T_id
%token<n> T_const
%token<c> T_char
%token<s> T_string

%left '&'
%left '|'
%left '!'

%nonassoc '<' '>' "==" "!=" "<=" ">="

%left '+' '-'
%left '*' '/' '%'

%left UPLUS
%left UMINUS

%nonassoc NOELSE
%nonassoc "else"

%type<a> program
%type<a> func_def
%type<v> fpar_list
%type<a> fpar_def
%type<v> local_def_list
%type<a> local_def
%type<a> var_def
%type<a> compound_stmt
%type<v> stmt_list
%type<a> stmt
%type<a> func_call
%type<v> expr_list
%type<a> expr
%type<a> l_value
%type<a> cond
%type<t> r_type
%type<t> type
%type<t> data_type

%start program

%%

data_type
    : "int"  { $$ = &sem::typeInteger; }
    | "byte" { $$ = &sem::typeByte;    }
    ;

type
    : data_type '[' ']' { $$ = new sem::TypePtr(SHARED<sem::TypeIArray>(*$1)); types.push_back($$); }
    | data_type         { $$ = $1;                                                                  }
    ;

r_type
    : data_type { $$ = $1;             }
    | "proc"    { $$ = &sem::typeVoid; }
    ;

cond
    : "true"         { $$ = new ast::astPtr(SHARED<ast::Byte>(1)); nodes.push_back($$);                                 }
    | "false"        { $$ = new ast::astPtr(SHARED<ast::Byte>(0)); nodes.push_back($$);                                 }
    | '(' cond ')'   { $$ = $2;                                                                                         }
    | '!' cond       { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::NOT, nullptr, *$2)); nodes.push_back($$); }
    | expr '<' expr  { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::LT, *$1, *$3)); nodes.push_back($$);      }
    | expr '>' expr  { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::GT, *$1, *$3)); nodes.push_back($$);      }
    | expr "==" expr { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::EQ, *$1, *$3)); nodes.push_back($$);      }
    | expr "!=" expr { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::NEQ, *$1, *$3)); nodes.push_back($$);     }
    | expr "<=" expr { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::LE, *$1, *$3)); nodes.push_back($$);      }
    | expr ">=" expr { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::GE, *$1, *$3)); nodes.push_back($$);      }
    | cond '&' cond  { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::AND, *$1, *$3)); nodes.push_back($$);     }
    | cond '|' cond  { $$ = new ast::astPtr(SHARED<ast::Condition>(ast::Cond::OR, *$1, *$3)); nodes.push_back($$);      }
    ;

l_value
    : T_id '[' expr ']' { $$ = new ast::astPtr(SHARED<ast::Var>(std::string($1), *$3)); nodes.push_back($$);     }
    | T_string          { $$ = new ast::astPtr(SHARED<ast::String>(std::string($1))); nodes.push_back($$);       }
    | T_id              { $$ = new ast::astPtr(SHARED<ast::Var>(std::string($1), nullptr)); nodes.push_back($$); }
    ;

expr
    : T_const               { $$ = new ast::astPtr(SHARED<ast::Int>($1)); nodes.push_back($$);                              }
    | T_char                { $$ = new ast::astPtr(SHARED<ast::Byte>($1)); nodes.push_back($$);                             }
    | l_value               { $$ = $1;                                                                                      }
    | '(' expr ')'          { $$ = $2;                                                                                      }
    | func_call             { $$ = $1;                                                                                      }
    | expr '+' expr         { $$ = new ast::astPtr(SHARED<ast::BinOp>('+', *$1, *$3)); nodes.push_back($$);                 }
    | expr '-' expr         { $$ = new ast::astPtr(SHARED<ast::BinOp>('-', *$1, *$3)); nodes.push_back($$);                 }
    | expr '*' expr         { $$ = new ast::astPtr(SHARED<ast::BinOp>('*', *$1, *$3)); nodes.push_back($$);                 }
    | expr '/' expr         { $$ = new ast::astPtr(SHARED<ast::BinOp>('/', *$1, *$3)); nodes.push_back($$);                 }
    | expr '%' expr         { $$ = new ast::astPtr(SHARED<ast::BinOp>('%', *$1, *$3)); nodes.push_back($$);                 }
    | '+' expr %prec UPLUS  { $$ = new ast::astPtr(SHARED<ast::BinOp>('+', SHARED<ast::Int>(0), *$2)); nodes.push_back($$); }
    | '-' expr %prec UMINUS { $$ = new ast::astPtr(SHARED<ast::BinOp>('-', SHARED<ast::Int>(0), *$2)); nodes.push_back($$); }
    ;

expr_list
    : /* nothing */      { $$ = new ast::astVec(); vecs.push_back($$);                     }
    | expr_list ',' expr { $$ = $1; $$->push_back(*$3);                                    }
    | expr               { $$ = new ast::astVec(); $$->push_back(*$1); vecs.push_back($$); }
    ;

func_call
    : T_id '(' expr_list ')' { $$ = new ast::astPtr(SHARED<ast::Call>(std::string($1), *$3)); nodes.push_back($$); }
    ;

stmt
    : ';'                                 { $$ = nullptr;                                                                      }
    | l_value '=' expr ';'                { $$ = new ast::astPtr(SHARED<ast::Assign>(*$1, *$3)); nodes.push_back($$);          }
    | compound_stmt                       { $$ = $1;                                                                           }
    | func_call ';'                       { $$ = $1;                                                                           }
    | "if" '(' cond ')' stmt %prec NOELSE { $$ = new ast::astPtr(SHARED<ast::IfElse>(*$3, *$5, nullptr)); nodes.push_back($$); }
    | "if" '(' cond ')' stmt "else" stmt  { $$ = new ast::astPtr(SHARED<ast::IfElse>(*$3, *$5, *$7)); nodes.push_back($$);     }
    | "while" '(' cond ')' stmt           { $$ = new ast::astPtr(SHARED<ast::While>(*$3, *$5)); nodes.push_back($$);           }
    | "return" expr ';'                   { $$ = new ast::astPtr(SHARED<ast::Ret>(*$2)); nodes.push_back($$);                  }
    ;

stmt_list
    : /* nothing */  { $$ = new ast::astVec(); vecs.push_back($$); }
    | stmt_list stmt { $$ = $1; $$->push_back(*$2);                }
    ;

compound_stmt
    : '{' stmt_list '}' { $$ = new ast::astPtr(SHARED<ast::Block>(*$2)); nodes.push_back($$); }
    ;

var_def
    : T_id ':' data_type '[' T_const ']' ';' { $$ = new ast::astPtr(SHARED<ast::VarDecl>(std::string($1), std::make_shared<sem::TypeArray>($5, *$3))); nodes.push_back($$); }
    | T_id ':' data_type ';'                 { $$ = new ast::astPtr(SHARED<ast::VarDecl>(std::string($1), *$3)); nodes.push_back($$);                                       }
    ;

local_def
    : func_def { $$ = $1; }
    | var_def  { $$ = $1; }
    ;

local_def_list
    : /* nothing */            { $$ = new ast::astVec(); vecs.push_back($$); }
    | local_def_list local_def { $$ = $1; $$->push_back(*$2);                }
    ;

fpar_def
    : T_id ':' "reference" type { $$ = new ast::astPtr(SHARED<ast::Param>(std::string($1), sem::PassMode::REFERENCE, *$4)); }
    | T_id ':' type             { $$ = new ast::astPtr(SHARED<ast::Param>(std::string($1), sem::PassMode::VALUE, *$3));     }
    ;

fpar_list
    : /* nothing */          { $$ = new ast::astVec(); vecs.push_back($$);                     }
    | fpar_list ',' fpar_def { $$ = $1; $$->push_back(*$3);                                    }
    | fpar_def               { $$ = new ast::astVec(); $$->push_back(*$1); vecs.push_back($$); }
    ;

func_def
    : T_id '(' fpar_list ')' ':' r_type local_def_list compound_stmt { $$ = new ast::astPtr(SHARED<ast::Func>(std::string($1), *$3, *$6, *$7, *$8)); nodes.push_back($$); }
    ;

program
    : func_def { t = $$ = $1; }
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Alan error: %s\n", msg);
    fprintf(stderr, "Aborting!\nYou made a stupid mistake in line %d\n", linecount);
}

void clearNodes() {
    int i;
    for ( i = 0; i < nodes.size(); i++ )
        delete nodes[i];
    for ( i = 0; i < types.size(); i++ )
        delete types[i];
    for ( i = 0; i < vecs.size(); i++ )
        delete vecs[i];
}

ast::astPtr parse() {
    if ( yyparse() )
        return nullptr;
    std::cout << "Compilation Successful\n";
    ast::astPtr toRet = *t;
    clearNodes();
    return toRet;
}
