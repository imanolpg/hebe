%{
#include "parser.hpp"
#include <cstdlib>
%}

%%

[ \t\r]+                    { /* Ignore whitespace */}

"save"                      { return SAVE; }
"in"                        { return IN; }
"create"                    { return CREATE; }
"done"                      { return DONE; }

(\-)?[0-9]+\.[0-9]+         { yylval.fval = atof(yytext); return NUMBER; }

"+"|"-"|"*"|"/"             { return yytext[0]; }

[0-9a-zA-Z_\-\>]+           { yylval.sval = strdup(yytext); return WORD; }

"\n"                        { return NEWLINE; }

.                           { return yytext[0]; }

%%

int yywrap() {
    return 1;
}