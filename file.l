%{
	#include "file.hh"
	#include "parse.tab.h"
	using namespace std;
%}

%option noyywrap 

digit						[0-9]
fraction					([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)
ws							[ \t]*

unquoted					[a-zA-Z0-9_\-.:/@+=]+
quoted						\"[^\"\\\r\n]*(\\.[^\"\\\r\n]*)*\"

clear						[Cc][Ll][Ee][Aa][Rr]
get							[Gg][Ee][Tt]
set							[Ss][Ee][Tt]
del							[Dd][Ee][Ll]
mget						[Mm][Gg][Ee][Tt]
mset						[Mm][Ss][Ee][Tt]

incr						[Ii][Nn][Cc][Rr]
incrby						[Ii][Nn][Cc][Rr][Bb][Yy]
decr						[Dd][Ee][Cc][Rr]
decrby						[Dd][Ee][Cc][Rr][Bb][Yy]

lpush						[Ll][Pp][Uu][Ss][Hh]
rpush						[Rr][Pp][Uu][Ss][Hh]
lpop						[Ll][Pp][Oo][Pp]
rpop						[Rr][Pp][Oo][Pp]
llen						[Ll][Ll][Ee][Nn]
lmove						[Ll][Mm][Oo][Vv][Ee]
lrange						[Ll][Rr][Aa][Nn][Gg][Ee]
ltrim						[Ll][Tt][Rr][Ii][Mm]

sadd						[Ss][Aa][Dd][Dd]
srem						[Ss][Rr][Ee][Mm]
sismember					[Ss][Ii][Ss][Mm][Ee][Mm][Bb][Ee][Rr]
sinter						[Ss][Ii][Nn][Tt][Ee][Rr]
scard						[Ss][Cc][Aa][Rr][Dd]

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

LEFT						{ return LEFT; }
RIGHT						{ return RIGHT; }

{clear}						{ return CLEAR; }
{get}						{ return GET; }
{set}						{ return SET; }
{del}						{ return DEL; }
{mget}						{ return MGET; }
{mset}						{ return MSET; }

{incr}						{ return INCR; }
{incrby}					{ return INCRBY; }
{decr}						{ return DECR; }
{decrby}					{ return DECRBY; }

{lpush}						{ return LPUSH; }
{rpush}						{ return RPUSH; }
{lpop}						{ return LPOP; }
{rpop}						{ return RPOP; }
{llen}						{ return LLEN; }
{lmove}						{ return LMOVE; }
{lrange}					{ return LRANGE; }
{ltrim}						{ return LTRIM; }

{sadd}						{ return SADD; }
{srem}						{ return SREM; }
{sismember}					{ return SISMEMBER; }
{sinter}					{ return SINTER; }
{scard}						{ return SCARD; }

({unquoted}|{quoted})		{ yylval.name = new string(yytext); return NAME; }

{ws}       					;
.         					{ fprintf(stderr, "unknown_token: %d\n", yytext[0]); exit(1); }
