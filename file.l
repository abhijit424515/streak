%{
	#include "file.hh"
	#include "parse.tab.h"
	using namespace std;
%}

%option noyywrap 

digit						[0-9]
fraction					([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)
letter						[a-zA-Z\._]
ws							[ \t\n]*

%%

\n							{ return NEWLINE; }

SEC							{ return SEC; }
MSEC						{ return MSEC; }
USEC						{ return USEC; }
UMSEC						{ return UMSEC; }

EX							{ return EX; }
PX							{ return PX; }
EXAT						{ return EXAT; }
PXAT						{ return PXAT; }

NX							{ return NX; }
XX							{ return XX; }
KEEPTTL						{ return KEEPTTL; }

GET							{ return GET; }
SET							{ return SET; }

({letter}|{digit})+ 		{ yylval.name = new string(yytext); return NAME; }

{ws}       					;
.         					{ fprintf(stderr, "unknown_token: %d\n", yytext[0]); exit(1); }
