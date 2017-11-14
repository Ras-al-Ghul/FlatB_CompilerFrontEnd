%{
  	#include <stdio.h>
  	#include <stdlib.h>
  	extern "C" FILE *yyin;
  	int yylex (void);
  	void yyerror (char const *s);
  	extern int yylineno;
  	extern char* yytext;

  	#include "AST.h"
  	extern char linebuf[50];
  	int errorcount;

  	class ASTProgram *start = NULL;
  	extern map<string, DataStorage> reserved_space;
  	extern vector<ASTStatement *> statements_vector;
  	extern map<string, int> gotolabels;
  	extern int program_counter;
%}

%union
{
	int number;
	char *string;
	class ASTVar *var;
	class ASTVars *vars;
	class ASTProgram *program;
	class ASTDeclBlock *declblock;
	class ASTDecls *decls;
	class ASTDecl *decl;
	class ASTCodeBlock *codeblock;
	class ASTCodes *codes;
	class ASTPrimaryArithExp *primarithexp;
	class ASTArithExp *arithexp;
	class ASTBoolExp *boolexp;
	class ASTForLoop *forloop;
	class ASTWhileLoop *whileloop;
	class ASTAssgnStmt *assignmentstmnt;
	class ASTPrintStmnt *printstmnt;
	class ASTIfStmnt *ifstmnt;
	class ASTGenStmnt *genstmnt;
	class ASTLoopStmnt *loopstmnt;
	class ASTStatement *codestmnt;
}

%type <program> program

%type <var> declname

%type <vars> declcomma
%type <declblock> decl_block
%type <decls> decl_statements
%type <decl> declstmnt

%type <codeblock> code_block
%type <codes> code_statements
%type <codestmnt> codestmnt
%type <primarithexp> primary_arith_expression
%type <primarithexp> array_expr
%type <arithexp> arith_expr
%type <boolexp> bool_expr

%type <forloop> forloop
%type <whileloop> whileloop

%type <assignmentstmnt> assignmentstmnt
%type <assignmentstmnt> readstmnt

%type <printstmnt> printstmnt
%type <printstmnt> arglist

%type <ifstmnt> ifstmnt
%type <ifstmnt>	gotostmnt

%type <genstmnt> genstmnt
%type <loopstmnt> loopstmnt

%token DECLBLOCK CODEBLOCK
%token <string> IDENTIFIER STRING_LITERAL
%token <number> NUMBER 
%token LE GE EQ NE
%token INT
%token IF ELSE WHILE FOR GOTO
%token PRINTLN PRINT READ
%token ETOK
%left '+' '-'
%left '*' '/'

%%

program
	: decl_block code_block
	{
		$$ = new ASTProgram($1, $2);
		start = $$;
	}
	;

decl_block
	: DECLBLOCK '{' decl_statements '}'
	{
		$$ = new ASTDeclBlock($3);
	}
	;

code_block
	: CODEBLOCK '{' code_statements '}'
	{
		$$ = new ASTCodeBlock($3);
	}
	;

decl_statements
	: declstmnt decl_statements
	{
		$2->push_back($1);
		$$ = $2;
	}
	|
	{
		$$ = new ASTDecls();
	}
	;

declstmnt
	: INT declcomma ';'
	{
		$$ = new ASTDecl($2->get_var_list());
	}
	| error ';'
	{
		yyerrok;
	}
	;

declcomma
	: declname ',' declcomma
	{
		$3->push_back($1);
		$$ = $3;
	}
	| declname
	{
		$$ = new ASTVars();
		$$->push_back($1);
	}
	;

declname
	: IDENTIFIER
	{
		$$ = new ASTVar(0,$1);
		DataStorage temp($1,0);
		reserved_space.insert(pair<string,DataStorage>($1,temp));
	}
	| IDENTIFIER '[' NUMBER ']'
	{
		$$ = new ASTVar(1,$1,$3);
		DataStorage temp($1,$3);
		reserved_space.insert(pair<string,DataStorage>($1,temp));
	}
	;

primary_arith_expression
	: IDENTIFIER
	{
		$$ = new ASTPrimaryArithExp($1);
	}
	| NUMBER
	{
		$$ = new ASTPrimaryArithExp($1);
	}
	| '(' arith_expr ')'
	{
		$$ = new ASTPrimaryArithExp($2,1);
	}
	| array_expr
	{

	}
	| '-' arith_expr
	{
		$$ = new ASTPrimaryArithExp(1,$2);
	}
	;

array_expr
	: IDENTIFIER '[' arith_expr ']'
	{
		$$ = new ASTPrimaryArithExp(1,$3,$1);
	}
	;

code_statements
	: codestmnt code_statements
	{
		$2->push_back($1);
		$$ = $2;
	}
	|
	{
		$$ = new ASTCodes();
	}
	;

codestmnt
	: IDENTIFIER ':' genstmnt ';'
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($1);
		$$ = new ASTStatement($3, temp);
	}
	| genstmnt ';'
	{
		$$ = new ASTStatement($1, nullptr);	
	}
	| IDENTIFIER ':' loopstmnt
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($1);
		$$ = new ASTStatement($3, temp);
	}
	| loopstmnt
	{
		$$ = new ASTStatement($1, nullptr);
	}
	| IDENTIFIER ':' ifstmnt
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($1);
		$$ = new ASTStatement($3, temp);
	}
	| ifstmnt
	{
		$$ = new ASTStatement($1, nullptr);
	}
	| error ';'
	{
		yyerrok;
	}
	;

genstmnt
	: assignmentstmnt
	{
		$$ = new ASTGenStmnt($1);
	}
	| printstmnt
	{
		$$ = new ASTGenStmnt($1);
	}
	| readstmnt
	{
		$$ = new ASTGenStmnt($1);
	}
	| gotostmnt
	{
		$$ = new ASTGenStmnt($1);
	}
	;

ifstmnt
	: IF bool_expr '{' code_statements '}' ELSE '{' code_statements '}'
	{
		$$ = new ASTIfStmnt($2, $4, $8);
	}
	| IF bool_expr '{' code_statements '}'
	{
		$$ = new ASTIfStmnt($2, $4);
	}
	;

gotostmnt
	: GOTO IDENTIFIER IF bool_expr
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($2);
		$$ = new ASTIfStmnt(temp, $4);
	}
	| GOTO IDENTIFIER
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($2);
		$$ = new ASTIfStmnt(temp);
	}
	;

loopstmnt
	: whileloop
	{
		$$ = new ASTLoopStmnt($1);
	}
	| forloop
	{
		$$ = new ASTLoopStmnt($1);
	}
	;

whileloop
	: WHILE bool_expr '{' code_statements '}'
	{
		$$ = new ASTWhileLoop($2, $4);
	}
	;

forloop
	: FOR array_expr '=' arith_expr ',' arith_expr ',' arith_expr '{' code_statements '}'
	{
		$$ = new ASTForLoop($2, $4, $6, $8, $10, bool(1));
	}
	| FOR IDENTIFIER '=' arith_expr ',' arith_expr ',' arith_expr '{' code_statements '}'
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($2);
		$$ = new ASTForLoop(temp, $4, $6, $8, $10, bool(0));
	}
	| FOR array_expr '=' arith_expr ',' arith_expr '{' code_statements '}' 
	{
		$$ = new ASTForLoop($2, $4, $6, $8, bool(1));
	}
	| FOR IDENTIFIER '=' arith_expr ',' arith_expr '{' code_statements '}'
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($2);
		$$ = new ASTForLoop(temp, $4, $6, $8, bool(0));
	}
	;

readstmnt
	: READ IDENTIFIER
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($2);
		$$ = new ASTAssgnStmt(temp);
	}
	| READ IDENTIFIER '[' arith_expr ']'
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($2);
		$$ = new ASTAssgnStmt(temp, $4, true);
	}
	;

printstmnt
	: PRINTLN arglist
	{
		$2->set_is_println(true);
		$$ = $2;
	}
	| PRINT arglist
	{
		$2->set_is_println(false);
		$$ = $2;
	}
	;

arglist
	: STRING_LITERAL ',' arglist
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($1, true);
		$3->push_back(temp);
		$$ = $3;
	}
	| primary_arith_expression ',' arglist
	{
		$3->push_back($1);
		$$ = $3;
	}
	| STRING_LITERAL
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($1, true);
		$$ = new ASTPrintStmnt();
		$$->push_back(temp);
	}
	| primary_arith_expression
	{
		$$ = new ASTPrintStmnt();
		$$->push_back($1);
	}
	;

assignmentstmnt
	: IDENTIFIER '=' arith_expr
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($1);
		$$ = new ASTAssgnStmt(temp, $3, false);
	}
	| IDENTIFIER '[' arith_expr ']' '=' arith_expr
	{
		ASTPrimaryArithExp * temp = new ASTPrimaryArithExp($1);
		$$ = new ASTAssgnStmt(temp, $3, $6);
	}
	;

arith_expr
	: arith_expr '+' arith_expr
	{
		$$ = new ASTArithExp($1,$3,"+");
	}
	| arith_expr '-' arith_expr
	{
		$$ = new ASTArithExp($1,$3,"-");
	}
	| arith_expr '*' arith_expr
	{
		$$ = new ASTArithExp($1,$3,"*");
	}
	| arith_expr '/' arith_expr
	{
		$$ = new ASTArithExp($1,$3,"/");
	}
	| primary_arith_expression
	{
		$$ = new ASTArithExp($1);
	}
	;

bool_expr
	: arith_expr '<' arith_expr
	{
		$$ = new ASTBoolExp($1,$3,"<");
	}
	| arith_expr '>' arith_expr
	{
		$$ = new ASTBoolExp($1,$3,">");
	}
	| arith_expr LE arith_expr
	{
		$$ = new ASTBoolExp($1,$3,"<=");
	}
	| arith_expr GE arith_expr
	{
		$$ = new ASTBoolExp($1,$3,">=");
	}
	| arith_expr EQ arith_expr
	{
		$$ = new ASTBoolExp($1,$3,"==");
	}
	| arith_expr NE arith_expr
	{
		$$ = new ASTBoolExp($1,$3,"!=");
	}
	| '(' bool_expr ')'
	{
		$$ = new ASTBoolExp($2);
	}
	;

%%

void yyerror (char const *s)
{
		errorcount++;
		fprintf(stderr, "Line %d: \x1b[31m %s \x1b[0m at %s:\n \x1b[31m %s \x1b[0m \n", yylineno, s, yytext, linebuf);
}

int main(int argc, char *argv[])
{
	if (argc == 1 ) {
		fprintf(stderr, "Correct usage: bcc filename\n");
		exit(1);
	}

	if (argc > 2) {
		fprintf(stderr, "Passing more arguments than necessary.\n");
		fprintf(stderr, "Correct usage: bcc filename\n");
	}

	yyin = fopen(argv[1], "r");

	errorcount = 0;
	yyparse();

	if(errorcount == 0){
		fprintf(stdout, "Success\n");
		class ASTVisitor* ASTVisitorobj = new ASTVisitor(start);
		fprintf(stdout, "Pass Done\n");

		//cout<<statements_vector.size()<<endl;
		//for(int i=0; i<statements_vector.size(); i++){
		//	  cout<<statements_vector[i]->gen_stmnt<<endl;
		//}
		//for(auto it=gotolabels.begin(); it!=gotolabels.end(); ++it){
		//	cout<<it->first<<" "<<it->second<<endl;
		//}

		cout<<"Beginning Interpreter Execution"<<endl;
		program_counter = 0;
		class InterpreterVisitor* IVobj = new InterpreterVisitor();

		cout<<"Interpreter Execution done"<<endl;
		cout<<"Beginning IR generation"<<endl;

		CodeGenVisitor * visitor = new CodeGenVisitor(start);
		visitor->codeGen();

		cout<<endl<<"IR generation done"<<endl;
	}
	else{
		fprintf(stderr, "%d errors\n", errorcount);
		fprintf(stdout, "Syntax Error\n");
	}
}
