#include "AST.h"
#include <fstream>

#define TAB giveTabs()

using namespace std;

bool gotoFlag;

map<string, DataStorage> reserved_space;
vector<ASTStatement *> statements_vector;
map<string, int> gotolabels;
int program_counter;

int tab_count = 0;
int tab_width = 4;
ofstream out("XML_visitor.txt");

void giveTabs(){
	for(int i=0; i<tab_count; i++){
		for(int j=0; j<tab_width; j++){
			out<<" ";
		}
	}
}

// Constructors

DataStorage::DataStorage(string name, int size){
	this->name = name;
	this->size = size;
	if(size)
		this->location = new int[size];
	else
		this->location  = new int();
}

ASTVar::ASTVar(bool decl_type, string var_name, int length){
	this->decl_type = decl_type;
	this->var_name = var_name;
	this->length = length;
}
ASTVar::ASTVar(bool decl_type, string var_name){
	this->decl_type = decl_type;
	this->var_name = var_name;
}
ASTVars::ASTVars(){

}
ASTDeclBlock::ASTDeclBlock(class ASTDecls *decls){
	this->decls = decls;
}
ASTDecls::ASTDecls(){
	this->count = 0;
}
ASTDecl::ASTDecl(vector<class ASTVar *> var_list){
	this->var_list = var_list;
}
ASTIfStmnt::ASTIfStmnt(ASTPrimaryArithExp * ident){
	this->ident = ident; this->bool_exp = nullptr; this->is_goto = true;
}
ASTIfStmnt::ASTIfStmnt(ASTPrimaryArithExp * ident, ASTBoolExp * bool_exp){
	this->ident = ident; this->bool_exp = bool_exp; this->is_goto = true;
}
ASTIfStmnt::ASTIfStmnt(ASTBoolExp * bool_exp, ASTCodes * if_statements){
	this->bool_exp = bool_exp; this->if_statements = if_statements;
	this->else_statements = nullptr; this->is_goto = false;
}
ASTIfStmnt::ASTIfStmnt(ASTBoolExp * bool_exp, ASTCodes * if_statements, ASTCodes * else_statements){
	this->bool_exp = bool_exp; this->if_statements = if_statements;
	this->else_statements = else_statements; this->is_goto = false;
}
ASTPrintStmnt::ASTPrintStmnt(){
	this->count = 0;
}
ASTPrimaryArithExp::ASTPrimaryArithExp(string str_literal, bool is_literal){
	this->str_literal = str_literal; this->is_literal = is_literal;
}
ASTPrimaryArithExp::ASTPrimaryArithExp(int literal){
	this->literal = literal;
	this->is_literal = true;
	this->sign = false; this->is_brace = false; this->is_array = false; this->expr = nullptr;
	this->str_literal = "";
}
ASTPrimaryArithExp::ASTPrimaryArithExp(string var_name){
	this->var_name = var_name;
	this->is_literal = false;
	this->sign = false; this->is_brace = false; this->is_array = false; this->expr = nullptr;
	this->str_literal = "";
}
ASTPrimaryArithExp::ASTPrimaryArithExp(bool sign, ASTArithExp *expr){
	this->sign = sign;
	this->expr = expr;
	this->str_literal = "";
}
ASTPrimaryArithExp::ASTPrimaryArithExp(bool is_array, ASTArithExp *expr, string var_name){
	this->var_name = var_name;
	this->expr = expr;
	this->is_array = is_array;
	this->str_literal = "";
}
ASTPrimaryArithExp::ASTPrimaryArithExp(ASTArithExp *expr, bool is_brace){
	this->is_brace = is_brace;
	this->expr = expr;
	this->str_literal = "";
}
ASTArithExp::ASTArithExp(ASTArithExp * left, ASTArithExp * right, string operation){
	this->left = left; this->right = right; this->operation = operation;
	this->is_primary = false;
}
ASTArithExp::ASTArithExp(ASTPrimaryArithExp * expr){
	this->expr = expr;
	this->is_primary = true;
}
ASTBoolExp::ASTBoolExp(ASTArithExp * left, ASTArithExp * right, string compare){
	this->left = left; this->right = right; this->compare = compare;
	this->is_brace = false;
}
ASTBoolExp::ASTBoolExp(ASTBoolExp * expr){
	this->expr = expr;
	this->is_brace = true;
}
ASTForLoop::ASTForLoop(ASTPrimaryArithExp * exp, ASTArithExp * begin, ASTArithExp * end, ASTArithExp * step, ASTCodes * statements, bool is_array){
	this->exp = exp; this->begin = begin; this->end = end; this->step = step; this->statements = statements;
	this->is_array = is_array; this->is_step = true;
}
ASTForLoop::ASTForLoop(ASTPrimaryArithExp * exp, ASTArithExp * begin, ASTArithExp * end, ASTCodes * statements, bool is_array){
	this->exp = exp; this->begin = begin; this->end = end; this->statements = statements;
	this->is_array = is_array; this->is_step = false;
}
ASTWhileLoop::ASTWhileLoop(ASTBoolExp * bool_exp, ASTCodes * statements){
	this->bool_exp = bool_exp;
	this->statements = statements;
}
ASTAssgnStmt::ASTAssgnStmt(ASTPrimaryArithExp * ident, ASTArithExp * array_exp, ASTArithExp * exp){
	this->ident = ident; this->array_exp = array_exp; this->exp = exp; this->is_read = false;
}
ASTAssgnStmt::ASTAssgnStmt(ASTPrimaryArithExp * ident, ASTArithExp * exp, bool is_read){
	this->ident = ident; this->is_read = is_read;
	if(this->is_read){
		this->array_exp = exp; this->exp = nullptr;
	}
	else{
		this->array_exp = nullptr; this->exp = exp;
	}
}
ASTAssgnStmt::ASTAssgnStmt(ASTPrimaryArithExp * ident){
	this->ident = ident; this->is_read = true; this->array_exp = nullptr;
}
ASTGenStmnt::ASTGenStmnt(ASTAssgnStmt * assgn_read_stmnt){
	this->assgn_read_stmnt = assgn_read_stmnt;
	this->print_stmnt = nullptr;
	this->goto_stmnt = nullptr;
}
ASTGenStmnt::ASTGenStmnt(ASTPrintStmnt * print_stmnt){
	this->print_stmnt = print_stmnt;
	this->assgn_read_stmnt = nullptr;
	this->goto_stmnt = nullptr;
}
ASTGenStmnt::ASTGenStmnt(ASTIfStmnt * goto_stmnt){
	this->goto_stmnt = goto_stmnt;
	this->assgn_read_stmnt = nullptr;
	this->print_stmnt = nullptr;
}
ASTLoopStmnt::ASTLoopStmnt(ASTForLoop * for_loop){
	this->for_loop = for_loop;
	this->while_loop = nullptr;
}
ASTLoopStmnt::ASTLoopStmnt(ASTWhileLoop * while_loop){
	this->while_loop = while_loop;
	this->for_loop = nullptr;
}
ASTStatement::ASTStatement(ASTGenStmnt * gen_stmnt, ASTPrimaryArithExp * exp = nullptr){
	this->gen_stmnt = gen_stmnt; this->exp = exp;
	this->loop_stmnt = nullptr; this->if_stmnt = nullptr;
}
ASTStatement::ASTStatement(ASTLoopStmnt * loop_stmnt, ASTPrimaryArithExp * exp = nullptr){
	this->loop_stmnt = loop_stmnt; this->exp = exp;
	this->gen_stmnt = nullptr; this->if_stmnt = nullptr;
}
ASTStatement::ASTStatement(ASTIfStmnt * if_stmnt, ASTPrimaryArithExp * exp = nullptr){
	this->if_stmnt = if_stmnt; this->exp = exp;
	this->gen_stmnt = nullptr; this->loop_stmnt = nullptr;
}
ASTCodes::ASTCodes(){

}
ASTCodeBlock::ASTCodeBlock(class ASTCodes *codes){
	this->codes = codes;
}
ASTProgram::ASTProgram(class ASTDeclBlock *decl_block, class ASTCodeBlock *code_block){
	this->decl_block = decl_block;
	this->code_block = code_block;
}

ASTVisitor::ASTVisitor(class ASTProgram * start){
	this->start = start;
	this->visit(this->start);
}
InterpreterVisitor::InterpreterVisitor(){
	while(program_counter != statements_vector.size()){
		gotoFlag = false;
		ASTStatement * cur_stmnt = statements_vector[program_counter];
		this->visit(cur_stmnt);
		program_counter += 1;
	}
}
// Other functions

void ASTVars::push_back(class ASTVar * var){
	this->var_list.push_back(var);
}
void ASTVars::push_back(class ASTVars * vars){
	for(int i=0; i<vars->var_list.size(); ++i)
		this->var_list.push_back(vars->var_list[i]);
}
vector <class ASTVar *> ASTVars::get_var_list(){
	return this->var_list;
}
void ASTDecls::push_back(class ASTDecl * decl){
	this->decl_list.push_back(decl);
	this->count++;
}
void ASTPrintStmnt::push_back(class ASTPrimaryArithExp * exp){
	this->arglist.push_back(exp);
	this->count++;
}
void ASTPrintStmnt::set_is_println(bool is_println){
	this->is_println = is_println;
}
vector <class ASTDecl *> ASTDecls::get_decl_list(){
	return this->decl_list;
}
vector <class ASTVar *> ASTDecl::get_var_list(){
	return this->var_list;
}
void ASTCodes::push_back(class ASTStatement * statement){
	this->statements.push_back(statement);
}

// ASTAccepts

void ASTProgram::accept(Visitor * obj){
	obj->visit(this);
}
void ASTDeclBlock::accept(Visitor * obj){
	obj->visit(this);	
}
void ASTDecls::accept(Visitor * obj){
	obj->visit(this);
}
void ASTDecl::accept(Visitor * obj){
	obj->visit(this);
}
void ASTVar::accept(Visitor * obj){
	obj->visit(this);	
}
void ASTIfStmnt::accept(Visitor * obj){
	obj->visit(this);	
}
void ASTPrintStmnt::accept(Visitor * obj){
	obj->visit(this);	
}
int ASTPrimaryArithExp::accept(Visitor * obj){
	return obj->visit(this);
}
int ASTArithExp::accept(Visitor * obj){
	return obj->visit(this);
}
bool ASTBoolExp::accept(Visitor * obj){
	return obj->visit(this);
}
void ASTForLoop::accept(Visitor * obj){
	obj->visit(this);
}
void ASTWhileLoop::accept(Visitor * obj){
	obj->visit(this);
}
void ASTAssgnStmt::accept(Visitor * obj){
	obj->visit(this);
}
void ASTGenStmnt::accept(Visitor * obj){
	obj->visit(this);
}
void ASTLoopStmnt::accept(Visitor * obj){
	obj->visit(this);
}
void ASTStatement::accept(Visitor * obj){
	obj->visit(this);
}
void ASTCodeBlock::accept(Visitor * obj){
	obj->visit(this);
}
void ASTCodes::accept(Visitor * obj){
	obj->visit(this);
}

// Interpreter Accepts
int* ASTPrimaryArithExp::lhs_accept(Visitor * obj){
	return obj->lhs_visit(this);
}
string ASTPrimaryArithExp::string_accept(Visitor * obj){
	return obj->string_visit(this);
}

// ASTVisitors

void ASTVisitor::visit(class ASTProgram * Node){
	TAB;
	out<<"<program>"<<endl;
	tab_count++;
	Node->decl_block->accept(this);
	Node->code_block->accept(this);
	tab_count--;
	TAB;
	out<<"</program>"<<endl;
}
void ASTVisitor::visit(class ASTDeclBlock * Node){
	TAB;
	out<<"<declblock>"<<endl;
	Node->decls->accept(this);
	TAB;
	out<<"</declblock>"<<endl;
}
void ASTVisitor::visit(class ASTDecls * Node){
	TAB;
	tab_count++;
	out<<"statement count = \""<<Node->count<<"\""<<endl;
	for(int i=Node->decl_list.size()-1; i>=0; --i)
		Node->decl_list[i]->accept(this);
	tab_count--;
}
void ASTVisitor::visit(class ASTDecl * Node){
	for(int i=Node->var_list.size()-1; i>=0; --i)
		Node->var_list[i]->accept(this);	
}
void ASTVisitor::visit(class ASTVar * Node){
	TAB;
	if(Node->decl_type){
		out<<"<declaration name=\""<<Node->var_name<<"\" count=\""<<Node->length<<"\" type=\"INT\"/>"<<endl;
	}
	else{
		out<<"<declaration name=\""<<Node->var_name<<"\" type=\"INT\"/>"<<endl;
	}
}
void ASTVisitor::visit(class ASTCodeBlock * Node){
	TAB;
	out<<"<codeblock>"<<endl;
	tab_count++;
	Node->codes->accept(this);
	tab_count--;
	TAB;
	out<<"</codeblock>"<<endl;
}
void ASTVisitor::visit(class ASTCodes * Node){
	for(int i=Node->statements.size()-1; i>=0; i--){
		Node->statements[i]->accept(this);
	}
}
void ASTVisitor::visit(class ASTIfStmnt * Node){
	if(Node->is_goto){
		TAB;
		if(Node->bool_exp){
			out<<"<conditional goto>"<<endl;
			tab_count++;
			Node->bool_exp->accept(this);
			tab_count--;
		}
		else{
			out<<"<unconditional goto>"<<endl;
		}
		TAB;
		out<<"<label>"<<endl;
		tab_count++;
		Node->ident->accept(this);
		tab_count--;
		TAB;
		out<<"</label>"<<endl;
		TAB;
		out<<"</goto>"<<endl;
	}
	else{
		TAB;
		out<<"<if condition>"<<endl;
		tab_count++;
		Node->bool_exp->accept(this);
		tab_count--;
		TAB;
		out<<"</condition>"<<endl;
		TAB;
		out<<"<if statement>"<<endl;
		tab_count++;
		Node->if_statements->accept(this);
		tab_count--;
		TAB;
		out<<"</if>"<<endl;
		if(Node->else_statements){
			TAB;
			out<<"<else statement>"<<endl;
			tab_count++;
			Node->else_statements->accept(this);
			tab_count--;
			TAB;
			out<<"</else>"<<endl;
		}
	}
}
void ASTVisitor::visit(class ASTPrintStmnt * Node){
	TAB;
	if(Node->is_println)
		out<<"<println statement with \""<<Node->count<<"\" args>"<<endl;
	else
		out<<"<print statement with \""<<Node->count<<"\" args>"<<endl;
	tab_count++;
	for(int i=Node->arglist.size()-1; i>=0; i--){
		Node->arglist[i]->accept(this);
	}
	tab_count--;
	TAB;
	out<<"</end print>"<<endl;
}
int ASTVisitor::visit(class ASTPrimaryArithExp * Node){
	TAB;
	if(Node->is_literal){
		if(Node->str_literal == "")
			out<<"<Literal int value=\""<<Node->literal<<"\"/>"<<endl;
		else
			out<<"<Literal string value=\""<<Node->str_literal<<"\"/>"<<endl;
	}
	else if(Node->sign){
		out<<"<negative expression>"<<endl;
		tab_count++;
		Node->expr->accept(this);
		tab_count--;
		TAB;
		out<<"</negative expression>"<<endl;
	}
	else if(Node->is_brace){
		out<<"<braced expression>"<<endl;
		tab_count++;
		Node->expr->accept(this);
		tab_count--;
		TAB;
		out<<"</braced expression>"<<endl;
	}
	else if(Node->is_array){
		out<<"<array expression Identifier=\""<<Node->var_name<<"\">"<<endl;
		tab_count++;
		Node->expr->accept(this);
		tab_count--;
		TAB;
		out<<"</array expression>"<<endl;
	}
	else{
		out<<"<Identifier name=\""<<Node->var_name<<"\"/>"<<endl;
	}
}
int ASTVisitor::visit(class ASTArithExp * Node){
	if(Node->is_primary){
		Node->expr->accept(this);
	}
	else{
		TAB;
		out<<"<arith expression operator=\""<<Node->operation<<"\">"<<endl;
		tab_count++;
		Node->left->accept(this);
		Node->right->accept(this);
		tab_count--;
		TAB;
		out<<"</arith expression>"<<endl;
	}
}
bool ASTVisitor::visit(class ASTBoolExp * Node){
	TAB;
	if(Node->is_brace){
		out<<"<braced bool expression>"<<endl;
		Node->expr->accept(this);
		TAB;
		out<<"</braced bool expression>"<<endl;
	}
	else{
		out<<"<bool expression> compare=\""<<Node->compare<<"\">"<<endl;
		tab_count++;
		Node->left->accept(this);
		Node->right->accept(this);
		tab_count--;
		TAB;
		out<<"</bool expression>"<<endl;
	}
}
void ASTVisitor::visit(class ASTForLoop * Node){
	TAB;
	out<<"<for loop>"<<endl;
	TAB;
	out<<"<counter variable>"<<endl;
	tab_count++;
	Node->exp->accept(this);
	tab_count--;
	TAB;
	out<<"</counter variable>"<<endl;
	TAB;
	out<<"<begin>"<<endl;
	tab_count++;
	Node->begin->accept(this);
	tab_count--;
	TAB;
	out<<"</begin>"<<endl;
	TAB;
	out<<"<end>"<<endl;
	tab_count++;
	Node->end->accept(this);
	tab_count--;
	TAB;
	out<<"</end>"<<endl;
	TAB;
	out<<"<step>"<<endl;
	tab_count++;
	if(Node->is_step)
		Node->step->accept(this);
	else{
		TAB;
		out<<"<Literal value=\"1\"/>"<<endl;
	}
	tab_count--;
	TAB;
	out<<"</step>"<<endl;
	TAB;
	out<<"<statements>"<<endl;
	tab_count++;
	Node->statements->accept(this);
	tab_count--;
	TAB;
	out<<"</statements>"<<endl;
	TAB;
	out<<"</for loop>"<<endl;
}
void ASTVisitor::visit(class ASTWhileLoop * Node){
	TAB;
	out<<"<while loop>"<<endl;
	TAB;
	out<<"<test expression>"<<endl;
	tab_count++;
	Node->bool_exp->accept(this);
	tab_count--;
	TAB;
	out<<"<test expression>"<<endl;
	TAB;
	out<<"<statements>"<<endl;
	tab_count++;
	Node->statements->accept(this);
	tab_count--;
	TAB;
	out<<"</statements>"<<endl;
	TAB;
	out<<"</while loop>"<<endl;
}
void ASTVisitor::visit(class ASTAssgnStmt * Node){
	TAB;
	if(Node->is_read){
		out<<"<read stmnt>"<<endl;
		tab_count++;
		if(Node->array_exp){
			TAB;
			out<<"<array name>"<<endl;
			Node->ident->accept(this);
			TAB;
			out<<"</array name>"<<endl;
			TAB;
			out<<"<array expr>"<<endl;
			Node->array_exp->accept(this);
			TAB;
			out<<"</array expr>"<<endl;
		}
		else{
			TAB;
			out<<"<var name>"<<endl;
			Node->ident->accept(this);
			TAB;
			out<<"</var name>"<<endl;
		}
		tab_count--;
		TAB;
		out<<"</read stmnt>"<<endl;
	}
	else{
		out<<"<assignment stmt>"<<endl;
		TAB;
		out<<"<LHS>"<<endl;
		tab_count++;
		if(Node->array_exp){
			TAB;
			out<<"<array name>"<<endl;
			Node->ident->accept(this);
			TAB;
			out<<"</array name>"<<endl;
			TAB;
			out<<"<array expr>"<<endl;
			Node->array_exp->accept(this);
			TAB;
			out<<"</array expr>"<<endl;
		}
		else
			Node->ident->accept(this);
		tab_count--;
		TAB;
		out<<"</LHS>"<<endl;
		TAB;
		out<<"<RHS>"<<endl;
		tab_count++;
		Node->exp->accept(this);
		tab_count--;
		TAB;
		out<<"</RHS>"<<endl;
		TAB;
		out<<"</assignment stmt>"<<endl;
	}
}
void ASTVisitor::visit(class ASTGenStmnt * Node){
	if(Node->assgn_read_stmnt)
		Node->assgn_read_stmnt->accept(this);
	else if(Node->print_stmnt)
		Node->print_stmnt->accept(this);
	else
		Node->goto_stmnt->accept(this);
}
void ASTVisitor::visit(class ASTLoopStmnt * Node){
	if(Node->for_loop)
		Node->for_loop->accept(this);
	else
		Node->while_loop->accept(this);
}
void ASTVisitor::visit(class ASTStatement * Node){
	statements_vector.push_back(Node);
	TAB;
	if(Node->exp){
		gotolabels.insert(make_pair(Node->exp->get_var_name(),statements_vector.size()-1));
		out<<"<statement with label>"<<endl;
		Node->exp->accept(this);
	}
	else
		out<<"<statement>"<<endl;
	tab_count++;
	if(Node->gen_stmnt)
		Node->gen_stmnt->accept(this);
	else if(Node->loop_stmnt)
		Node->loop_stmnt->accept(this);
	else
		Node->if_stmnt->accept(this);
	tab_count--;
	TAB;
	out<<"</statement>"<<endl;
}

// InterpreterVisitor
int InterpreterVisitor::visit(class ASTPrimaryArithExp * Node){
	if(Node->is_literal){
		return Node->literal;
	}
	if(Node->var_name != ""){
		auto it = reserved_space.find(Node->var_name);
		if(!Node->is_array)
			return *(it->second.location);
	}
	if(Node->is_array){
		int array_val = Node->expr->accept(this);
		auto it = reserved_space.find(Node->var_name);
		if(it != reserved_space.end()){
			try{
				return (it->second.location[array_val]);
			}
			catch(int e){
				throw out_of_range("Array Index out of range");
				exit(1);
			}
		}
	}
	if(Node->sign){
		return -(Node->expr->accept(this));
	}
	if(Node->is_brace){
		return (Node->expr->accept(this));
	}
}
int* InterpreterVisitor::lhs_visit(class ASTPrimaryArithExp * Node){
	auto it = reserved_space.find(Node->var_name);
	if(it != reserved_space.end()){
		try{
			if(Node->is_array){
				int array_val = Node->expr->accept(this);
				return (it->second.location + (array_val * sizeof(int)));
			}
			else{
				return (it->second.location);
			}
		}
		catch(int e){
			throw out_of_range("Array Index out of range");
			exit(1);
		}
	}
}
string InterpreterVisitor::string_visit(class ASTPrimaryArithExp * Node){
	if(Node->is_literal)
		return Node->str_literal;
	else
		return Node->var_name;
}
int InterpreterVisitor::visit(class ASTArithExp * Node){
	if(Node->is_primary)
		return Node->expr->accept(this);
	int left_val = Node->left->accept(this);
	int right_val = Node->right->accept(this);
	if(Node->operation == "+")
		return left_val + right_val;
	else if(Node->operation == "-")
		return left_val - right_val;
	else if(Node->operation == "*")
		return left_val * right_val;
	else
		return left_val / right_val;
}
bool InterpreterVisitor::visit(class ASTBoolExp * Node){
	if(Node->is_brace)
		return Node->expr->accept(this);
	int left_val = Node->left->accept(this);
	int right_val = Node->right->accept(this);
	if(Node->compare == "<"){
		return (left_val<right_val);
	}
	else if(Node->compare == ">"){
		return (left_val>right_val);
	}
	else if(Node->compare == "<="){
		return (left_val<=right_val);
	}
	else if(Node->compare == ">="){
		return (left_val>=right_val);
	}
	else if(Node->compare == "=="){
		return (left_val==right_val);
	}
	else{
		return (left_val!=right_val);
	}
}
void InterpreterVisitor::visit(class ASTAssgnStmt * Node){
	if(Node->is_read){
		int* temp = Node->ident->lhs_accept(this);
		if(Node->array_exp){
			int jump = Node->array_exp->accept(this);
			scanf("%d", &temp[jump]);
		}
		else{
			scanf("%d", temp);
		}
	}
	else{
		int* temp = Node->ident->lhs_accept(this);
		if(Node->array_exp){
			int jump = Node->array_exp->accept(this);
			for(int jc=0;jc<jump;++jc)
				temp += 1;
			*(temp) = Node->exp->accept(this);
		}
		else{
			*(temp) = Node->exp->accept(this);
		}
	}
}
void InterpreterVisitor::visit(class ASTPrintStmnt * Node){
	for(int i=Node->arglist.size()-1; i>=0; --i){
		if(Node->arglist[i]->str_literal != "")
			cout<<Node->arglist[i]->string_accept(this);
		else
			cout<<Node->arglist[i]->accept(this);
	}
	if(Node->is_println)
		cout<<endl;
}
void InterpreterVisitor::visit(class ASTLoopStmnt * Node){
	if(Node->for_loop)
		Node->for_loop->accept(this);
	else
		Node->while_loop->accept(this);
}
void InterpreterVisitor::visit(class ASTGenStmnt * Node){
	if(Node->assgn_read_stmnt)
		Node->assgn_read_stmnt->accept(this);
	else if(Node->print_stmnt)
		Node->print_stmnt->accept(this);
	else
		Node->goto_stmnt->accept(this);
}
void InterpreterVisitor::visit(class ASTStatement * Node){
	if(Node->gen_stmnt)
		Node->gen_stmnt->accept(this);
	else if(Node->loop_stmnt)
		Node->loop_stmnt->accept(this);
	else
		Node->if_stmnt->accept(this);
}
void InterpreterVisitor::visit(class ASTIfStmnt * Node){
	if(Node->is_goto){
		gotoFlag = true;
		string temp = Node->ident->string_accept(this);
		if(Node->bool_exp){
			if(Node->bool_exp->accept(this)){
				program_counter = gotolabels.find(temp)->second - 1;
			}
		}
		else{
			program_counter = gotolabels.find(temp)->second - 1;
		}
	}
	else{
		int jump_size = Node->if_statements->statements.size();
		if(Node->else_statements)
			jump_size += Node->else_statements->statements.size();
		if(Node->bool_exp->accept(this)){
			for(int i=Node->if_statements->statements.size()-1;i>=0;--i){
				Node->if_statements->statements[i]->accept(this);
				if(gotoFlag)
					return;
			}
		}
		else if(Node->else_statements){
			for(int i=Node->else_statements->statements.size()-1;i>=0;--i){
				Node->else_statements->statements[i]->accept(this);
				if(gotoFlag)
					return;
			}
		}
		program_counter = program_counter + jump_size;
	}
}
void InterpreterVisitor::visit(class ASTWhileLoop * Node){
	int jump_size = Node->statements->statements.size();
	int old_pc = program_counter;
	while(Node->bool_exp->accept(this)){
		for(int i=Node->statements->statements.size()-1;i>=0; --i){
			Node->statements->statements[i]->accept(this);
			if(gotoFlag)
				return;
		}
		if(program_counter != old_pc and Node->bool_exp->accept(this)){
			program_counter = old_pc;
		}
	}
	program_counter = program_counter + jump_size;
}
void InterpreterVisitor::visit(class ASTForLoop * Node){
	int jump_size = Node->statements->statements.size();
	int* temp = Node->exp->lhs_accept(this);
	int begin = Node->begin->accept(this);
	int end = Node->end->accept(this);
	int step = 1;
	int old_pc = program_counter;
	if(Node->is_step)
		step = Node->step->accept(this);
	for(*(temp) = begin; *(temp)<end; *(temp) += step){
		for(int i=Node->statements->statements.size()-1;i>=0; --i){
			Node->statements->statements[i]->accept(this);
			if(gotoFlag)
				return;
		}
		if(program_counter != old_pc and ((*(temp) + step) < end))
			program_counter = old_pc;
	}
	program_counter = program_counter + jump_size;
}

// CodeGenVisitor Code
CodeGenVisitor::CodeGenVisitor(ASTProgram * start){
	string id = "program";
	this->module = new llvm::Module(id, llvm::getGlobalContext());
	this->start = start;
	llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), false);
	this->mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main", module);
}
void CodeGenVisitor::codeGen(){
	llvm::BasicBlock *block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", this->mainFunction, 0);
	this->symbolTable.pushBlock(block);
	this->visit(start);
	block = symbolTable.topBlock();
	symbolTable.popBlock();
	llvm::ReturnInst::Create(llvm::getGlobalContext(), block);
	llvm::verifyModule(*module);
	llvm::legacy::PassManager PM;
	PM.add(llvm::createPrintModulePass(llvm::outs()));
	PM.run(*module);
}
llvm::Value * CodeGenVisitor::ErrorHandler(const char * error){
	cerr<<error<<endl;
	exit(-1);
}
void * CodeGenVisitor::visit(ASTProgram * Node){
	this->visit(Node->decl_block);

	llvm::Function * iterator = NULL;
	llvm::Function * userMain = NULL;

	iterator = static_cast<llvm::Function *>(this->visit(Node->code_block));
	userMain = iterator;

	llvm::CallInst::Create(userMain, "", symbolTable.topBlock());
}

void * CodeGenVisitor::visit(ASTDeclBlock * Node){
	this->visit(Node->decls);
}
void * CodeGenVisitor::visit(ASTDecls * Node){
	for(int i=Node->decl_list.size()-1; i>=0; --i)
		this->visit(Node->decl_list[i]);
}
void * CodeGenVisitor::visit(ASTDecl * Node){
	for(int i=Node->var_list.size()-1; i>=0; --i)
		this->visit(Node->var_list[i]);
}
void * CodeGenVisitor::visit(ASTVar * Node){
	if(Node->decl_type){
		if(Node->length <= 0)
			return ErrorHandler("Error Invalid Size");
		llvm::GlobalVariable* variable = new llvm::GlobalVariable(*module, llvm::ArrayType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), Node->length), false, llvm::GlobalValue::CommonLinkage, NULL, Node->var_name);
        variable->setInitializer(llvm::ConstantAggregateZero::get(llvm::ArrayType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), Node->length)));
        symbolTable.declareLocalVariables(Node->var_name, variable);
        return variable;
	}
	else{
		llvm::GlobalVariable * globalInteger = new llvm::GlobalVariable(*module, llvm::Type::getInt64Ty(llvm::getGlobalContext()), false, llvm::GlobalValue::CommonLinkage, NULL, Node->var_name);
        globalInteger->setInitializer(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true));
        symbolTable.declareLocalVariables(Node->var_name, globalInteger);
        return globalInteger;
	}
}

void * CodeGenVisitor::visit(ASTCodeBlock * Node){
	vector<llvm::Type*> argTypes;
	string name = "main";
	llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, name, module);
    llvm::BasicBlock *block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", function, 0); 
    symbolTable.pushBlock(block);

    this->visit(Node->codes);

    if(!symbolTable.topBlock()->getTerminator()){
        llvm::ReturnInst::Create(llvm::getGlobalContext(), llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), symbolTable.topBlock());
    }

    symbolTable.popBlock();
    return function;
}
void * CodeGenVisitor::visit(ASTCodes * Node){
	for(int i=Node->statements.size()-1; i>=0; i--){
		this->visit(Node->statements[i]);
	}
}
void * CodeGenVisitor::visit(ASTStatement * Node){
	if(Node->gen_stmnt)
		this->visit(Node->gen_stmnt);
	else if(Node->loop_stmnt)
		this->visit(Node->loop_stmnt);
	else
		this->visit(Node->if_stmnt);
}
void * CodeGenVisitor::visit(ASTGenStmnt * Node){
	if(Node->assgn_read_stmnt)
		this->visit(Node->assgn_read_stmnt);
	else if(Node->print_stmnt)
		this->visit(Node->print_stmnt);
	else
		this->visit(Node->goto_stmnt);
}
void * CodeGenVisitor::visit(ASTAssgnStmt * Node){
	if(Node->is_read){
		// scanf not implemented
	}
	else{
		llvm::Value * location = NULL;
        llvm::Value * existingValue = NULL;
        
        if(Node->array_exp){
            if(!symbolTable.lookupGlobalVariables(Node->ident->var_name)){
                return ErrorHandler("Variable Not Declared");
            }
            std::vector <llvm::Value *> index;
            index.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true));
            index.push_back(static_cast<llvm::Value *>(this->visit(Node->array_exp)));
            llvm::Value * val = symbolTable.returnLocalVariables(Node->ident->var_name);
            location = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", symbolTable.topBlock());
        }
        else{
            if(!symbolTable.lookupGlobalVariables(Node->ident->var_name)){
                return ErrorHandler("Variable Not Declared");
            }
            location = symbolTable.returnLocalVariables(Node->ident->var_name);
        }
        llvm::Value * expr = static_cast<llvm::Value *>(this->visit(Node->exp));
        if(!expr->getType()->isIntegerTy(64)){
            return ErrorHandler("RHS of assignment statement is invalid");
        }
        if(!location->getType()->isPointerTy()){
            return ErrorHandler("LHS of assignment statement is invalid");
        }
        return new llvm::StoreInst(expr, location, false, symbolTable.topBlock());
	}
}
void * CodeGenVisitor::visit(ASTPrintStmnt * Node){
	string func = "printf";
	string arg = "\n";
	
	vector<llvm::Value *>args;
	string temparglist = "";

	for(int i=Node->arglist.size()-1; i>=0; --i){
		if(Node->arglist[i]->str_literal != ""){
			string temparg = Node->arglist[i]->str_literal;
        	temparglist += temparg;
		}
		else{
			string temparg = "%d";
			temparglist += temparg;        	
			args.push_back(static_cast<llvm::Value *>(this->visit(Node->arglist[i])));
		}
	}

	if(Node->is_println)
		temparglist += arg;
	auto it = args.begin();
	llvm::GlobalVariable* variable = new llvm::GlobalVariable(*module, llvm::ArrayType::get(llvm::IntegerType::get(llvm::getGlobalContext(), 8), temparglist.size() + 1), true, llvm::GlobalValue::InternalLinkage, NULL, "string");
    variable->setInitializer(llvm::ConstantDataArray::getString(llvm::getGlobalContext(), temparglist, true));
    args.insert(it,static_cast<llvm::Value *>(variable));

	llvm::Function * function = module->getFunction(func);
	if(!function){
		llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), true);
		function = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, func, module);
	}
	llvm::CallInst *call = llvm::CallInst::Create(function, llvm::makeArrayRef(args), func, symbolTable.topBlock());
	
	return call;
}

void * CodeGenVisitor::visit(ASTIfStmnt * Node){
	if(Node->is_goto){
		// goto not implemented
	}
	else{
		llvm::BasicBlock * entryBlock = symbolTable.topBlock();
        llvm::Value * condition = static_cast<llvm::Value *>(this->visit(Node->bool_exp));
        llvm::ICmpInst * comparison = new llvm::ICmpInst(*entryBlock, llvm::ICmpInst::ICMP_NE, condition, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), "tmp");
        llvm::BasicBlock * ifBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "ifBlock", entryBlock->getParent());
        llvm::BasicBlock * mergeBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "mergeBlock", entryBlock->getParent());

        llvm::BasicBlock * returnedBlock = NULL;

        symbolTable.pushBlock(ifBlock);
        this->visit(Node->if_statements);
        returnedBlock = symbolTable.topBlock();
        symbolTable.popBlock();
        if(!returnedBlock->getTerminator()){
            llvm::BranchInst::Create(mergeBlock, returnedBlock);
        }
        if(Node->else_statements){
            llvm::BasicBlock * elseBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "elseBlock", entryBlock->getParent());

            symbolTable.pushBlock(elseBlock);
            this->visit(Node->else_statements);
            returnedBlock = symbolTable.topBlock();
            symbolTable.popBlock();
            if (!returnedBlock->getTerminator()){
                llvm::BranchInst::Create(mergeBlock, returnedBlock);
            }
            llvm::BranchInst::Create(ifBlock, elseBlock, comparison, entryBlock);
        }
        else{
           llvm::BranchInst::Create(ifBlock, mergeBlock, comparison, entryBlock);
        }
        auto localVariables = symbolTable.getLocalVariables();
        symbolTable.popBlock();
        symbolTable.pushBlock(mergeBlock);
        symbolTable.setLocalVariables(localVariables);
	}	
}

void * CodeGenVisitor::visit(ASTLoopStmnt * Node){
	if(Node->for_loop)
		this->visit(Node->for_loop);
	else
		this->visit(Node->while_loop);
}
void * CodeGenVisitor::visit(ASTWhileLoop * Node){
    llvm::BasicBlock * entryBlock = symbolTable.topBlock();
    llvm::BasicBlock * headerBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "loop_header", entryBlock->getParent(), 0);
    llvm::BasicBlock * bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "loop_body", entryBlock->getParent(), 0);
    llvm::BasicBlock * afterLoopBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "after_loop", entryBlock->getParent(), 0);

    symbolTable.pushBlock(headerBlock);
    llvm::Value * condition = static_cast<llvm::Value *>(this->visit(Node->bool_exp));
    symbolTable.popBlock();
    llvm::ICmpInst * comparison = new llvm::ICmpInst(*headerBlock, llvm::ICmpInst::ICMP_NE, condition, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), "tmp");
    llvm::BranchInst::Create(bodyBlock, afterLoopBlock, comparison, headerBlock);
    llvm::BranchInst::Create(headerBlock, entryBlock);

    symbolTable.pushBlock(bodyBlock);
    this->visit(Node->statements);
    bodyBlock = symbolTable.topBlock();
    symbolTable.popBlock();
    if(!bodyBlock->getTerminator()){
        llvm::BranchInst::Create(headerBlock, bodyBlock);
    }

    auto localVariables = symbolTable.getLocalVariables();
    symbolTable.popBlock();
    symbolTable.pushBlock(afterLoopBlock);
    symbolTable.setLocalVariables(localVariables);
}
void * CodeGenVisitor::visit(ASTForLoop * Node){
	llvm::BasicBlock * entryBlock = symbolTable.topBlock();
    llvm::BasicBlock * headerBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "loop_header", entryBlock->getParent(), 0);
    llvm::BasicBlock * bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "loop_body", entryBlock->getParent(), 0);
    llvm::BasicBlock * afterLoopBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "after_loop", entryBlock->getParent(), 0);

    llvm::Value * location = NULL;
    llvm::Value * existingValue = NULL;
    if(Node->is_array){
        if(!symbolTable.lookupGlobalVariables(Node->exp->var_name)){
            return ErrorHandler("Variable Not Declared");
        }
        std::vector <llvm::Value *> index;
        index.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true));
        index.push_back(static_cast<llvm::Value *>(this->visit(Node->exp->expr)));
        llvm::Value * val = symbolTable.returnLocalVariables(Node->exp->var_name);
        location = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", symbolTable.topBlock());
    }
    else{
        if(!symbolTable.lookupGlobalVariables(Node->exp->var_name)){
            return ErrorHandler("Variable Not Declared");
        }
        location = symbolTable.returnLocalVariables(Node->exp->var_name);
    }
    llvm::Value * expr = static_cast<llvm::Value *>(this->visit(Node->begin));
    if(!expr->getType()->isIntegerTy(64)){
        return ErrorHandler("RHS of assignment statement is invalid");
    }
    if(!location->getType()->isPointerTy()){
        return ErrorHandler("LHS of assignment statement is invalid");
    }
    new llvm::StoreInst(expr, location, false, symbolTable.topBlock());

    llvm::Value * step = static_cast<llvm::Value *>(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 1, true));
    if(Node->is_step){
    	step = static_cast<llvm::Value *>(this->visit(Node->step));
    }

    symbolTable.pushBlock(headerBlock);
    llvm::Value * condition = static_cast<llvm::Value *>(this->visit(Node->exp));
    llvm::Value * finalCondn = static_cast<llvm::Value *>(this->visit(Node->end));
    symbolTable.popBlock();
    llvm::ICmpInst * comparison = new llvm::ICmpInst(*headerBlock, llvm::ICmpInst::ICMP_SLT, condition, finalCondn, "tmp");
    llvm::BranchInst::Create(bodyBlock, afterLoopBlock, comparison, headerBlock);
    llvm::BranchInst::Create(headerBlock, entryBlock);

    symbolTable.pushBlock(bodyBlock);
    this->visit(Node->statements);
    bodyBlock = symbolTable.topBlock();
    new llvm::StoreInst(llvm::BinaryOperator::Create(llvm::Instruction::Add, static_cast<llvm::Value*>(step), static_cast<llvm::Value*>(this->visit(Node->exp)), "tmp", symbolTable.topBlock()), location, false, symbolTable.topBlock());
    symbolTable.popBlock();
    if(!bodyBlock->getTerminator()){
        llvm::BranchInst::Create(headerBlock, bodyBlock);
    }

    auto localVariables = symbolTable.getLocalVariables();
    symbolTable.popBlock();
    symbolTable.pushBlock(afterLoopBlock);
    symbolTable.setLocalVariables(localVariables);	
}

void * CodeGenVisitor::visit(ASTPrimaryArithExp * Node){
	if(Node->is_literal){
		return llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), Node->literal, true);
	}
	if(Node->var_name != "" and (!Node->is_array)){
		if (!symbolTable.lookupGlobalVariables(Node->var_name)) {
            return ErrorHandler("Variable Not Declared");
        }
        llvm::Value * val = symbolTable.returnLocalVariables(Node->var_name);
		if(!Node->is_array)
			if (val)
            	return new llvm::LoadInst(val, "tmp", symbolTable.topBlock());
        	return ErrorHandler("Variable Not Initilized");
	}
	if(Node->is_array){
		if (!symbolTable.lookupGlobalVariables(Node->var_name)) {
            return ErrorHandler("Variable Not Declared");
        }
        std::vector <llvm::Value *> index;
        index.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true));
        index.push_back(static_cast<llvm::Value *>(this->visit(Node->expr)));
        llvm::Value * val = symbolTable.returnLocalVariables(Node->var_name);
        llvm::Value * offset = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", symbolTable.topBlock());
        if(val){
            llvm::LoadInst * load = new llvm::LoadInst(offset, "tmp", symbolTable.topBlock());
            return load;
        }
        return ErrorHandler("Variable Not Initialized");
	}
	if(Node->sign){
		llvm::BinaryOperator::Create(llvm::Instruction::Sub, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), static_cast<llvm::Value*>(this->visit(Node->expr)), "tmp", symbolTable.topBlock());
	}
	if(Node->is_brace){
		return this->visit(Node->expr);
	}
}
void * CodeGenVisitor::visit(ASTArithExp * Node){
	if(Node->is_primary)
		return this->visit(Node->expr);
	if(Node->operation == "+")
		return llvm::BinaryOperator::Create(llvm::Instruction::Add, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)), "tmp", symbolTable.topBlock());
	else if(Node->operation == "-")
		return llvm::BinaryOperator::Create(llvm::Instruction::Sub, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)), "tmp", symbolTable.topBlock());
	else if(Node->operation == "*")
		return llvm::BinaryOperator::Create(llvm::Instruction::Mul, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)), "tmp", symbolTable.topBlock());
	else
		return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)), "tmp", symbolTable.topBlock());
}
void * CodeGenVisitor::visit(ASTBoolExp * Node){
	if(Node->is_brace)
		return this->visit(Node->expr);
	if(Node->compare == "<"){
		return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLT, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
	}
	else if(Node->compare == ">"){
		return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGT, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
	}
	else if(Node->compare == "<="){
		return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLE, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
	}
	else if(Node->compare == ">="){
		return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGE, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
	}
	else if(Node->compare == "=="){
		return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_EQ, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
	}
	else{
		return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_NE, static_cast<llvm::Value*>(this->visit(Node->left)), static_cast<llvm::Value*>(this->visit(Node->right)),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
	}
}