%{
	#include "file.cc"
    extern "C" void yyerror(const char *s) {
		fprintf(stderr, "(syntax error)\n");
	}
    extern int yylex(void);
%}

%union{
	string *name;
	Cmd *cmd;
	KeyWrapper *kw;
	KV *kvp;
	KVWrapper *kvw;
}

%token NAME NEWLINE SEC MSEC USEC UMSEC EX PX EXAT PXAT NX XX KEEPTTL GET SET MGET MSET INCR INCRBY DECR DECRBY SADD SREM SISMEMBER SINTER SCARD

%type <name> NAME
%type <kw> key_list
%type <kvp> kv
%type <kvw> kv_list
%type <cmd> get set mget mset incr incrby decr decrby sadd srem sismember sinter scard
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
	:	error NEWLINE       			{ yyerrok; }
	|	get								{ process($1); }
	|	set								{ process($1); }
	|	mget							{ process($1); }
	|	mset							{ process($1); }
	|	incr							{ process($1); }
	|	incrby							{ process($1); }
	|	decr							{ process($1); }
	|	decrby							{ process($1); }
	// |	lpush							{ process<LPush>($1); }
	// |	lpop							{ process<LPop>($1); }
	// |	llen							{ process<LLen>($1); }
	// |	lmove							{ process<LMove>($1); }
	// |	lrange							{ process<LRange>($1); }
	// |	ltrim							{ process<LTrim>($1); }
	|	sadd							{ process($1); }
	|	srem							{ process($1); }
	|	sismember						{ process($1); }
	|	sinter							{ process($1); }
	|	scard							{ process($1); }
	|	NEWLINE							{}
;

// --------------------------------

get
	:	GET NAME NEWLINE				{ $$ = new Get($2); }
;

set
	:	SET NAME NAME nx_xx opt_get exp keepttl NEWLINE		{ $$ = new Set($2,$3); }
;

mget
	:	MGET key_list NEWLINE			{ $$ = new Mget($2); }
;

mset
	:	MSET kv_list NEWLINE			{ $$ = new Mset($2); }
;

// --------------------------------

incr
	:	INCR NAME NEWLINE				{ $$ = new ModifyInt($2, new string("1")); }
;

incrby
	:	INCRBY NAME NAME NEWLINE		{ $$ = new ModifyInt($2,$3); }
;

decr
	:	DECR NAME NEWLINE				{ $$ = new ModifyInt($2, new string("-1")); }
;

decrby
	:	DECRBY NAME NAME NEWLINE		{ $$ = new ModifyInt($2,$3,1); }
;

// --------------------------------

// lpush
// 	:	LPUSH NAME NAME NEWLINE			{ $$ = new LPush($2,$3); }
// ;

// lpop
// 	:	LPOP NAME NEWLINE				{ $$ = new LPop($2); }
// ;

// llen
// 	:	LLEN NAME NEWLINE				{ $$ = new LLen($2); }
// ;

// lmove
// 	:	LMOVE NAME NAME NAME NEWLINE	{ $$ = new LMove($2,$3,$4); }
// ;

// lrange
// 	:	LRANGE NAME NAME NAME NEWLINE	{ $$ = new LRange($2,$3,$4); }
// ;

// ltrim
// 	:	LTRIM NAME NAME NAME NEWLINE	{ $$ = new LTrim($2,$3,$4); }
// ;

// --------------------------------

sadd
	:	SADD NAME key_list NEWLINE			{ $$ = new SAdd($2,$3); }
;

srem
	:	SREM NAME key_list NEWLINE			{ $$ = new SRem($2,$3); }
;

sismember
	:	SISMEMBER NAME NAME NEWLINE		{ $$ = new SIsMember($2,$3); }
;

sinter
	:	SINTER key_list NEWLINE			{ $$ = new SInter($2); }
;

scard
	:	SCARD NAME NEWLINE				{ $$ = new SCard($2); }
;

// --------------------------------

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