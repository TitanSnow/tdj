#include"judger.h"
#include"../config/config.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
int tdj_compile(int qid,int fd,const char* lang,const char* path){
    pid_t pid;
    const size_t max_buf=1024;
    char cp[max_buf];
    int wstatus;
    if((pid=fork())){
        // parent
        waitpid(pid,&wstatus,0);
        if(WIFEXITED(wstatus)&&WEXITSTATUS(wstatus)==0) return 0;
        else return -1;
    }else{
        // child
        cp[0]='\0';
        if(tdj_get_config(qid,"compiler",cp)==-1) exit(-1);
        if(dup2(fd,0)==-1) exit(-1);
        if(fd!=0)close(fd);
        execlp(cp,cp,"-x",lang,"-","-o",path,NULL);
        exit(-1);
    }
}
int tdj_judge(int qid,int did,const char* path,int *pstatus){
    pid_t pid;
    const size_t max_buf=1024;
    char fn[max_buf],jp[max_buf],tl[max_buf],pp[max_buf];
    int pipefd[2];
    int fd;
    int wstatus;
    int time_limit;
    int t;
    FILE* pf;
    char ts;
    if(pstatus)*pstatus=TDJ_JUDGESUCCESS;
    if(pipe(pipefd)==-1){
        if(pstatus)*pstatus=TDJ_PIPEGETTINGERROR;
        return -1;
    }
    if((pid=fork())){
        // parent
        goto normal_run;
        error_exit:;
        {
            sprintf(pp,"/proc/%d/stat",pid);
            pf=fopen(pp,"r");
            if(!pf) return -1;
            ts='X';
            fscanf(pf,"%*d %*s %c",&ts);
            switch(ts){
                case 'R':case 'S':case 'T':
                    kill(pid,SIGKILL);
                    // No break;
                case 'Z':
                    waitpid(pid,&wstatus,0);
                    // No break;
                case 'D':case 'X':default:
                    fclose(pf);
                    return -1;
            }
        }
        normal_run:;
        close(pipefd[1]);
        tl[0]='\0';
        if(tdj_get_config(qid,"time_limit",tl)==-1){
            if(pstatus)*pstatus=TDJ_TIMELIMITGETTINGERROR;
            goto error_exit;
        }
        time_limit=atoi(tl);
        if(usleep(time_limit)==-1){
            // if(pstatus)*pstatus=TDJ_USLEEPERROR;
            // goto error_exit;
        }
        if((t=waitpid(pid,&wstatus,WNOHANG))==0){
            if(pstatus)*pstatus=TDJ_STILLRUNNINGERROR;
            goto error_exit;
        }
        if(t==-1){
            if(pstatus)*pstatus=TDJ_WAITERROR;
            goto error_exit;
        }
        if(!WIFEXITED(wstatus)||WEXITSTATUS(wstatus)!=0){
            if(pstatus)*pstatus=TDJ_EXITERROR;
            return -1;
        }
        return pipefd[0];
    }else{
        // child
        jp[0]='\0';
        if(tdj_get_config(qid,"judge_data_path",jp)==-1) exit(-1);
        sprintf(fn,"%s/%d/%d.in",jp,qid,did);
        fd=open(fn,O_RDONLY);
        if(fd==-1) exit(-1);
        if(dup2(fd,0)==-1) exit(-1);
        close(fd);
        if(dup2(pipefd[1],1)==-1) exit(-1);
        close(pipefd[1]);
        close(pipefd[0]);
        fd=open("/dev/null",O_WRONLY);
        if(fd==-1) exit(-1);
        if(dup2(fd,2)==-1) exit(-1);
        close(fd);
        execlp(path,path,NULL);
        exit(-1);
    }
}
void tdj_void_sa_handler(int sig){}
int tdj_listen_SIGCHLD(struct sigaction* oldact){
    struct sigaction act;
    memset(&act,0,sizeof(act));
    act.sa_handler=tdj_void_sa_handler;
    return sigaction(SIGCHLD,&act,oldact);
}
