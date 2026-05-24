# DSA-ASSIGNMENT-1
Expression Conversion and Evaluation
# DSA Assignment 1 — Expression Conversion and Evaluation

## Overview

This program processes arithmetic expressions in three stages:
1. **Tokenization** — breaks input into tokens (numbers, variables, operators, brackets)
2. **Infix → Postfix** — converts using the Shunting-Yard algorithm (stack-based)
3. **Postfix Evaluation** — evaluates the postfix expression using a stack

## Compilation

```bash
g++ -std=c++17 -Wall -o expression main.cpp
```

## Usage

```bash
./expression
```

Type an infix expression and press Enter. The program will:
- Print the postfix form to **stderr**
- Prompt for variable values on **stderr**
- Print **only** the final result to **stdout**

## Example

```
Input (stdin):    a + b * (c + 2)
Postfix (stderr): a b c 2 + * +
Prompts (stderr): Enter value for a: 3  Enter value for b: 5  Enter value for c: 2
Result (stdout):  23
```

## Supported Syntax

| Feature | Detail |
|---------|--------|
| Operators | `+`, `-`, `*`, `/` (binary), `-` (unary) |
| Brackets | `()`, `[]`, `{}` — must match correctly |
| Integers | e.g. `42`, `0`, `100` |
| Variables | C++ identifier rules (letters/digits/underscore, not starting with digit) |

**Not allowed:** implicit multiplication like `a(b+c)` or `(a+b)(c+d)`, empty brackets `()`, mismatched brackets `(a+b]`

## Exit Codes

| Code | Meaning | Example |
|------|---------|---------|
| 0 | Success | `a + b` evaluated correctly |
| 1 | Syntax error | `a + * b`, `(a+b)(c+d)`, `()` |
| 2 | Runtime error | division overflow, bad variable input |
| 3 | Logical error | division by zero |

## Output Rules

Per assignment specification:
- All prompts and intermediate output (including postfix) → **stderr**
- Only the final numeric result → **stdout**

This ensures automated checkers reading stdout see exactly one line: the answer.
