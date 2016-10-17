/*
*
*    judger.c - a part of the judge tool "tdj", providing program judging
*    Copyright (C) 2016  TitanSnow <sweeto@live.cn>
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as published
*    by the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include"judger.h"
#include"config/config.h"
#include"utility/printf.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
int tdj_compile(int qid,int fd,const char* lang,const char* path,int* pcpfd){
    pid_t pid;
    const char *cp;
    int wstatus;
    int pipefd[2];
    struct sigaction sa;
    if(sigaction(SIGCHLD,0,&sa)==-1) return -1;
    if(sa.sa_handler!=tdj_void_signal_handler) return -1;
    if(pipe(pipefd)==-1) return -1;
    if(pcpfd!=0)*pcpfd=pipefd[0];
    pid=fork();
    if(pid==-1) return -1;
    if(pid){
        // parent
        close(pipefd[1]);
        waitpid(pid,&wstatus,0);
        if(pcpfd==0)close(pipefd[0]);
        if(WIFEXITED(wstatus)&&WEXITSTATUS(wstatus)==0)
            return 0;
        else
            return -1;
    }else{
        // child
        if((cp=tdj_get_config2(qid,"compiler"))==0) exit(-1);
        if(dup2(fd,0)==-1) exit(-1);
        if(fd!=0)close(fd);
        close(pipefd[0]);
        if(dup2(pipefd[1],1)==-1) exit(-1);
        if(dup2(pipefd[1],2)==-1) exit(-1);
        close(pipefd[1]);
        execlp(cp,cp,"-x",lang,"-","-o",path,NULL);
        exit(-1);
    }
}
int tdj_judge6(int qid,int did,const char* path,int *pstatus,int *pwstatus,struct rusage *usage){
    pid_t pid;
    const char *jp,*tl,*fn,*pp,*rl;
    int pipefd[2];
    int fd;
    int wstatus;
    int time_limit;
    int t;
    FILE* pf;
    char ts;
    struct rlimit rlm;
    if(pstatus)*pstatus=TDJ_JUDGESUCCESS;
    struct sigaction sa;
    if(sigaction(SIGCHLD,0,&sa)==-1){
        *pstatus=TDJ_SIGERROR;
        return -1;
    }
    if(sa.sa_handler!=tdj_void_signal_handler){
        *pstatus=TDJ_SIGERROR;
        return -1;
    }
    if(pipe(pipefd)==-1){
        if(pstatus)*pstatus=TDJ_PIPEGETTINGERROR;
        return -1;
    }
    pid=fork();
    if(pid==-1){
        if(pstatus)*pstatus=TDJ_FORKERROR;
        return -1;
    }
    if(pid){
        // parent
        goto normal_run;
        error_exit:;
        {
            pp=mprintf("/proc/%d/stat",pid);
            pf=fopen(pp,"r");
            free((void*)pp);
            if(!pf) return -1;
            ts='X';
            fscanf(pf,"%*d %*s %c",&ts);
            switch(ts){
                case 'R':case 'S':case 'T':
                    kill(pid,SIGKILL);
                    // No break;
                case 'Z':
                    wait4(pid,&wstatus,0,usage);
                    if(pwstatus)*pwstatus=wstatus;
                    // No break;
                case 'D':case 'X':default:
                    fclose(pf);
                    return -1;
            }
        }
        normal_run:;
        close(pipefd[1]);
        if((tl=tdj_get_config2(qid,"time_limit"))==0){
            if(pstatus)*pstatus=TDJ_TIMELIMITGETTINGERROR;
            goto error_exit;
        }
        time_limit=atoi(tl);
        if(usleep(time_limit)==-1){
            // if(pstatus)*pstatus=TDJ_USLEEPERROR;
            // goto error_exit;
        }
        if((t=wait4(pid,&wstatus,WNOHANG,usage))==0){
            if(pstatus)*pstatus=TDJ_STILLRUNNINGERROR;
            goto error_exit;
        }
        if(t==-1){
            if(pstatus)*pstatus=TDJ_WAITERROR;
            goto error_exit;
        }
        if(pwstatus)*pwstatus=wstatus;
        if(WIFEXITED(wstatus)){
            if(WEXITSTATUS(wstatus)!=0){
                if(pstatus)*pstatus=TDJ_EXITERROR;
                return -1;
            }
        }else{
            if(pstatus)*pstatus=TDJ_TERMINATEERROR;
            return -1;
        }
        return pipefd[0];
    }else{
        // child
        if((jp=tdj_get_config2(qid,"judge_data_path"))==0) exit(-1);
        fn=mprintf("%s/%d/%d.in",jp,qid,did);
        fd=open(fn,O_RDONLY);
        free((void*)fn);
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
        if((rl=tdj_get_config2(qid,"rlimit_as"))!=0){
            rlm.rlim_cur=rlm.rlim_max=atoi(rl);
            setrlimit(RLIMIT_AS,&rlm);
        }
        if((rl=tdj_get_config2(qid,"rlimit_core"))!=0){
            rlm.rlim_cur=rlm.rlim_max=atoi(rl);
            setrlimit(RLIMIT_CORE,&rlm);
        }
        if((rl=tdj_get_config2(qid,"time_limit"))!=0){
            rlm.rlim_cur=rlm.rlim_max=(atoi(rl)+999999)/1000000;
            setrlimit(RLIMIT_CPU,&rlm);
        }
        execlp(path,path,NULL);
        exit(-1);
    }
}
int tdj_judge(int qid,int did,const char* path,int *pstatus){
    return tdj_judge6(qid,did,path,pstatus,0,0);
}
void tdj_void_signal_handler(int sig){}
