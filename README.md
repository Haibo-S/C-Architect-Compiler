## C-Architect-Compiler

The project consists of two main components: an assembler that converts MIPS Assembly into machine code, and a compiler that translates code from the WLP4 language (a subset of C programming langugage) into MIPS Assembly.

## Setup
Clone the repository:

``` bash
git clone git@github.com:Haibo-S/C-Architect-Compiler.git
```


To compile and run the compiler on the University of Waterloo server:

```bash
source /u/cs241/setup
```


For the compiler, navigate to the compiler source directory and compile the WLP4 code:

```bash
cd src/compiler
g++ wlp4gen-prescanned.cc wlp4data.cc -o wlp4gen
./wlp4scan < test.wlp4 | ./wlp4gen > ./output/output.asm
```


For the assembler, navigate to the assembler source directory and compile the MIPS Assembly into machine code:

``` bash
cd src/assembler
g++ asm.cc dfa.h -o asm
./asm < output.asm > ./output/bin.card
```

## Quick Example:
For Compiler, the following WLP4 (C) code gets compiled into MIPS code.

``` C
// Source Code
int main(int a, int b) {
    return a + b;
}
```

``` MIPS
; Compiled MIPS Code
.import print
.import init
.import new
.import delete
lis $4
.word 4
lis $11
.word 1
sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4
sub $29, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lis $2
.word 0
lis $3
.word init
jalr $3
add $30, $30, $4
lw $31, -4($30)
lw $3, 8($29);
sw $3, -4($30)
sub $30, $30, $4
lw $3, 4($29);
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
jr $31

```

For assembler, the following MIPS Assembly code gets translated into machine code.

```MIPS
; MIPS Source Code
lis $20
.word 0xffff000c ; output address
lis $21
.word 0xffff0004 ; input address
lis $10
.word 0x0A       ; newline character
lis $11
.word 10
lis $12
.word 48
lis $13
.word 0xffffffff

lw $3,  0($21)   ; read byte
slt $5, $3, $12
beq $5, $0, cont
input:
lw $3,  0($21)   ; read byte
cont:
beq $3, $10, finish
sub $3, $3, $12
mult $4, $11
mflo $4
add $4, $4, $3
beq $3, $3, input

finish:
beq $5, $0, 2
mult $4, $13
mflo $4
sw $4,  0($20)
jr $31

```
```
00000000000000001010000000010100
11111111111111110000000000001100
00000000000000001010100000010100
11111111111111110000000000000100
00000000000000000101000000010100
00000000000000000000000000001010
00000000000000000101100000010100
00000000000000000000000000001010
00000000000000000110000000010100
00000000000000000000000000110000
00000000000000000110100000010100
11111111111111111111111111111111
10001110101000110000000000000000
00000000011011000010100000101010
00010000101000000000000000000001
10001110101000110000000000000000
00010000011010100000000000000101
00000000011011000001100000100010
00000000100010110000000000011000
00000000000000000010000000010010
00000000100000110010000000100000
00010000011000111111111111111001
00010000101000000000000000000010
00000000100011010000000000011000
00000000000000000010000000010010
10101110100001000000000000000000
00000011111000000000000000001000
```



## Design Overview

The compiler progresses several stages to transform WLP4 (C) code into MIPS Assembly:

### Tokenization (Scanning)

Tokenization involves breaking down the source code into tokens using Simpilfied Maximal Munch Algorithm. This approach consumes the input to form tokens, based on predefined lexical rules.

To run scanning:
```bash
g++ wlp4scan.cc -o wlp4scan
./wlp4scan < test.wlp4
```

### Syntax Analysis (Parsing)

Parsing interprets the tokens against a Context Free Grammar (CFG) using Pushdown Automata, which accommodates the nesting structures like parentheses that regular languages cannot easily express. Our compiler implements an LR(1) parser, a Bottom-Up Parsing technique, to generate a parse tree that reflects the program's structure.

To run parsing:
```bash
g++ wlp4parse-prescanned.cc wlp4data.cc -o wlp4parse
./wlp4scan < test.wlp4 | ./wlp4parse 
```

If you would like to view the parse tree better, in wlp4parse-prescanned.cc, comment out line 129 and un-comment line 128. For source code of printing a parsing tree nicely, checkout `/src/compiler/treeprint.cc`.

If you would like to checkout the DFA (Deterministic Finite Automata) for WLP4, you can checkout `/src/compiler/wlp4data.cc`. If you would like to check this file in a better format, execute the following command in `src/compiler`.

```bash
g++ dfa-print.cc -o dfa-print
./dfa-print < wlp4data.cc
```

### Semantic Analysis

After parsing, the compiler verifies that the code adheres to WLP4's context-sensitive rules, such as unique variable names and declared-before-use variables. This step involves traversing the parse tree and checking each rule. This phase checks the WLP4 code for type consistency, ensuring variables are correctly identified as either integers or integer pointers.

To run semantic analysis:
``` bash
g++ wlp4type-prescanned.cc wlp4data.cc -o wlp4type
./wlp4scan < test.wlp4 | ./wlp4type

```

Semantic analysis checks if your program's type system is valid or not. It would not output anything if your program is correct but would output error to the console if there is an error related to type system.

### Code Generation

In the final phase, the compiler generates MIPS Assembly code from the WLP4 code, aligning with the syntax and semantics of the source language.

To run code generation:
```bash
g++ wlp4gen-prescanned.cc wlp4data.cc -o wlp4gen
./wlp4scan < test.wlp4 | ./wlp4gen
```
Key functions within the code generation phase include:

- `setUpWain`: Establishes the environment for the 'wain' function, which is the entry point of the WLP4 program.
- `constructProcedure`: Generates MIPS assembly for each procedure defined in the WLP4 program, handling function prologues and epilogues.
- `process`: Traverses the parse tree and emits corresponding MIPS instructions for each type of node, such as expressions, statements, and tests.


For instance, arithmetic expressions like addition, the code manages the stack and use the appropriate instructions:

``` C++
// Handle addition for 'expr PLUS term'
if(root->getChild("expr",1)->type == "INT" && root->getChild("term",1)->type == "INT"){
    process(root->getChild("expr", 1), offsetTable);
    push(3);
    process(root->getChild("term", 1), offsetTable);
    pop(5);
    Add(3,5,3);
}

```


