%{
	#include "file.cc"
    extern "C" void yyerror(const char *s);
    extern int yylex(void);
%}

%union{
	string *name;
}

%token NEWLINE SEC MSEC USEC UMSEC EX PX EXAT PXAT NX XX KEEPTTL GET SET NAME

%type <name> NAME
%start program
%%

program
	:	req_list						{}
	|	%empty							{}
;

req_list
	:	req NEWLINE req_list			{}
	|	req								{}
;

req
	:	get								{}
	|	set								{}
;

get
	:	GET NAME						{}
;

set
	:	SET NAME nx_xx opt_get exp keepttl						{}
;

nx_xx
	:	NX								{}
	|	XX								{}
	|	%empty							{}
;

opt_get
	:	GET								{}
	|	%empty							{}
;

exp
	:	EX SEC							{}
	|	PX MSEC							{}
	|	EXAT USEC						{}
	|	PXAT UMSEC						{}
	|	%empty							{}
;

keepttl
	:	KEEPTTL							{}
	| 	%empty							{}
;

%%

int main() {
	yyparse();
}