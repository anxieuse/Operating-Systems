#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define INBUFSIZE 300 // Buffer size

int isVowel(char t)
{
    t = tolower(t);
    if (t == 'a' || t == 'e' || t == 'i' || t == 'o' || t == 'u' || t == 'y')
        return 1;
    return 0;
}

int main(void)
{
    char buff[INBUFSIZE], res[INBUFSIZE];

    pid_t mypid = getpid(); 
    printf("1st Child %d: Started!\n", mypid);

    while (fgets(buff, INBUFSIZE, stdin))
    {
        printf("Received line: %s", buff);

        int d = 0;
        for (int i = 0; i < strlen(buff); ++i)
        {
            if (!isVowel(buff[i]))
            {
                res[d++] = buff[i];
            }
        }
        res[d] = '\0';
        
        printf("Processed line: %s", res);
    }
    printf("1st Child %d: I'm Done!\n", mypid);

    return 0;
}