#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>

struct line
{
    size_t length;
    char* ptr;
};

size_t find_file_size(const char* file_name);
size_t count_lines(char* text);
void get_lines(line*  lines, char* text);


size_t find_file_size(const char* file_name)
{
    assert (file_name != 0);

    struct stat file_info = {};
    stat(file_name, &file_info);

    assert(file_info.st_size != 0);

    return file_info.st_size;
}


size_t count_lines(char* text)
{
    assert (text != 0);

    size_t linenum = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            linenum++;
        }
    }
    assert(linenum != 0);

    return linenum;
}


void get_lines(line* lines, char* text)
{
    assert (lines != 0);
    assert (text != 0);

    size_t count = 0, ptr_shift = 0;

    for (size_t letter = 0; text[letter] != '\0'; letter++) {
        //printf("%c ", text[letter]);
        if (text[letter] == '#') {
            text[letter] = '\0';
            letter++;
            while(text[letter] != '\n') {
                letter++;
            }
            lines[count].ptr = text + ptr_shift;
            lines[count].length = (letter + 1) - ptr_shift;
            ptr_shift = letter + 1;
            count++;
        }
        else if (text[letter] == '\n') {
            text[letter] = '\0';
            lines[count].ptr = text + ptr_shift;
            lines[count].length = (letter + 1) - ptr_shift;
            ptr_shift = letter + 1;
            count++;
        }
    }
}
