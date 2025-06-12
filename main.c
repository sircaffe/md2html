#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char *content;
    size_t length;
} Line;

typedef struct {
    Line *lines;
    size_t count;
    char *file_path;
} Buffer;

enum Token_Type {
    STRING = 0,
    H1,
    H2,
    H3,
    H4,
    H5,
    H6,
    ASTERISK,
    MINUS,
    PLUS,
};

typedef struct {
    enum Token_Type type;
    char *within;
    size_t count;
} Token;

typedef struct {
    Token *tokens;
} Lexer;

// TODO: fix memory leaks
void buffer_append_line(Buffer *buffer, char *line) {
    size_t len = strlen(line);

    buffer->lines = realloc(buffer->lines, sizeof(Line) * (buffer->count + 1));
    buffer->lines[buffer->count].content = malloc(sizeof(char) * len);

    if (buffer->lines == NULL || buffer->lines[buffer->count].content == NULL) {
        fprintf(stderr, "ERROR: Could not allocate `Buffer`\n");
        exit(1);
    }

    memcpy(buffer->lines[buffer->count].content, line, len);
    buffer->lines[buffer->count].length = len;
    buffer->count += 1;
}

// TODO: Fix newline going into buffer lines
bool read_entire_file(Buffer *buffer, char *file_path) {
	FILE *f = fopen(file_path, "r");
    if (f == NULL) return false;

    size_t lines = 0;
    char cursor;
    const size_t MAX_LINE_LEN = 256;
    
    // count lines
    // possibly use as pre-processor
    for (;;) {
        cursor = fgetc(f);
        if (cursor == EOF) break;
        if (cursor == '\n') lines += 1;
    }
    rewind(f);
    
    char tmp[MAX_LINE_LEN];
    if (lines > MAX_LINE_LEN) {
        fprintf(stderr, "ERROR: Exceded max line length: %zu bytes\n", MAX_LINE_LEN);
    }
    for (size_t i = 0; i < lines; ++i) {
        fgets(tmp, MAX_LINE_LEN, f);

        buffer_append_line(buffer, tmp);
    }
    buffer->file_path = file_path;

	fclose(f);
    return true;
}

void mdlexer_init(Lexer *lexer) {
    lexer->tokens = malloc(sizeof(Token));
    lexer->tokens->count = 0;
}

void mdlexer_append_token(Lexer *lexer, enum Token_Type type, char *within) {
    lexer->tokens = realloc(lexer->tokens, (lexer->tokens->count * sizeof(Token)) + sizeof(Token));
    if (lexer->tokens == NULL) {
        fprintf(stderr, "ERROR: Could not append token to lexer\n");
        exit(1);
    }
    lexer->tokens[lexer->tokens->count].type = type;
    lexer->tokens[lexer->tokens->count].within = malloc(sizeof(char) * strlen(within));
    strcpy(lexer->tokens[lexer->tokens->count].within, within);
}


void mdlexer_parse_buffer(Lexer *lexer, Buffer *buffer) {
    printf("<!DOCTYPE html>\n");
    printf("<html>\n");
    printf("    <head>\n");
    printf("    </head>\n");
    printf("    </body>\n");

    for (size_t line = 0; line < buffer->count; ++line) {
        char *line_start = buffer->lines[line].content;
        size_t line_length = buffer->lines[line].length;

        size_t cursor = 0;
       
        while (cursor < line_length) {
            switch (line_start[cursor]) {
                case '#':
                    for (size_t i = 1; i < (line_length - cursor); ++i) {
                        if (line_start[cursor + i] == ' ') {
                            // TODO: refactor when newlines into buffer bug has been fixed
                            size_t within_length = line_length - (cursor + i);
                            char *within = &line_start[cursor + i + 1];
                            within[within_length - 2] = '\0';

                            printf("        <h%zu>%s</h%zu>\n", i, within, i);

                            mdlexer_append_token(lexer, i, within);
                            lexer->tokens->count += 1;
                            cursor += i;
                            break;
                        }
                    }
                    break;
                case '-':
                case '+':
                case '*':
                    printf("%s:%zu:%zu: error: `%c` not yet supported\n", buffer->file_path, line + 1, cursor + 1, line_start[cursor]);
                    break;
                default:

            }
            cursor += 1;
        }
    }
    printf("    </body>\n");
    printf("</html>\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: Invalid number of arguments\n");
        fprintf(stderr, "Usage: %s <path-to-file>\n", argv[0]);
        return 1;
    }

    char *file_path = argv[1];

    Buffer buffer = {0};

    if (!read_entire_file(&buffer, file_path)) {
        fprintf(stderr, "ERROR: Could not find file: %s\n", file_path);
        fprintf(stderr, "Usage: %s <path-to-file>\n", argv[0]);
        return 1;
    }

    Lexer lexer = {0};
    mdlexer_init(&lexer);
    mdlexer_parse_buffer(&lexer, &buffer);
    
    // PRINT TOKENS FOR DEBUGGING
    //for (size_t i = 0; i < lexer.tokens->count; ++i) {
    //    printf("TOKEN: H%u = %s\n", lexer.tokens[i].type, lexer.tokens[i].within);
    //}

    return 0;
}

