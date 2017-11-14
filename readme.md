# Compiler AST  
### Execution
`To run:`  
- `make`
- `./bcc input_file.b`  
The scanner outputs to the terminal.  
The AST pass is output to the `XML_visitor.txt` file.  
The interpreter has been implemented (with goto support) and the output is shown on the terminal.  
The LLVM IR is also emitted to the screen. One can copy this to a file named `input_file.ll` and execute it with `lli input_file.ll` or one could use something like `llc input_file.ll` to get a `.obj` or a `.s` file.  
Use `make clean` to cleanup.  
### Notes
- Please look at the report for a detailed description.
- `AST.h` contains the header files for the code in `AST.cpp` which contains the code for AST construction, the Interpreter and the LLVM IR Code Generation.
- `scanner.l` is the lexer - uses Flex.
- `parser.y` is the parser - uses Bison.
- Files ending with `.b` are FlatB files.
- `dot` diagrams can be created using the `opt` tool.
