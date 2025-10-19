#ifndef READ_FILE
#define READ_FILE

#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

struct line
{
    size_t length;
    char* ptr;
};

size_t find_file_size(const char* file_name);
size_t count_lines(char* text);
void get_lines(line*  lines, char* text);

#endif // READ_FILE
