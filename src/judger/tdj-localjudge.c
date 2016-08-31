#include"../config/config.h"
#include"judger.h"
#include"compare.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
int main(int argc,char** argv){
    int qid,did,jfd,cfd,r,status;
    FILE* fl;
    char *lang;
    const size_t max_buf=1024;
    char jp[max_buf],fn[max_buf],cm[max_buf];
    if(argc!=3){
        puts("Usage: tdj-localjudge problem_id language");
    }else{
        qid=atoi(argv[1]);
        lang=argv[2];
        if(tdj_get_config(qid,"judge_data_path",jp)==-1){
            puts("Error: incorrect problem_id or incorrect judge_data_path");
            return 0;
        }
        if(tdj_get_config(qid,"compare_method",cm)==-1){
            puts("Error: incorrect compare_method");
            return 0;
        }
        if(tdj_compile(qid,0,lang,"a.out")==-1){
            puts("Error: compile error");
            return 0;
        }
        if(tdj_listen_SIGCHLD(0)==-1){
            puts("Error: bind SIGCHLD handler error");
            return 0;
        }
        for(did=1;;++did){
            sprintf(fn,"%s/%d/%d.in",jp,qid,did);
            fl=fopen(fn,"r");
            if(!fl) break;
            fclose(fl);
            jfd=tdj_judge(qid,did,"./a.out",&status);
            if(jfd==-1){
                printf("problem %d test %d : judge error %d\n",qid,did,status);
                continue;
            }
            sprintf(fn,"%s/%d/%d.out",jp,qid,did);
            fl=fopen(fn,"r");
            if(!fl||(cfd=fileno(fl))==-1){
                printf("problem %d test %d : no std output\n",qid,did);
                continue;
            }
            if(!strcmp(cm,"lesser"))
                r=tdj_lesser_cmp(jfd,cfd);
            else if(!strcmp(cm,"strict"))
                r=tdj_strict_cmp(jfd,cfd);
            else{
                printf("problem %d test %d : unknown compare_method\n",qid,did);
                continue;
            }
            if(r==1){
                printf("problem %d test %d : AC\n",qid,did);
            }else if(r==0){
                printf("problem %d test %d : WA\n",qid,did);
            }else if(r==-1){
                printf("problem %d test %d : compare error",qid,did);
            }
            close(jfd);
            fclose(fl);
        }
    }
    return 0;
}
