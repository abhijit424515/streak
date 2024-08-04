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
	// yyparse();

	mqd_t mq;
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    if (mq == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

	while (1) {
		string s;
		getline(cin, s);
		Entry e(time(NULL),s);

		if (mq_send(mq, (char*)(void*)&e, sizeof(e), 0) == -1) {
			perror("mq_send");
			exit(EXIT_FAILURE);
		}
	}

    mq_close(mq);
}