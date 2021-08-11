#include "types.h"
#include "stat.h"
#include "user.h"
int getcmd(char *buf, int nbuf)
{
    printf(2, "myshell>");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);

    if (buf[0] == 0) 
        return -1;
    return 0;
}
void flushArgs(char *args[32])
{
    for (int i = 0; i < 32; i++)
        args[i] = 0;
}
int extractCmd(char *res, char *args[32])
{
    int index = 0, j = 0;
    char *arg;
    arg = (char *)malloc(sizeof(char) * 512);
    for (int i = 0; i <= strlen(res); i++)
    {
        if (res[i] == ' ' || res[i] == '\n')
        {
            arg[i] = '\0';
            args[index] = arg;
            index++;
            arg = (char *)malloc(sizeof(char) * 512);
            j = 0;
            continue;
        }
        arg[j] = res[i];
        j += 1;
    }
    // args[index]=0;
    return 0;
}
int main(void)
{
    char buf[512], *args[32], *legacy[] = {"ls", "cat", "grep", "echo", "wc"}, legacyLen = 5;
    while (1)
    {
        flushArgs(args);
        getcmd(buf, sizeof(buf));
        extractCmd(buf, args);
        for (int i = 0; i < legacyLen; i++)
            {
                if (!strcmp(legacy[i], args[0]))
                {

                    if (fork() == 0)
                    {
                        if (exec(args[0], args) < 0)
                            printf(1, "error");
                    }
                    else
                        wait(0);
                }
            }
    }
    exit(1);
}