%code requires {
    #include "ast/ast.h"
    #include "logging.h"
}

%{
#include <iostream>
#include <memory>
#include "ast/ast.h"

extern int yylex();
void yyerror(const char *s);
ASTNode* root;
%}

%union {
    double fval;
    char* sval;
    ASTNode* node;
}

%token SAVE IN CREATE DONE NEWLINE SHOW ARROW
%token <fval> NUMBER
%token <sval> WORD
%left '+' '-'
%left '*' '/'

%type <node> input line expression assignment procedureBody procedure procedureCall showCall showArguments

%%

input
  : /* empty */                 { $$ = new ProgramNode(); root = $$; }
  | input line                  {
                                  if ($2) static_cast<ProgramNode*>($1)->append($2);
                                  $$ = $1;
                                }
  ;

line
  : expression                  { $$ = $1; }
  | assignment                  { $$ = $1; }
  | procedure                   { $$ = $1; }
  | procedureCall               { $$ = $1; }
  | NEWLINE                     { $$ = nullptr; }
  ;

expression
  : NUMBER                      { $$ = new NumberNode($1); }
  | expression '+' expression    { $$ = new BinaryOpNode('+', $1, $3); }
  | expression '-' expression    { $$ = new BinaryOpNode('-', $1, $3); }
  | expression '*' expression    { $$ = new BinaryOpNode('*', $1, $3); }
  | expression '/' expression    { $$ = new BinaryOpNode('/', $1, $3); }
  | '(' expression ')'           { $$ = $2; }
  ;

assignment
  : SAVE expression IN WORD      { $$ = new AssignmentNode($4, $2); }
  ;

procedureBody
  :                             { $$ = new ProcedureBodyNode(); }
  | procedureBody line          {
                                  if ($1) static_cast<ProcedureBodyNode*>($1)->append($2);
                                  $$ = $1;
                                }
  ;

procedure
  : CREATE WORD NEWLINE procedureBody DONE       { $$ = new ProcedureNode($2, $4); }
  ;

procedureCall
  : WORD                        { $$ = new ProcedureCallNode($1); }
  ;

showCall
  : SHOW showArguments
  ;

showArguments
  : ARROW WORD showArguments
  | WORD showArguments
  ;

%%

void yyerror(const char *s) {
    logsys::get()->error("Error: {}", s);
}
