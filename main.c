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
void read_entire_file(Buffer *buffer, char *file_path) {
	FILE *f = fopen(file_path, "r");

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
    
    char tmp[255];
    for (size_t i = 0; i < lines; ++i) {
        fgets(tmp, 255, f);

        buffer_append_line(buffer, tmp);
    }

	fclose(f);
}

int main() {
    Buffer buff = {0};
    read_entire_file(&buff, "sample.md");

    printf("%s", buff.line[0]);
    return 0;
}

