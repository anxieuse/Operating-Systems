#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>

#define INBUFSIZE 300 // Buffer size

int check_vowel(char t)
{
    t = tolower(t);
    if (t == 'a' || t == 'e' || t == 'i' || t == 'o' || t == 'u' || t == 'y')
        return 1;

    return 0;
}

int main(void)
{
    char buff[INBUFSIZE], res[INBUFSIZE];
    pid_t mypid;

    mypid = getpid(); // Process-ID
    printf("1st Child %d: Started!\n", mypid);

    while (fgets(buff, INBUFSIZE, stdin))
    {
        printf("I got new line: %s", buff);
        int d = 0;
        for (int i = 0; i < strlen(buff); ++i)
        {
            if (!check_vowel(buff[i]))
            {
                res[d++] = buff[i];
            }
        }
        res[d] = '\0';
        printf("Proceeded line: %s", res);
    }
    printf("1st Child %d: I'm Done!\n", mypid);

    return 0;
}
