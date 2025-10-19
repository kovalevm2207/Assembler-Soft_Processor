#include "ReadFile.h"

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
    int i = 0;

    while (text[i] != '\0') {

        while (text[i] != '\0' && text[i] != '\n' && isspace(text[i])) i++;

        if (text[i] == '\n') {
            i++;
            continue;
        }
        else if (text[i] == '#') {
            while (text[i] != '\0' && text[i] != '\n') i++;
            continue;
        }
        else if (text[i] == '\0') continue;
        else {
            linenum++;
            while(text[i] != '\0' && text[i] != '\n') i++;
        }
    }

    return linenum;
}


void get_lines(line* lines, char* text)
{
    assert (lines != 0);
    assert (text != 0);

    size_t ptr_shift = 0;
    size_t count = 0;
    int i = 0;

    while (text[i] != '\0') {
        while (text[i] != '\0' && text[i] != '\n' && isspace((unsigned char) text[i])) i++;
        ptr_shift = i;

        if (text[i] == '\n') {
            i++;
            continue;
        }
        else if (text[i] == '#') {
            while (text[i] != '\0' && text[i] != '\n') i++;
            i++;
            continue;
        }
        else if (text[i] == '\0') continue;
        else {
            lines[count].ptr = text + ptr_shift;
            while(text[i] != '\0' && text[i] != '\n' && text[i] != '#') i++;

            if (text[i] == '#') {
                text[i] = '\0';
                i++;
                lines[count].length = i - ptr_shift + 1;
                while(text[i] != '\0' && text[i] != '\n') i++;
                if (text[i] == '\n') i++;
                count++;
            }
            else if (text[i] == '\n') {
                text[i] = '\0';
                lines[count].length = i - ptr_shift + 1;
                i++;
                count++;
            }
            else if (text[i] == '\0') {
                lines[count].length = i - ptr_shift + 1;
                count++;
            }
        }
    }
}
