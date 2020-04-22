%{

#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror( const char * s );

%}

%%


%%

int main()
{
    yyin = stdin;
    
    do
    {
        yyparse();
    } while( !feof( yyin ) );
    return 0;
}

void yyerror( const char * s )
{
    fprintf( stderr, "parse error: %s\n", s );
    exit( 1 );
}
