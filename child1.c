#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#define INBUFSIZE   300  // Buffer size

int main(void)
{
    char    buff[INBUFSIZE];
    pid_t   mypid;
    char    *eol;

    mypid = getpid();   // Process-ID
    printf("1st Child %d: Started!\n", mypid);

    // For each line read, write one tostdout.
    while (fgets(buff, INBUFSIZE, stdin)) {
        // Chop off ending EOL
        if ((eol = rindex(buff, '\n')) != NULL)
            *eol = '\0';
        // Write
        printf("I got new line: %s\n", buff);
    }
    printf("1st Child %d: I'm Done!\n", mypid);

    return 0;
}