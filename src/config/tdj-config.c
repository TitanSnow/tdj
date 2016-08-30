#include"config.h"
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
int isNum(const char* s){
    size_t i;
    for(i=0;s[i];++i)
        if(!isdigit(s[i])) return 0;
    return 1;
}
void error_usage(){
    puts("Usage: tdj-config [problem id] key [value]");
}
int main(int argc,char** argv){
    const size_t max_buf=1024;
    int qid=0;
    char* key=0;
    char* value=0;
    char dist[max_buf];
    dist[0]='\0';
    if(argc==2)
        key=argv[1];
    else if(argc==3){
        if(isNum(argv[1])){
            qid=atoi(argv[1]);
            key=argv[2];
        }else{
            key=argv[1];
            value=argv[2];
        }
    }else if(argc==4){
        if(!isNum(argv[1])){
            error_usage();
            return 0;
        }
        qid=atoi(argv[1]);
        key=argv[2];
        value=argv[3];
    }else{
        error_usage();
        return 0;
    }
    if(value){
        if(tdj_set_config(qid,key,value)==-1)
            puts("Error when setting config");
    }else{
        if(tdj_get_config(qid,key,dist)==-1)
            puts("Error: undefined key");
        else
            puts(dist);
    }
    return 0;
}
