%{
#include <stdlib.h>
#include <string.h>
#include <string>

#include <ast/ast.hpp>
#include <parser/parser.hpp>
#include <fix/fix.hpp>

#define T_eof 0

int linecount      = 1;
int nested_comment = 0;
void yyerror(const char *msg);
%}

L   [a-zA-Z_]
D   [0-9]
W   [ \t\r]
H   [a-fA-F0-9]
ESC [ntr0\'\"\\]

%x COMMENT

%%

 /* Keywords */
"int"       { return T_int;   }
"byte"      { return T_byte;  }
"proc"      { return T_proc;  }
"reference" { return T_ref;   }
"return"    { return T_ret;   }
"if"        { return T_if;    }
"else"      { return T_else;  }
"while"     { return T_while; }
"true"      { return T_true;  }
"false"     { return T_false; }

 /* Symbols */
[()\[\]{}=+\-*/\%!&\|<>,:;] { return yytext[0]; }
"=="                        { return T_eq;      }
"!="                        { return T_neq;     }
"<="                        { return T_le;      }
">="                        { return T_ge;      }

 /* Constant. Names. Chars. Strings. */
{D}+                        { yylval.n = atoi(yytext)                      ; return T_const;  }
{L}({L}|{D})*               { yylval.s = strdup(yytext)                    ; return T_id;     }
\'({L}|\\({ESC}|x{H}{H}))\' { int n = 0; yylval.c = fixChar(yytext + 1, n) ; return T_char;   }
\"(\\.|[^\\"])*\"           { yylval.s = fixString(yytext + 1)             ; return T_string; }

 /* Comments */
\-\-.*\n             { ++linecount;                                               }
"(*"                 { BEGIN(COMMENT);                                            }
<COMMENT>"(*"        { ++nested_comment;                                          }
<COMMENT>"*)"        { if (nested_comment) --nested_comment; else BEGIN(INITIAL); }
<COMMENT>\n          { ++linecount;                                               }
<COMMENT>"*"         { /* nothing */                                              }
<COMMENT>[^{(*}*\n]+ { /* nothing */                                              }

 /* WhiteSpace */
\n   { ++linecount;            }
{W}+ { /* ignore whitespace */ }

. { yyerror("Illegal Character"); }

%%
