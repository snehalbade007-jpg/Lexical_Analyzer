#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "inline", "int", "long", "register", "restrict", "return",
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
    "union", "unsigned", "void", "volatile", "while"
};

static const char *operators[] = {
    ">>=", "<<=", "...", "++", "--", "->", "&&", "||", "<=", ">=",
    "==", "!=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
    "<<", ">>", "##", "+", "-", "*", "/", "%", "=", "!", "<", ">",
    "&", "|", "^", "~", "?", ":", ".", "#"
};
static const char *specialCharacters = ",;{}()[]";
static FILE *inputFile = NULL;

int initializeLexer(const char *filename)
{
    if (inputFile != NULL) {
        fclose(inputFile);
        inputFile = NULL;
    }
    inputFile = fopen(filename, "r");
    return inputFile != NULL;
}

void closeLexer(void)
{
    if (inputFile != NULL) {
        fclose(inputFile);
        inputFile = NULL;
    }
}

int isKeyword(const char *str)
{
    size_t i;
    for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        if (strcmp(str, keywords[i]) == 0) return 1;
    return 0;
}

int isOperator(const char *str)
{
    size_t i;
    for (i = 0; i < sizeof(operators) / sizeof(operators[0]); i++)
        if (strcmp(str, operators[i]) == 0) return 1;
    return 0;
}

int isSpecialCharacter(char ch)
{
    return strchr(specialCharacters, ch) != NULL;
}

int isConstant(const char *str)
{
    size_t i = 0;
    int digitsBefore = 0, digitsAfter = 0;
    if (str == NULL || str[0] == '\0') return 0;
    while (isdigit((unsigned char)str[i])) { digitsBefore = 1; i++; }
    if (str[i] == '.') {
        i++;
        while (isdigit((unsigned char)str[i])) { digitsAfter = 1; i++; }
    }
    if (!digitsBefore && !digitsAfter) return 0;
    if (str[i] == 'f' || str[i] == 'F' || str[i] == 'l' || str[i] == 'L') i++;
    return str[i] == '\0';
}

int isIdentifier(const char *str)
{
    size_t i;
    if (str == NULL || !(isalpha((unsigned char)str[0]) || str[0] == '_')) return 0;
    for (i = 1; str[i] != '\0'; i++)
        if (!(isalnum((unsigned char)str[i]) || str[i] == '_')) return 0;
    return 1;
}

void categorizeToken(Token *token)
{
    if (isKeyword(token->lexeme)) token->type = KEYWORD;
    else if (isConstant(token->lexeme)) token->type = CONSTANT;
    else if (isIdentifier(token->lexeme)) token->type = IDENTIFIER;
    else if (isOperator(token->lexeme)) token->type = OPERATOR;
    else token->type = UNKNOWN;
}

const char *tokenTypeName(TokenType type)
{
    switch (type) {
        case KEYWORD: return "KEYWORD";
        case OPERATOR: return "OPERATOR";
        case SPECIAL_CHARACTER: return "SPECIAL_CHARACTER";
        case CONSTANT: return "CONSTANT";
        case STRING_LITERAL: return "STRING_LITERAL";
        case CHARACTER_LITERAL: return "CHARACTER_LITERAL";
        case IDENTIFIER: return "IDENTIFIER";
        case UNKNOWN: return "UNKNOWN";
        case END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
}

static Token makeToken(const char *text, TokenType type)
{
    Token token;
    snprintf(token.lexeme, sizeof(token.lexeme), "%s", text);
    token.type = type;
    return token;
}

static int appendChar(char *text, size_t *length, int ch)
{
    if (*length + 1 >= MAX_TOKEN_SIZE) return 0;
    text[(*length)++] = (char)ch;
    text[*length] = '\0';
    return 1;
}

/* Return the next token, skipping whitespace and both C comment styles. */
Token getNextToken(void)
{
    int ch;
    char text[MAX_TOKEN_SIZE] = {0};
    size_t length = 0;

    if (inputFile == NULL) return makeToken("", END_OF_FILE);

    for (;;) {
        ch = fgetc(inputFile);
        while (ch != EOF && isspace((unsigned char)ch)) ch = fgetc(inputFile);
        if (ch != '/') break;
        {
            int next = fgetc(inputFile);
            if (next == '/') {
                while ((ch = fgetc(inputFile)) != EOF && ch != '\n') { }
                continue;
            }
            if (next == '*') {
                int prev = 0;
                while ((ch = fgetc(inputFile)) != EOF) {
                    if (prev == '*' && ch == '/') break;
                    prev = ch;
                }
                continue;
            }
            if (next != EOF) ungetc(next, inputFile);
            break;
        }
    }

    if (ch == EOF) return makeToken("", END_OF_FILE);

    if (isalpha((unsigned char)ch) || ch == '_') {
        do {
            if (!appendChar(text, &length, ch)) break;
            ch = fgetc(inputFile);
        } while (ch != EOF && (isalnum((unsigned char)ch) || ch == '_'));
        if (ch != EOF) ungetc(ch, inputFile);
        return makeToken(text, isKeyword(text) ? KEYWORD : IDENTIFIER);
    }

    if (isdigit((unsigned char)ch) || ch == '.') {
        int leadingDot = 0, dotSeen = 0;
        if (ch == '.') {
            int next = fgetc(inputFile);
            if (next == EOF || !isdigit((unsigned char)next)) {
                if (next != EOF) ungetc(next, inputFile);
                text[0] = '.'; text[1] = '\0';
                return isOperator(text) ? makeToken(text, OPERATOR) : makeToken(text, UNKNOWN);
            }
            leadingDot = 1;
            dotSeen = 1;
            appendChar(text, &length, '.');
            ch = next;
        }
        (void)leadingDot;
        do {
            if (!appendChar(text, &length, ch)) break;
            ch = fgetc(inputFile);
            if (ch == '.' && !dotSeen) dotSeen = 1;
            else if (ch == '.' && dotSeen) {
                appendChar(text, &length, ch);
                while ((ch = fgetc(inputFile)) != EOF && (isdigit((unsigned char)ch) || ch == '.'))
                    appendChar(text, &length, ch);
                if (ch != EOF) ungetc(ch, inputFile);
                return makeToken(text, UNKNOWN);
            }
        } while (ch != EOF && (isdigit((unsigned char)ch) || ch == '.' || ch == 'f' || ch == 'F' || ch == 'l' || ch == 'L'));
        if (ch != EOF) ungetc(ch, inputFile);
        return makeToken(text, isConstant(text) ? CONSTANT : UNKNOWN);
    }

    if (ch == '"' || ch == '\'') {
        int quote = ch, escaped = 0, closed = 0;
        appendChar(text, &length, ch);
        while ((ch = fgetc(inputFile)) != EOF) {
            appendChar(text, &length, ch);
            if (!escaped && ch == quote) { closed = 1; break; }
            if (!escaped && ch == '\\') escaped = 1;
            else escaped = 0;
            if (ch == '\n' && !closed) break;
        }
        if (quote == '\'' && length < 3) closed = 0;
        if (!closed) return makeToken(text, UNKNOWN);
        return makeToken(text, quote == '"' ? STRING_LITERAL : CHARACTER_LITERAL);
    }

    if (isSpecialCharacter((char)ch)) {
        text[0] = (char)ch; text[1] = '\0';
        return makeToken(text, SPECIAL_CHARACTER);
    }

    /* Maximal munch: choose the longest operator beginning at this character. */
    {
        char candidate[4] = {(char)ch, '\0', '\0', '\0'};
        int c2 = fgetc(inputFile), c3 = EOF;
        size_t i, bestLen = 0;
        if (c2 != EOF) { candidate[1] = (char)c2; c3 = fgetc(inputFile); }
        if (c3 != EOF) candidate[2] = (char)c3;
        for (i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
            size_t opLen = strlen(operators[i]);
            if (opLen > bestLen && strncmp(candidate, operators[i], opLen) == 0)
                bestLen = opLen;
        }
        if (bestLen == 3) {
            candidate[3] = '\0';
            return makeToken(candidate, OPERATOR);
        }
        if (bestLen == 2) {
            if (c3 != EOF) (void)fseek(inputFile, -1L, SEEK_CUR);
            candidate[2] = '\0';
            return makeToken(candidate, OPERATOR);
        }
        /* Restore lookahead bytes for the next token. The input is a regular source file. */
        if (c2 != EOF) (void)fseek(inputFile, c3 != EOF ? -2L : -1L, SEEK_CUR);
        candidate[1] = '\0';
        return isOperator(candidate) ? makeToken(candidate, OPERATOR) : makeToken(candidate, UNKNOWN);
    }
}
