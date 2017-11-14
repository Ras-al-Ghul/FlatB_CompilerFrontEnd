%option yylineno
%{
	#include "parser.tab.h"
	char linebuf[50];
%}

%%
\n.*					{
							strncpy(linebuf, yytext+1, sizeof(linebuf));
							yyless(1);
						}

"declblock"				{ return(DECLBLOCK); }
"codeblock"				{ return(CODEBLOCK); }

"else"					{ return(ELSE); }
"for"					{ return(FOR); }
"goto"					{ return(GOTO); }
"if"					{ return(IF); }
"int"					{ return(INT); }
"while"					{ return(WHILE); }
"println"				{ return(PRINTLN); }
"print"					{ return(PRINT); }
"read"					{ return(READ); }

[0-9][0-9]*				{
							yylval.number = atoi(yytext);
							printf("Token type: Number, Lexeme/Token Value: %s\n", yytext);
							return NUMBER;
						}
[a-zA-Z][a-zA-Z0-9]*	{
							yylval.string = strdup(yytext);
							printf("Token type: Identifier, Lexeme/Token Value: %s\n", yytext);
							return IDENTIFIER;
						}

\".*\"					{
							yylval.string = strdup(yytext);
							return(STRING_LITERAL);
						}

"<="					{ return LE; }
">="					{ return GE; }
"=="					{ return EQ; }
"!="					{ return NE; }
";"						{ return ';'; }
"{"						{ return '{'; }
"}"						{ return '}'; }
","						{ return ','; }
":"						{ return ':'; }
"="						{ return '='; }
"["						{ return '['; }
"]"						{ return ']'; }
"("						{ return '('; }
")"						{ return ')'; }
"!"						{ return '!'; }
"-"						{ return '-'; }
"+"						{ return '+'; }
"*"						{ return '*'; }
"/"						{ return '/'; }
"<"						{ return '<'; }
">"						{ return '>'; }

[ \t]+					{ /* Do nothing */ }

.						{ printf("Unexpected token encountered: %s\n", yytext); return ETOK; }
