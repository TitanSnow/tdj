#include"compare.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int tdj_strict_cmp(int fd1,int fd2){
    FILE* fl1,*fl2;
    int ch1,ch2;
    if(!(fl1=fdopen(fd1,"r"))) return -1;
    if(!(fl2=fdopen(fd2,"r"))) return -1;
    while(1){
        ch1=fgetc(fl1);
        ch2=fgetc(fl2);
        if(ch1!=ch2) return 0;
        if(ch1==EOF) return 1;
    }
}
int tdj_lesser_cmp(int fd1,int fd2){
    FILE* fl1,*fl2;
    const size_t max_buf=1024;
    char st1[max_buf],st2[max_buf];
    int r1,r2;
    if(!(fl1=fdopen(fd1,"r"))) return -1;
    if(!(fl2=fdopen(fd2,"r"))) return -1;
    while(1){
        r1=fscanf(fl1,"%s",st1);
        r2=fscanf(fl2,"%s",st2);
        if(r1==EOF){
            if(r2==EOF) return 1;
            else return 0;
        }else if(r2==EOF) return 0;
        if(strcmp(st1,st2)!=0) return 0;
    }
}
