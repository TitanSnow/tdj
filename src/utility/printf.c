#include"printf.h"
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
char* mprintf(const char *zFormat, ...){
    va_list ap;
    char *z;
    va_start(ap, zFormat);
    z = vmprintf(zFormat, ap);
    va_end(ap);
    return z;
}
char* vmprintf(const char *zFormat, va_list ap){
    FILE *fm;
    char *buf,*z;
    size_t sizeloc;
    if((fm=open_memstream(&buf,&sizeloc))==NULL) return 0;
    if(vfprintf(fm,zFormat,ap)<0){
        fclose(fm);
        free(buf);
        return 0;
    }
    if(fflush(fm)!=0){
        fclose(fm);
        free(buf);
        return 0;
    }
    if((z=malloc((strlen(buf)+1)*sizeof(char)))==NULL){
        fclose(fm);
        free(buf);
        return 0;
    }
    strcpy(z,buf);
    fclose(fm);
    free(buf);
    return z;
}
