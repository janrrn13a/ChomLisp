# ChomLisp
## Overview

ChomLisp is a lightweight Lisp interpreter implemented in C++. It is designed to tokenize, parse, and evaluate Lisp-like expressions efficiently. The interpreter supports basic arithmetic operations, conditional expressions, and function definitions.

## Features

- **Tokenizer**: Breaks down the input Lisp code into tokens, identifying numbers, strings, identifiers, and parentheses.
- **Parser**: Converts tokens into an abstract syntax tree (AST) for evaluation.
- **Evaluator**: Executes the AST and computes the results of the Lisp expressions.
- **Built-in Functions**: Includes basic arithmetic operations (`+`, `-`, `*`, `/`), comparison (`>`), and custom functions like `square`.

## File Descriptions

### `all.cpp`
This file contains the entire implementation of the ChomLisp interpreter, including:

- **Tokenizer**: Functions like `tokenize` and `splitString` handle lexical analysis by breaking down the input code into tokens.
- **Parser**: Functions like `parse` and `parse_expr` construct an abstract syntax tree (AST) from the tokens.
- **Evaluator**: Functions like `eval` and `eval_list` execute the AST and return the computed results.
- **Environment**: A map-based environment (`Env`) stores variable bindings and function definitions.

## Getting Started

1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/ChomLisp.git
    cd ChomLisp
    ```

2. Build the project:
    ```bash
    g++ -std=c++11 all.cpp -o chomlisp
    ```

3. Run the interpreter:
    ```bash
    ./chomlisp
    ```

## Example Usage

### Input:
```lisp
(define x 5)
(+ x 3)
(quote (1 2 3))
(if (> x 3) (+ x 2) (- x 2))
(square 4)

#### Output:
==============================
Program 1: (define x 5)
Tokens: ( define x 5 )
[define] x = 5
Result: 5

==============================
Program 2: (+ x 3)
Tokens: ( + x 3 )
[call] + with args: 5 3
Result: 8

==============================
Program 3: (quote (1 2 3))
Tokens: ( quote ( 1 2 3 ) )
[quote] Returning raw expression
Result: 0

==============================
Program 4: (if (> x 3) (+ x 2) (- x 2))
Tokens: ( if ( > x 3 ) ( + x 2 ) ( - x 2 ) )
[call] > with args: 5 3
[call] + with args: 5 2
Result: 7

==============================
Program 5: (square 4)
Tokens: ( square 4 )
[call] square with args: 4
Result: 16