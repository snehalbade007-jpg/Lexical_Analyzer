#ifndef LEXER_H
#define LEXER_H

#define MAX_KEYWORDS 35
#define MAX_TOKEN_SIZE 256

typedef enum {
    KEYWORD,
    OPERATOR,
    SPECIAL_CHARACTER,
    CONSTANT,
    STRING_LITERAL,
    CHARACTER_LITERAL,
    IDENTIFIER,
    UNKNOWN,
    END_OF_FILE
} TokenType;

typedef struct {
    char lexeme[MAX_TOKEN_SIZE];
    TokenType type;
} Token;

int initializeLexer(const char *filename);
Token getNextToken(void);
void categorizeToken(Token *token);
int isKeyword(const char *str);
int isOperator(const char *str);
int isSpecialCharacter(char ch);
int isConstant(const char *str);
int isIdentifier(const char *str);
const char *tokenTypeName(TokenType type);
void closeLexer(void);

#endif
