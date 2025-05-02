# ChomLisp
## Overview

ChomLisp is a lightweight Lisp interpreter implemented in C++. It is designed to tokenize, parse, and evaluate Lisp-like expressions efficiently.

## Features

- **Tokenizer**: Breaks down the input Lisp code into tokens, identifying numbers, strings, identifiers, and parentheses.
- **Parser**: Converts tokens into an abstract syntax tree (AST) for evaluation.
- **Evaluator**: Executes the AST and computes the results of the Lisp expressions.

## File Descriptions

### `all.cpp`
This file contains the entire implementation of the ChomLisp interpreter, including:

- **Tokenizer**: Functions like `tokenize` and `splitString` handle lexical analysis by breaking down the input code into tokens.
- **Parser**: Functions like `parse` and `parse_expr` construct an abstract syntax tree (AST) from the tokens.
- **Evaluator**: Functions like `eval` and `eval_list` execute the AST and return the computed results.

## Getting Started

1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/ChomLisp.git
    cd ChomLisp
    ```

2. Build the project:
    ```bash
    g++ all.cpp -o chomlisp
    ```

3. Run the interpreter:
    ```bash
    ./chomlisp
    ```

## Example Usage

Input:
```lisp
((define x 5) (+ x 3))