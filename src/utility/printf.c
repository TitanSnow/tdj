#include"printf.h"
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
char* mprintf(const char *zFormat, ...){
    va_list ap;
    char *z;
    va_start(ap, zFormat);
    z = vmprintf(zFormat, ap);
    va_end(ap);
    return z;
}
char* vmprintf(const char *zFormat, va_list ap){
    int pipefd[2];
    char *z=0;
    size_t len=0;
    if(pipe(pipefd)==-1) return 0;
    if(vdprintf(pipefd[1],zFormat,ap)<0) return 0;
    close(pipefd[1]);
    do{
        z=realloc(z,(++len)*sizeof(char));
        if(z==0){
            close(pipefd[0]);
            return 0;
        }
    }while(read(pipefd[0],z+len-1,sizeof(char))==sizeof(char));
    z[len-1]='\0';
    close(pipefd[0]);
    return z;
}
