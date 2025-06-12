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
} Buffer;

enum Token_Type {
    HASH = 0,
    ASTERISK,
    MINUS,
    PLUS,
    STRING
};

typedef struct {
    enum Token_Type type;
    char *within;
} Token;

typedef struct {
    Token *tokens;
    size_t count;
} Lexer;

// TODO: Leaks memory. Shouldn't be much of a problem for now, but do not neglect
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

	fclose(f);
    return true;
}

// typedef struct {
//     enum Token_Type type;
//     char *within;
// } Token;
// 
// typedef struct {
//     Token *tokens;
//     size_t count;
// } Lexer;

void mdlexer_init(Lexer *lexer) {
    lexer->tokens = malloc(sizeof(Token));
}

void mdlexer_append_token(Lexer *lexer, enum Token_Type type, char *within) {

}

void mdlexer_parse_buffer(Lexer *lexer, Buffer *buffer) {
    for (size_t line = 0; line < buffer->count; ++line) {
        char *line_start = buffer->lines[line].content;
        size_t line_length = buffer->lines[line].length;

        size_t cursor = 0;
        
        while (cursor < line_length) {
            switch (line_start[cursor]) {
                case '#':
                    for (size_t i = 1; i < (line_length - cursor); ++i) {
                        if (line_start[cursor + i] == ' ') {
                            printf("line %zu:%zu: HEADER %zu\n", line, cursor + i, i);
                            cursor += i;
                            break;
                        }
                    }
                    break;
                default:
            }
            cursor += 1;
        }
    }
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
    //for (int i = 0; i < 

    return 0;
}

