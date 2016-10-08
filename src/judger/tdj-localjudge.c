/*
*
*    tdj-localjudge.c - a part of the judge tool "tdj", providing local judging
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

#include"config/config.h"
#include"judger.h"
#include"compare.h"
#include"time/time.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
void print_time(tdj_usec_t tm){
#if __WORDSIZE == 64
    printf("%lu",tm);
#else
    printf("%llu",tm);
#endif
}
int main(int argc,char** argv){
    int qid,did,jfd,cfd,r,status,i,ofd,ifd;
    char *lang;
    const size_t max_buf=1024;
    char jp[max_buf],fn[max_buf],cm[max_buf],outn[max_buf],bp[max_buf];
    tdj_usec_t tm;
    int cpfd;
    FILE* cpfl;
    int ch;
    if(argc!=3){
        printf("Usage: %s problem_id language\n",argv[0]);
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
        if(tdj_get_config(qid,"judge_build_path",bp)==-1){
            puts("Error: incorrect judge_build_path");
            return 0;
        }
        for(i=0;;++i){
            sprintf(outn,"%s/%d.out",bp,i);
            ofd=open(outn,O_RDONLY);
            if(ofd==-1){
                if(errno==ENOENT){
                    cpfd=-1;
                    if(tdj_compile(qid,0,lang,outn,&cpfd)==-1){
                        if(cpfd!=-1){
                            cpfl=fdopen(cpfd,"r");
                            while((ch=fgetc(cpfl))!=EOF)
                                putchar(ch);
                            fclose(cpfl);
                        }
                        puts("Error: compile error");
                        return 0;
                    }
                    cpfl=fdopen(cpfd,"r");
                    while((ch=fgetc(cpfl))!=EOF)
                        putchar(ch);
                    fclose(cpfl);
                    break;
                }
            }else close(ofd);
        }
        if(tdj_listen_SIGCHLD(0)==-1){
            puts("Error: bind SIGCHLD handler error");
            return 0;
        }
        for(did=1;;++did){
            sprintf(fn,"%s/%d/%d.in",jp,qid,did);
            ifd=open(fn,O_RDONLY);
            if(ifd==-1) break;
            close(ifd);
            tm=tdj_time();
            jfd=tdj_judge(qid,did,outn,&status);
            tm=tdj_time()-tm;
            if(jfd==-1){
                printf("problem %d test %d : judge error %d ",qid,did,status);
                print_time(tm);
                printf("microseconds\n");
                continue;
            }
            sprintf(fn,"%s/%d/%d.out",jp,qid,did);
            cfd=open(fn,O_RDONLY);
            if(cfd==-1){
                printf("problem %d test %d : no std output ",qid,did);
                print_time(tm);
                printf("microseconds\n");
                continue;
            }
            if(!strcmp(cm,"lesser"))
                r=tdj_lesser_cmp(jfd,cfd);
            else if(!strcmp(cm,"strict"))
                r=tdj_strict_cmp(jfd,cfd);
            else{
                printf("problem %d test %d : unknown compare_method ",qid,did);
                print_time(tm);
                printf("microseconds\n");
                continue;
            }
            if(r==1){
                printf("problem %d test %d : AC ",qid,did);
                print_time(tm);
                printf("microseconds\n");
            }else if(r==0){
                printf("problem %d test %d : WA ",qid,did);
                print_time(tm);
                printf("microseconds\n");
            }else if(r==-1){
                printf("problem %d test %d : compare error ",qid,did);
                print_time(tm);
                printf("microseconds\n");
            }
            close(jfd);
            close(cfd);
        }
        unlink(outn);
    }
    return 0;
}
