bcc:	parser.tab.c lex.yy.c
	g++ -w -Wno-deprecated -g -std=c++11 parser.tab.c lex.yy.c AST.cpp -lfl `llvm-config --cxxflags --cppflags mcjit native --libs core --ldflags --system-libs` -pthread -ltinfo -fexceptions -o bcc
parser.tab.c: parser.y 
	bison -v -d parser.y
parser.tab.h: parser.y
	bison -v -d parser.y
lex.yy.c: scanner.l parser.tab.h
	flex scanner.l

.PHONY: clean 
clean:
	-@rm -rf parser.tab.c parser.tab.h lex.yy.c parser.output bcc XML_visitor.txt 2>/dev/null || true
