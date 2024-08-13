%{
	#include "file.cc"
    extern "C" void yyerror(const char *s);
    extern int yylex(void);
%}

%union{
	string *name;
	Get* g;
	Set *s;
	Mget* mg;
	Mset *ms;
	KeyWrapper *kw;
	KV *kvp;
	KVWrapper *kvw;
}

%token NEWLINE SEC MSEC USEC UMSEC EX PX EXAT PXAT NX XX KEEPTTL GET SET MGET MSET NAME

%type <name> NAME
%type <kw> key_list
%type <kvp> kv
%type <kvw> kv_list
%type <g> get
%type <s> set
%type <mg> mget
%type <ms> mset
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
	|	mget							{ process<Mget>($1); }
	|	mset							{ process<Mset>($1); }
	|	NEWLINE							{}
;

get
	:	GET NAME NEWLINE				{ $$ = new Get($2); }
;

set
	:	SET NAME NAME nx_xx opt_get exp keepttl NEWLINE		{ $$ = new Set($2,$3); }
;

mget
	:	MGET key_list					{ $$ = new Mget($2); }
;

mset
	:	MSET kv_list					{ $$ = new Mset($2); }
;

key_list
	:	key_list NAME					{ $1->push_back($2); $$ = $1; }
	|	NAME							{ $$ = new KeyWrapper($1); }
;

kv_list
	:	kv_list kv						{ $1->push_back($2); $$ = $1; }
	|	kv								{ $$ = new KVWrapper($1); }
;

kv
	:	NAME NAME						{ $$ = new KV(new pair($1,$2)); }
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