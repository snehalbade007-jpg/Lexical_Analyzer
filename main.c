#include <stdio.h>
#include "lexer.h"

int main(int argc, char *argv[])
{
    const char *filename = (argc > 1) ? argv[1] : "input.c";
    Token token;

    if (!initializeLexer(filename)) {
        fprintf(stderr, "Error: Cannot open '%s'.\n", filename);
        return 1;
    }

    printf("Lexical Analyzer Output\n");
    printf("-----------------------\n");

    token = getNextToken();
    while (token.type != END_OF_FILE) {
        printf("Token: %-12s Type: %s\n",
               token.lexeme, tokenTypeName(token.type));
        token = getNextToken();
    }

    closeLexer();
    return 0;
}
