#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "sys/stat.h"

#define INBUFSIZE 300 // Buffer size

int isVowel(char t)
{
    t = tolower(t);
    if (t == 'a' || t == 'e' || t == 'i' || t == 'o' || t == 'u' || t == 'y')
        return 1;
    return 0;
}

char *ufgetl(char *stream)
{
    unsigned int maxlen = 128, size = 128;
    char *buffer = (char *)malloc(maxlen);

    if (buffer != NULL) /* NULL if malloc() fails */
    {
        int ch = EOF;
        int pos = 0;

        /* Read input one character at a time, resizing the buffer as necessary */
        while ((ch = *(stream + pos)) != '\n' && ch != EOF)
        {
            buffer[pos++] = ch;
            if (pos == size) /* Next character to be inserted needs more memory */
            {
                size = pos + maxlen;
                buffer = (char *)realloc(buffer, size);
            }
        }
        buffer[pos] = '\0'; /* Null-terminate the completed string */
    }
    return buffer;
}

void PrintNoVowels(char *msg, int type) {
    int len = strlen(msg);
    if (len > 10 && type || len <= 10 && !type) {
        for (int i = 0; i < len; ++i)
            if (!isVowel(msg[i]))
                printf("%c", msg[i]);
        printf("\n");
    }
    return;
}

int main(int argc, char *argv[])
{
    int type = atoi(argv[1]);

    int fd = open("./input.txt", O_RDWR, 0777);
    if (fd < 0) {
        perror("Couldn't open input file");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        perror("Couldn't get the input file size");
        exit(1);
    }

    uint8_t *byte_ptr = mmap(NULL,
                             st.st_size,
                             PROT_READ | PROT_WRITE, MAP_SHARED,
                             fd, 0);
    if (byte_ptr < 0) {
        perror("Couldn't mmap file");
        exit(1);
    }

    int len;
    char *msg;
    int pos = 0;
    while ( (msg = ufgetl(byte_ptr + pos)) && msg[0] != '\0' )
    {
        PrintNoVowels(msg, type);
        pos += strlen(msg);
        free(msg);
        if (byte_ptr[pos] == '\n') pos++;
    }

    if(munmap(byte_ptr, st.st_size) != 0){
        perror("Couldn't munmap ptr");
        exit(1);
    }

    return 0;
}