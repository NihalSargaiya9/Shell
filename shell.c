#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int getcmd(char *buf, int nbuf)
{
    printf(1, "myshell>");
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
int extractCmd(char *res, int start, int len, char *args[32])
{
    int index = 0, j = 0;
    char *arg;
    arg = (char *)malloc(sizeof(char) * 512);
    for (int i = start; i <= len; i++)
    {

        if (res[i] == ' ' || res[i] == '\n' || res[i] == '|')
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
int checkPipe(char *buf, int len)
{
    while (len - 1 > 0)
    {
        if (buf[len - 1] == '|')
            return len - 1;
        len--;
    }
    return -1;
}
int checkForRedirection(char *args[])
{
    char fileName[100];
    int i = 0;
    int readIndex = -1, writeIndex = -1;
    while (args[i] && args[i + 1])
    {
        if (!strcmp("<", args[i]))
        {
            strcpy(fileName, args[i + 1]);
            close(0);
            open(fileName, O_RDONLY);
            // args[i]=0;
            // args[i+1]=0;
            readIndex = i;
            break;
        }
        i++;
    }
    i=0;
    while (args[i] && args[i + 1])
    {
        if (!strcmp(">", args[i]))
        {
            strcpy(fileName, args[i + 1]);
            close(1);
            open(fileName, O_CREATE | O_WRONLY);
            // args[i]=0;
            // args[i+1]=0;
            // break;
            writeIndex = i;
        }
        i++;
    }
    if (readIndex != -1)
    {
        args[readIndex] = 0;
        args[readIndex + 1] = 0;
    }
    if (writeIndex != -1)
    {
        args[writeIndex] = 0;
        args[writeIndex + 1] = 0;
    }
    // for (int i = 0; i < 5; i++)
    //     printf(1, "%s\n", args[i]);
    return 1;
}
int runCmd(char *buf, int len, int parent)
{

    int pipeFlag = 0;
    int p[2];
    pipe(p);
    char *argsLeft[32], *argsRight[32], *legacy[] = {"ls", "cat", "grep", "echo", "wc"}, legacyLen = 5;
    flushArgs(argsLeft);
    flushArgs(argsRight);
    int pipeIndex = len;
    int tempPipeIndex = checkPipe(buf, len);
    if (tempPipeIndex > 0)
        pipeIndex = tempPipeIndex;
    if (tempPipeIndex > 0)
    {
        extractCmd(buf, pipeIndex + 2, 512, argsRight);
        runCmd(buf, pipeIndex - 1, 1);
        pipeFlag = 1;
    }
    if (tempPipeIndex == -1 && parent)
        return 1;
    extractCmd(buf, 0, pipeIndex, argsLeft);
    psinfo();
    if (fork() == 0)
    {
        // char fileName[10]="ls";
        int cmdFLag = 0;
        // close(0);
        // open(fileName,O_RDONLY);
        checkForRedirection(argsLeft);
        for (int i = 0; i < legacyLen; i++)
        {
            if (!strcmp(legacy[i], argsLeft[0]))
            {
                cmdFLag = 1;
                break;
            }
        }
        if (!cmdFLag)
        {
            printf(1, "Illegal command or arguments\n");
            exit(-1);
        }
        if (pipeFlag)
        {
            close(1);
            dup(p[1]);
            close(p[0]);
            close(p[1]);
        }
        // procinfo(3);
        if (exec(argsLeft[0], argsLeft) < 0)
            printf(1, "error");
        // psinfo();
        // procinfo(4);
    }
    if (argsRight[0] && fork() == 0)
    {
        int cmdFLag = 0;
        for (int i = 0; i < legacyLen; i++)
        {
            if (!strcmp(legacy[i], argsLeft[0]))
            {
                cmdFLag = 1;
                break;
            }
        }
        if (!cmdFLag)
        {
            printf(1, "Illegal command or arguments\n");
            exit(-1);
        }
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        if (exec(argsRight[0], argsRight) < 0)
            printf(1, "error");
    }
    if (argsRight[0])
    {
        close(p[0]);
        close(p[1]);
        wait(0);
    }
    wait(0);

    return 1;
}
int main(void)
{
    char buf[512], en[] = "exit\n";
    while (1)
    {
        getcmd(buf, sizeof(buf));
        if (!strcmp(en, buf))
            break;
        runCmd(buf, strlen(buf), 0);
    }
    exit(1);
}