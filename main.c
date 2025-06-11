#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char **line;
    size_t *line_len;
    size_t count;
} Buffer;

// TODO: Leaks memory. Shouldn't be much of a problem for now, but do not neglect
void buffer_append_line(Buffer *buffer, char *line) {
    size_t len = strlen(line);

    buffer->line = realloc(buffer->line, buffer->count + 1);
    buffer->line[buffer->count] = malloc(sizeof(char) * len);
    buffer->line_len = realloc(buffer->line_len, buffer->count + 1);
    if (buffer->line == NULL || buffer->line[buffer->count] == NULL) {
        fprintf(stderr, "ERROR: Could not allocate `Buffer`\n");
    }

    memcpy(buffer->line[buffer->count], line, len);
    buffer->line_len[buffer->count] = len;
    buffer->count += 1;
}

// TODO: Fix newline going into buffer lines
bool read_entire_file(Buffer *buffer, char *file_path) {
	FILE *f = fopen(file_path, "r");
    if (f == NULL) return false;

    size_t lines = 0;
    char cursor;
    
    // count lines
    // possibly use as pre-processor
    for (;;) {
        cursor = fgetc(f);
        if (cursor == EOF) break;
        if (cursor == '\n') lines += 1;
    }
    rewind(f);
    
    char tmp[256];
    for (size_t i = 0; i < lines; ++i) {
        fgets(tmp, 256, f);

        buffer_append_line(buffer, tmp);
    }

	fclose(f);
    return true;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: Invalid number of arguments\n");
        fprintf(stderr, "Usage: %s <path-to-file>\n", argv[0]);
        return 1;
    }

    char *file_path = argv[1];

    Buffer buff = {0};
    if (!read_entire_file(&buff, file_path)) {
        fprintf(stderr, "ERROR: Could not find file: %s\n", file_path);
        fprintf(stderr, "Usage: %s <path-to-file>\n", argv[0]);
        return 1;
    }
    
    for (int i = 0; i < buff.count; ++i) {
        printf("%s", buff.line[i]);
    }
    return 0;
}

