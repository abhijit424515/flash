%{
	#include "file.cc"
    extern "C" void yyerror(const char *s) {
		fprintf(stderr, "(syntax error)\n");
	}
    extern int yylex(void);
	extern void yyrestart(FILE *input_file);
%}

%union{
	string *name;
	Cmd *cmd;
	KeyWrapper *kw;
	KV *kvp;
	KVWrapper *kvw;
	bool b;
}

%token NAME NEWLINE SEC MSEC USEC UMSEC EX PX EXAT PXAT NX XX KEEPTTL LEFT RIGHT CLEAR GET SET DEL MGET MSET INCR INCRBY DECR DECRBY LPUSH RPUSH LPOP RPOP LLEN LMOVE LRANGE LTRIM SADD SREM SISMEMBER SINTER SCARD

%type <b> dir
%type <name> NAME
%type <kw> key_list
%type <kvp> kv
%type <kvw> kv_list
%type <cmd> command get set del mget mset incr incrby decr decrby lpush rpush lpop rpop llen lmove lrange ltrim sadd srem sismember sinter scard
%start program
%%

program
	:	req_list						{}
;

req_list
	:	req_list NEWLINE req			{}
	|	req								{}
;

req
	:	error			       			{ yyerrok; }
	|	CLEAR							{ clear(); }
	|	%empty							{}
	|	command 						{ process($1); }
;

command
	:	get								{}
	|	set								{}
	|	del								{}
	|	mget							{}
	|	mset							{}
	|	incr							{}
	|	incrby							{}
	|	decr							{}
	|	decrby							{}
	|	lpush							{}
	|	rpush							{}
	|	lpop							{}
	|	rpop							{}
	|	llen							{}
	|	lmove							{}
	|	lrange							{}
	|	ltrim							{}
	|	sadd							{}
	|	srem							{}
	|	sismember						{}
	|	sinter							{}
	|	scard							{}
;

// --------------------------------

get
	:	GET NAME						{ $$ = new Get($2); }
;

set
	:	SET NAME NAME nx_xx opt_get exp keepttl		{ $$ = new Set($2,$3); }
;

del
	:	DEL key_list					{ $$ = new Del($2); }
;

mget
	:	MGET key_list					{ $$ = new Mget($2); }
;

mset
	:	MSET kv_list					{ $$ = new Mset($2); }
;

// --------------------------------

incr
	:	INCR NAME						{ $$ = new ModifyInt($2, new string("1")); }
;

incrby
	:	INCRBY NAME NAME				{ $$ = new ModifyInt($2,$3); }
;

decr
	:	DECR NAME						{ $$ = new ModifyInt($2, new string("-1")); }
;

decrby
	:	DECRBY NAME NAME				{ $$ = new ModifyInt($2,$3,1); }
;

// --------------------------------

lpush
	:	LPUSH NAME key_list				{ $$ = new LPush($2,$3); }
;

rpush
	:	RPUSH NAME key_list				{ $$ = new RPush($2,$3); }
;

lpop
	:	LPOP NAME NAME					{ $$ = new LPop($2,$3); }
	|	LPOP NAME						{ $$ = new LPop($2, new string("1")); }
;

rpop
	:	RPOP NAME NAME					{ $$ = new RPop($2,$3); }
	|	RPOP NAME						{ $$ = new RPop($2, new string("1")); }
;

llen
	:	LLEN NAME						{ $$ = new LLen($2); }
;

lmove
	:	LMOVE NAME NAME dir dir			{ $$ = new LMove($2,$3,$4,$5); }
;

lrange
	:	LRANGE NAME NAME NAME			{ $$ = new LRange($2,$3,$4); }
;

ltrim
	:	LTRIM NAME NAME NAME			{ $$ = new LTrim($2,$3,$4); }
;

// --------------------------------

sadd
	:	SADD NAME key_list 				{ $$ = new SAdd($2,$3); }
;

srem
	:	SREM NAME key_list				{ $$ = new SRem($2,$3); }
;

sismember
	:	SISMEMBER NAME NAME				{ $$ = new SIsMember($2,$3); }
;

sinter
	:	SINTER key_list					{ $$ = new SInter($2); }
;

scard
	:	SCARD NAME						{ $$ = new SCard($2); }
;

// --------------------------------

dir
	:	LEFT							{ $$ = 0; }
	|	RIGHT							{ $$ = 1; }
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
	std::thread init_thread(init);

	yyparse();
}