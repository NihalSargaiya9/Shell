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

    // arg = (char *)malloc(sizeof(char) * 512);
    for (int i = start; i <= len; i++)
    {


        if (res[i] == ' ' || res[i] == '\n' || res[i] == '|' || res[i] == ';')
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
int checkParallel(char *buf,int len)
{
    while (len - 1 > 0)
    {
        if (buf[len - 1] == ';')
            return len - 1;
        len--;
    }
    return -1;
}
int checkLogical(char *buf,int len)
{
    while (len - 1 > 0)
    {
        if (buf[len - 1] == '&' && buf[len - 2] == '&')
            return len - 2;
        if (buf[len - 1] == '|' && buf[len - 2] == '|')
            return len - 2 ;
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
            unlink(fileName);
            open(fileName, O_CREATE | O_RDWR);
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
    return 1;
}
int runCmd(char *buf, int len, int parent)
{
    // psinfo();
    int pipeFlag = 0;
    int p[2],parallel=-1,logical=0;
    char *argsLeft[32], *argsRight[32], *legacy[] = {"ls", "cat", "grep", "echo", "wc","ps", "procinfo"}, legacyLen = 7;
    flushArgs(argsLeft);
    flushArgs(argsRight);
    parallel = checkParallel(buf,len); 
    if(parallel!=-1)
    {
        extractCmd(buf, 0, parallel, argsLeft);
        extractCmd(buf, parallel + 2, 512, argsRight);
        if(fork()==0)
        {
                exec(argsLeft[0],argsLeft);
        }
        if (fork()==0)
        {
            exec(argsRight[0],argsRight);
        }
         wait(0);
         wait(0);

        for(int i=3;i<16;i++)
            close(i);
        return 1;
    }
    logical = checkLogical(buf,len);
    if(logical!=-1)
    {  
        extractCmd(buf, 0, logical-1, argsLeft);
        extractCmd(buf, logical + 3, 512, argsRight);
        if(buf[logical]=='&')
        {
            int p1es;
            if(fork()==0)
            {
                    exec(argsLeft[0],argsLeft);
            }
            else
                wait(&p1es);
            if(p1es==0)
            {
                if(fork()==0)
                    exec(argsRight[0],argsRight);
                else                    
                    wait(0);
            }
        }
        if(buf[logical]=='|')
        {
            int p1es;
            if(fork()==0)
            {
                    exec(argsLeft[0],argsLeft);
            }
            wait(&p1es);
            if(p1es!=0)
            {
                if(fork()==0)
                    exec(argsRight[0],argsRight);
                wait(0);
            }
            
        }
        for(int i=3;i<16;i++)
            close(i);
        return 1;
    }
    int pipeIndex = len;
    int tempPipeIndex = checkPipe(buf, len);
    if (tempPipeIndex > 0)
    {
        pipe(p);
        pipeIndex = tempPipeIndex;

    }
    if (tempPipeIndex > 0)
    {
        extractCmd(buf, pipeIndex + 2, 512, argsRight);
        runCmd(buf, pipeIndex - 1, 1);
        pipeFlag = 1;
    }
    if (tempPipeIndex == -1 && parent)
        return 1;
    extractCmd(buf, 0, pipeIndex, argsLeft);
    if (fork() == 0) 
    {

        int cmdFLag = 0;
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
        if (exec(argsLeft[0], argsLeft) < 0)
            printf(1, "error");
    }
    if (argsRight[0] && fork() == 0)
    {
        int cmdFLag = 0;
        checkForRedirection(argsRight);
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
  
    for(int i=3;i<16;i++)
        close(i);
    return 1;
}
int main(void)
{
    char buf[512], en[] = "exit\n",*tempArgs[32];
    while (1)
    {
        getcmd(buf, sizeof(buf));
        if (!strcmp(en, buf))
        {
            wait(0);
            break;
        }
        extractCmd(buf,0,strlen(buf),tempArgs);
        if(!strcmp(tempArgs[0],"executeCommands"))
        {  
            int n,index=0;
            char newbuf[512];
            int fd = open(tempArgs[1],O_RDONLY);
            while((n = read(fd, newbuf, sizeof(newbuf))) > 0) {
                char cmd[512];
                for(int i=0;i<strlen(newbuf);i++)
                {
                    cmd[index]=newbuf[i];
                    index++;
                    if( i == strlen(newbuf)-1  || newbuf[i]=='\n')
                    {
                        if(i == strlen(newbuf)-1)
                            cmd[index]='\n';
                        runCmd(cmd,strlen(cmd),0);
                        memset(cmd, 0, sizeof(cmd)); 
                        index=0;
                    }
                }
            }
            for(int i=3;i<16;i++)
               close(i);
            continue;
        }

        runCmd(buf, strlen(buf), 0);

        for(int i=3;i<16;i++)
        {
            close(i);
        }
    }
    exit(1);
}