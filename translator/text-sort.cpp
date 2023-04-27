#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>

#include "text-sort.h"
#include "common.h"

int TextReader (const char* file, struct poem* text, const char* mode)
{
    FILE* fp = fopen(file, "r");

    assert(fp != NULL);

    if (fp == NULL)
    {
        return FILERR;
    }

    (*text).size = FileSize(fp);
    (*text).ptr = (char*) calloc((*text).size + 1, sizeof(char));

    assert((*text).ptr != NULL);

    if ((*text).ptr == NULL)
    {
        return MEMERR;
    }

    if (fread((*text).ptr, 1, (*text).size, fp) != (*text).size)
        return INPUTERR;

    fclose(fp);

    return NOERR;
}

long FileSize(FILE *fp)
{
    assert(fp != NULL);

    struct stat buf;
    fstat(fileno(fp), &buf);
//    fseek(fp, 0, SEEK_END);
//    long size = ftell(fp);
//    rewind(fp);
    return buf.st_size;
}

int LineCounter(char* text, long size)
{
    assert(text != NULL);

    int number_of_lines = 0;

    for(int i = 0; i < size; i++)
    {
        if (*(text + i) == '\n')
        {
            if (*(text + i - 1) != '\n')
                number_of_lines++;
        }
    }

    return number_of_lines;
}

int LinesSeparator(struct poem* text, const char end)
{
    (*text).nlines = LineCounter((*text).ptr, (*text).size);

    (*text).Strings = (struct line*) calloc((*text).nlines, sizeof(struct line));

    assert((*text).ptr != NULL);
    assert((*text).Strings != NULL);

    if ((*text).Strings == NULL)
    {
        return MEMERR;
    }

    int counter = 1;
    char* prev = (*text).ptr;

    for (int i = 0; i < (*text).size && counter < (*text).nlines + 1; i++)
    {
        if (*((*text).ptr + i) == '\n')
        {
            *((*text).ptr + i) = '\0';
            if (*((*text).ptr + i - 1) != '\0')
            {
                ((*text).Strings)[counter-1].length = (*text).ptr + i - prev;
                ((*text).Strings)[counter-1].ptr = prev + 1;
                prev = (*text).ptr + i;
                counter++;
            }
            else
                prev = (*text).ptr + i;
        }
    }

    return NOERR;
}

void my_qsort_r(void* base, int nmemb, size_t size, int (*CompFunc)(const void*, const void*, void*), void* arg)
{
    assert(base != NULL);

    void* left = base;

    if (nmemb == 1 || nmemb == 0)
        return;

    swap(left, (left + size * (int)(nmemb/2)), size);

    void* current = left;
    int counter = 0;

    for (int i = 1; i < nmemb; i++)
    {
        if ((*CompFunc) (base + i * size, base, arg) < 0)
        {
            current += size;
            swap(current, base + i * size, size);
            counter++;
        }
    }

    swap(left, current, size);

    current += size;

    my_qsort_r(base, counter, size, CompFunc, arg);
    my_qsort_r(current, nmemb - counter - 1, size, CompFunc, arg);
}

void swap(void* elem1ptr, void* elem2ptr, size_t size)
{
    assert(elem1ptr != NULL);
    assert(elem2ptr != NULL);

    char* elem1 = (char*) elem1ptr;
    char* elem2 = (char*) elem2ptr;

    for (int i = 0; i < size; i++)
    {
        char temp = *elem1;
        *elem1 = *elem2;
        *elem2 = temp;
        elem1++;
        elem2++;
    }
}

int cmp(const void* struct1ptr, const void* struct2ptr, void* arg)
{
    assert(struct1ptr != NULL);
    assert(struct2ptr != NULL);

    const struct line struct1 = *(const struct line*) struct1ptr;
    const struct line struct2 = *(const struct line*) struct2ptr;

    const char* str1 = struct1.ptr;
    const char* str2 = struct2.ptr;
    const char step = *(char*) arg;

    if (step == -1)
    {
        str1 += struct1.length;
        str2 += struct2.length;
    }

    while(!(isalpha(*str1)))
        str1 += step;

    while(!(isalpha(*str2)))
        str2 += step;

    for( ; tolower(*str1) == tolower(*str2); str1 += step, str2 += step)
    {
        if (*str1 == '\0')
            return 0;
    }
    if (tolower(*str1) > tolower(*str2))
        return 1;
    else
        return -1;
}

int FileWrite (const char* file, struct line Strings[], int nlines)
{
    FILE* output = fopen(file, "w");

    assert(output != NULL);
    assert(Strings != NULL);

    if (output == NULL)
    {
        return FILERR;
    }

    for (int i = 0; i < nlines; i++)
        fprintf(output, "%s\n", Strings[i].ptr);

    fclose(output);

    return NOERR;
}

int OriginWrite(const char* file, char* text, long size)
{
    FILE* output = fopen(file, "w");

    assert(output != NULL);
    assert(text != NULL);

    if (output == NULL)
    {
        return FILERR;
    }

    for (int i = 0; i < size; i++)
    {
        if (*(text + i) == '\0')
            putc('\n', output);
        else
            putc(*(text + i), output);
    }

    fclose(output);

    return NOERR;
}
