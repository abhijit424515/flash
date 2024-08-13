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

get							[Gg][Ee][Tt]
set							[Ss][Ee][Tt]
mget						[Mm][Gg][Ee][Tt]
mset						[Mm][Ss][Ee][Tt]

incr						[Ii][Nn][Cc][Rr]
incrby						[Ii][Nn][Cc][Rr][Bb][Yy]
decr						[Dd][Ee][Cc][Rr]
decrby						[Dd][Ee][Cc][Rr][Bb][Yy]

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

{get}						{ return GET; }
{set}						{ return SET; }
{mget}						{ return MGET; }
{mset}						{ return MSET; }

{incr}						{ return INCR; }
{incrby}					{ return INCRBY; }
{decr}						{ return DECR; }
{decrby}					{ return DECRBY; }

({unquoted}|{quoted})		{ yylval.name = new string(yytext); return NAME; }

{ws}       					;
.         					{ fprintf(stderr, "unknown_token: %d\n", yytext[0]); exit(1); }
