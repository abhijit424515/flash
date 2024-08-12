%{
	#include "file.cc"
    extern "C" void yyerror(const char *s);
    extern int yylex(void);
%}

%union{
	string *name;
	Get* g;
	Set *s;
}

%token NEWLINE SEC MSEC USEC UMSEC EX PX EXAT PXAT NX XX KEEPTTL GET SET NAME

%type <name> NAME
%type <g> get
%type <s> set
%start program
%%

program
	:	req_list						{}
	|	%empty							{}
;

req_list
	:	req req_list					{}
	|	req								{}
;

req
	:	get								{ process<Get>($1); }
	|	set								{ process<Set>($1); }
	|	NEWLINE							{}
;

get
	:	GET NAME NEWLINE				{ $$ = new Get(*($2)); }
;

set
	:	SET NAME NAME nx_xx opt_get exp keepttl NEWLINE		{ $$ = new Set(*($2),*($3)); }
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