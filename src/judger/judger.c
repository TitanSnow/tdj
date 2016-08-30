#include"judger.h"
#include"../config/config.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
int tdj_compile(int qid,int fd,const char* lang){
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
        dup2(fd,0);
        if(fd!=0)close(fd);
        execlp(cp,cp,"-x",lang,"-",NULL);
    }
}
int tdj_judge_aout(int qid,int did){
    pid_t pid;
    const size_t max_buf=1024;
    char fn[max_buf],jp[max_buf],tl[max_buf];
    int pipefd[2];
    int fd;
    int wstatus;
    int time_limit;
    if(pipe(pipefd)==-1) return -1;
    if((pid=fork())){
        // parent
        goto normal_run;
        error_exit:;
        { 
            kill(pid,SIGKILL);
            waitpid(pid,&wstatus,0);
            return -1;
        }
        normal_run:;
        close(pipefd[1]);
        tl[0]='\0';
        if(tdj_get_config(qid,"time_limit",tl)==-1) goto error_exit;
        time_limit=atoi(tl);
        if(usleep(time_limit)==-1) goto error_exit;
        if(waitpid(pid,&wstatus,WNOHANG)!=pid) goto error_exit;
        if(!WIFEXITED(wstatus)||WEXITSTATUS(wstatus)!=0) return -1;
        return pipefd[0];
    }else{
        // child
        jp[0]='\0';
        if(tdj_get_config(qid,"judge_data_path",jp)==-1) exit(-1);
        sprintf(fn,"%s/%d/%d.in",jp,qid,did);
        fd=open(fn,O_RDONLY);
        if(fd==-1) exit(-1);
        dup2(fd,0);
        close(fd);
        dup2(pipefd[1],1);
        close(pipefd[1]);
        close(pipefd[0]);
        fd=open("/dev/null",O_WRONLY);
        if(fd==-1) exit(-1);
        dup2(fd,2);
        close(fd);
        execlp("./a.out","./a.out",NULL);
    }
}
