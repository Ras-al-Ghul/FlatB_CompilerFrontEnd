#include <iostream>
#include <cstdlib>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <stdexcept>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

using namespace std;

class Visitor{
	public:
    	virtual void visit(class ASTProgram *) = 0;
    	virtual void visit(class ASTDeclBlock *) = 0;
    	virtual void visit(class ASTDecls *) = 0;
    	virtual void visit(class ASTDecl *) = 0;
    	virtual void visit(class ASTVar *) = 0;
    	virtual void visit(class ASTCodeBlock *) = 0;
    	virtual void visit(class ASTCodes *) = 0;
    	virtual int visit(class ASTPrimaryArithExp *) = 0;
    	virtual int visit(class ASTArithExp *) = 0;
    	virtual bool visit(class ASTBoolExp *) = 0;
    	virtual void visit(class ASTForLoop *) = 0;
    	virtual void visit(class ASTWhileLoop *) = 0;
    	virtual void visit(class ASTAssgnStmt *) = 0;
    	virtual void visit(class ASTPrintStmnt *) = 0;
    	virtual void visit(class ASTIfStmnt *) = 0;
    	virtual void visit(class ASTGenStmnt *) = 0;
    	virtual void visit(class ASTLoopStmnt *) = 0;
    	virtual void visit(class ASTStatement *) = 0;

    	virtual int* lhs_visit(class ASTPrimaryArithExp *) = 0;
		virtual string string_visit(class ASTPrimaryArithExp *) = 0;
};

class ASTNode{
	public:
	    ASTNode(){}
    	~ASTNode(){}
};

class ASTVar : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		bool decl_type;	//array or not - 1 if array
		string var_name;	//name of the variable
		int length;	//length of array if array
	public:
		ASTVar(bool, string, int length);
		ASTVar(bool, string);
		void accept(Visitor *);
		~ASTVar();
};

class ASTVars : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		vector<class ASTVar *> var_list;
	public:
		ASTVars();
		void push_back(class ASTVar *);
		void push_back(class ASTVars *);
		vector<class ASTVar *> get_var_list();
		~ASTVars();
};

class ASTProgram : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
    	class ASTDeclBlock *decl_block;
    	class ASTCodeBlock *code_block;
	public:
	    ASTProgram(class ASTDeclBlock *, class ASTCodeBlock *);
	    void accept(Visitor *);
	    ~ASTProgram();
};

class ASTDeclBlock : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		class ASTDecls *decls;
	public:
		ASTDeclBlock(class ASTDecls *);
		void accept(Visitor *);
		~ASTDeclBlock();
};

class ASTDecls : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		vector <class ASTDecl *> decl_list;
		int count;
	public:
		ASTDecls();
		void push_back(class ASTDecl *);
		vector<class ASTDecl *> get_decl_list();
		void accept(Visitor *);
		~ASTDecls();
};

class ASTDecl : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		vector <class ASTVar *> var_list;
	public:
		ASTDecl(vector<class ASTVar *>);
		vector<class ASTVar *> get_var_list();
		void accept(Visitor *);
		~ASTDecl();
};

class ASTIfStmnt : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		bool is_goto;
		class ASTPrimaryArithExp * ident;	// identifier for goto
		class ASTBoolExp * bool_exp;
		class ASTCodes * if_statements;
		class ASTCodes * else_statements;
	public:
		ASTIfStmnt(class ASTPrimaryArithExp *);	// for goto
		ASTIfStmnt(class ASTPrimaryArithExp *, class ASTBoolExp *);	// for conditional goto
		ASTIfStmnt(class ASTBoolExp *, class ASTCodes *);	// for if
		ASTIfStmnt(class ASTBoolExp *, class ASTCodes *, class ASTCodes *);	// for if else
		void accept(Visitor *);
		~ASTIfStmnt();
};

class ASTPrintStmnt : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		bool is_println;	// if println
		vector<class ASTPrimaryArithExp *> arglist;
		int count = 0;	// size of vector
	public:
		ASTPrintStmnt();
		void set_is_println(bool);
		void push_back(class ASTPrimaryArithExp *);
		void accept(Visitor *);
		~ASTPrintStmnt();
};

class ASTPrimaryArithExp : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		string str_literal;	// if string literal
		int literal;	// if number
		bool is_literal;	// is it a literal
		string var_name;	// if variable
		bool sign;	// negative or not - 1 for negative
		bool is_brace;	// surrounded by braces or not
		bool is_array;	// array expression or not
		class ASTArithExp *expr;	// the related Arith expr if any
	public:
		ASTPrimaryArithExp(string, bool);	// string literal
		ASTPrimaryArithExp(int);
		ASTPrimaryArithExp(string);
		ASTPrimaryArithExp(bool, ASTArithExp *);	// signed expr
		ASTPrimaryArithExp(bool, ASTArithExp *, string);	// array
		ASTPrimaryArithExp(ASTArithExp *, bool);	// braces
		int accept(Visitor *);
		int* lhs_accept(Visitor *);
		string string_accept(Visitor *);
		string get_var_name(){return this->var_name;}
		~ASTPrimaryArithExp();
};

class ASTArithExp : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		bool is_primary; // if primaryArithExpr
		class ASTPrimaryArithExp *expr;
		class ASTArithExp * left;
		class ASTArithExp * right;
		string operation;
	public:
		ASTArithExp(ASTArithExp *, ASTArithExp *, string);
		ASTArithExp(ASTPrimaryArithExp *);
		int accept(Visitor *);
		~ASTArithExp();
};

class ASTBoolExp : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		bool is_brace;
		class ASTArithExp * left;
		class ASTArithExp * right;
		class ASTBoolExp * expr;
		string compare;
	public:
		ASTBoolExp(ASTArithExp *, ASTArithExp *, string);
		ASTBoolExp(ASTBoolExp *);
		bool accept(Visitor *);
		~ASTBoolExp();
};

class ASTForLoop : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		bool is_array;
		bool is_step;	// step counter 1 if exists
		class ASTPrimaryArithExp * exp;
		class ASTArithExp * begin;
		class ASTArithExp * end;
		class ASTArithExp * step;
		class ASTCodes * statements;
	public:
		ASTForLoop(ASTPrimaryArithExp *, ASTArithExp *, ASTArithExp *, ASTArithExp *, ASTCodes *, bool);
		ASTForLoop(ASTPrimaryArithExp *, ASTArithExp *, ASTArithExp *, ASTCodes *, bool);
		void accept(Visitor *);
		~ASTForLoop();
};

class ASTWhileLoop : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		class ASTBoolExp * bool_exp;
		class ASTCodes * statements;
	public:
		ASTWhileLoop(ASTBoolExp *, ASTCodes *);
		void accept(Visitor *);
		~ASTWhileLoop();
};

class ASTAssgnStmt : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		bool is_read;	// is it a read statement 1 if true
		class ASTPrimaryArithExp * ident;
		class ASTArithExp * array_exp;
		class ASTArithExp * exp;
	public:
		ASTAssgnStmt(ASTPrimaryArithExp *, ASTArithExp *, bool);
		ASTAssgnStmt(ASTPrimaryArithExp *, ASTArithExp *, ASTArithExp *);
		ASTAssgnStmt(ASTPrimaryArithExp *);
		void accept(Visitor *);
		~ASTAssgnStmt();
};

class ASTGenStmnt : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		class ASTAssgnStmt * assgn_read_stmnt;
		class ASTPrintStmnt * print_stmnt;
		class ASTIfStmnt * goto_stmnt;
	public:
		ASTGenStmnt(ASTAssgnStmt *);
		ASTGenStmnt(ASTPrintStmnt *);
		ASTGenStmnt(ASTIfStmnt *);
		void accept(Visitor *);
		~ASTGenStmnt();
};

class ASTLoopStmnt : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		class ASTForLoop * for_loop;
		class ASTWhileLoop * while_loop;
	public:
		ASTLoopStmnt(ASTForLoop *);
		ASTLoopStmnt(ASTWhileLoop *);
		void accept(Visitor *);
		~ASTLoopStmnt();
};

class ASTStatement : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		class ASTGenStmnt *gen_stmnt;
		class ASTLoopStmnt *loop_stmnt;
		class ASTIfStmnt *if_stmnt;
		class ASTPrimaryArithExp *exp;
	public:
		ASTStatement(ASTGenStmnt *, ASTPrimaryArithExp *);
		ASTStatement(ASTLoopStmnt *, ASTPrimaryArithExp *);
		ASTStatement(ASTIfStmnt *, ASTPrimaryArithExp *);
		void accept(Visitor *);
		~ASTStatement();
};

class ASTCodeBlock : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		class ASTCodes *codes;
	public:
		ASTCodeBlock(class ASTCodes*);
		void accept(Visitor *);
		~ASTCodeBlock();
};

class ASTCodes : public ASTNode{
	friend class ASTVisitor;
	friend class InterpreterVisitor;
	friend class CodeGenVisitor;
	private:
		vector<ASTStatement *> statements;
	public:
		ASTCodes();
		void push_back(ASTStatement *);
		void accept(Visitor *);
		~ASTCodes();
};

class ASTVisitor : public Visitor{
	private:
		class ASTProgram * start;
	public:
		ASTVisitor(class ASTProgram *);
		void visit(class ASTProgram *);
    	void visit(class ASTDeclBlock *);
    	void visit(class ASTDecls *);
    	void visit(class ASTDecl *);
    	void visit(class ASTVar *);
    	void visit(class ASTCodeBlock *);
    	void visit(class ASTCodes *);
    	int visit(class ASTPrimaryArithExp *);
    	int visit(class ASTArithExp *);
    	bool visit(class ASTBoolExp *);
    	void visit(class ASTForLoop *);
    	void visit(class ASTWhileLoop *);
    	void visit(class ASTAssgnStmt *);
    	void visit(class ASTPrintStmnt *);
    	void visit(class ASTIfStmnt *);
    	void visit(class ASTGenStmnt *);
    	void visit(class ASTLoopStmnt *);
    	void visit(class ASTStatement *);

    	int* lhs_visit(ASTPrimaryArithExp *){};
		string string_visit(ASTPrimaryArithExp *){};
		~ASTVisitor();
};

class InterpreterVisitor : public Visitor{
	public:
		InterpreterVisitor();
		void visit(class ASTProgram *){};
    	void visit(class ASTDeclBlock *){};
    	void visit(class ASTDecls *){};
    	void visit(class ASTDecl *){};
    	void visit(class ASTVar *){};
    	void visit(class ASTCodeBlock *){};
    	void visit(class ASTCodes *){};

    	int visit(class ASTPrimaryArithExp *);
    	int* lhs_visit(ASTPrimaryArithExp *);
		string string_visit(ASTPrimaryArithExp *);
    	int visit(class ASTArithExp *);
    	bool visit(class ASTBoolExp *);
    	void visit(class ASTForLoop *);
    	void visit(class ASTWhileLoop *);
    	void visit(class ASTAssgnStmt *);
    	void visit(class ASTPrintStmnt *);
    	void visit(class ASTIfStmnt *);
    	void visit(class ASTGenStmnt *);
    	void visit(class ASTLoopStmnt *);
    	void visit(class ASTStatement *);
		~InterpreterVisitor();
};

class DataStorage{
	public:
		string name;
		int size;
		int* location;
		DataStorage(string, int);
		~DataStorage(){};
};

// Symbol Table classes and methods
class SymbolTableNode{
	public:
		llvm::BasicBlock * block;
		std::map<std::string, llvm::Value *> localVariables;
		SymbolTableNode(llvm::BasicBlock * block){
			this->block = block;
		}
		~SymbolTableNode(){}
};
class SymbolTable{
	private:
		list<SymbolTableNode> table;
	public:
		SymbolTable(){}
		~SymbolTable(){}
		std::map<std::string, llvm::Value *> getLocalVariables() {
	        return this->table.front().localVariables;
	    }
	    void setLocalVariables(std::map<std::string, llvm::Value *> variables) {
	        this->table.front().localVariables.insert(variables.begin(), variables.end());
	    }
	    bool lookupLocalVariables(std::string name) {
	        auto it = this->getLocalVariables();
	        return it.find(name) != it.end();
	    }
	    void declareLocalVariables(std::string name, llvm::Value * value) {
	        if (!this->lookupLocalVariables(name)) {
	            this->table.front().localVariables.insert(std::pair<std::string, llvm::Value *>(name, value));
	        } else {
	            std::cerr<<"Variable "<<name<<" already declared";
	            exit(0);
	        }
	    }
	    bool lookupGlobalVariables(std::string name) {
	        return this->returnLocalVariables(name) != NULL;
	    }

	    llvm::Value * returnLocalVariables(std::string name) {
	        for (auto it = this->table.begin(); it != this->table.end(); it++) {
	            auto found_or_end = it->localVariables.find(name);
	            if (found_or_end != it->localVariables.end()) {
	                return found_or_end->second;
	            }
	        }
	        return NULL;
	    }

		void pushBlock(llvm::BasicBlock *block){
			this->table.push_front(SymbolTableNode(block));
		}
		void popBlock(){
			this->table.pop_front();
		}
		llvm::BasicBlock * topBlock(){
			for(auto it = this->table.begin(); it != this->table.end(); ++it)
				if(it->block)
					return it->block;
			return this->table.front().block;
		}
		llvm::BasicBlock * bottomBlock(){
			return this->table.back().block;
		}
};
// Symbol Table definitions end

class CodeGenVisitor{
	private:
		class ASTProgram * start;
		llvm::Module * module;
		llvm::Function * mainFunction;
		SymbolTable symbolTable;
	public:
		CodeGenVisitor(class ASTProgram *);
		~CodeGenVisitor(){};
		void * visit(class ASTProgram *);
    	void * visit(class ASTDeclBlock *);
    	void * visit(class ASTDecls *);
    	void * visit(class ASTDecl *);
    	void * visit(class ASTVar *);
    	void * visit(class ASTCodeBlock *);
    	void * visit(class ASTCodes *);
    	void * visit(class ASTPrimaryArithExp *);
    	void * visit(class ASTArithExp *);
    	void * visit(class ASTBoolExp *);
    	void * visit(class ASTForLoop *);
    	void * visit(class ASTWhileLoop *);
    	void * visit(class ASTAssgnStmt *);
    	void * visit(class ASTPrintStmnt *);
    	void * visit(class ASTIfStmnt *);
    	void * visit(class ASTGenStmnt *);
    	void * visit(class ASTLoopStmnt *);
    	void * visit(class ASTStatement *);		
		void codeGen();
		llvm::Value * ErrorHandler(const char *);
};