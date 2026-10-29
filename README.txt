LEXICAL ANALYZER IN C - IMPROVED BEGINNER VERSION

FILES
- main.c: opens the input file and prints tokens and their categories.
- lexer.c: scans source text and identifies tokens.
- lexer.h: token types and function declarations.
- input.c: sample source file for testing.

COMPILE
Run these commands from this folder:

  gcc -Wall -Wextra -Wpedantic -std=c11 main.c lexer.c -o lexer

Linux/macOS:
  ./lexer input.c

Windows (MinGW GCC):
  lexer.exe input.c

If no filename is supplied, the program reads input.c from the current folder.

SUPPORTED FEATURES
- Common C keywords and identifiers
- Integer and simple decimal constants, including forms such as .5 and 2.5f
- Single-character and common multi-character operators (++, ==, !=, <=, >=, &&, ||, +=, <<=, etc.)
- Punctuation/special characters such as ; , ( ) { } [ ]
- String literals and character literals with escaped characters
- Skips whitespace, // single-line comments, and /* multi-line comments */
- Reports unrecognized or malformed tokens as UNKNOWN
- Checks whether the input file can be opened and reports an error if not

EXAMPLE
For source code: int a = 10;
The lexer reports int as KEYWORD, a as IDENTIFIER, = as OPERATOR,
10 as CONSTANT, and ; as SPECIAL_CHARACTER.

LIMITATIONS
This is an educational lexer, not a complete implementation of the C standard.
It does not fully support preprocessing, hexadecimal/octal/binary constants,
scientific notation, universal character names, or every C literal suffix.
It recognizes tokens; it does not parse or validate the full C program grammar.
