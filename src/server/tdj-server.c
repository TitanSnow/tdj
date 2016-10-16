/*
*
*    tdj-server.c - a part of the judge tool "tdj", as the server
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
#include"judger/judger.h"
#include"time/time.h"
#include"judger/compare.h"
#include"server_def.h"
#include"z/zpipe.h"
#include"lib/zlib/zlib.h"
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>
#include<netinet/in.h>
void error_handling(const char* mes){
    puts(mes);
    exit(EXIT_FAILURE);
}
int get_localsock(int* pport){
    struct sockaddr_in addr;
    const char *pt;
    int sock;
    int port;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    if((pt=tdj_get_config2(0,"listen_port"))==0) pt="25864";
    addr.sin_port=htons(port=atoi(pt));
    if((sock=socket(PF_INET,SOCK_DGRAM,0))==-1)return -1;
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))==-1){
        close(sock);
        return -1;
    }
    if(pport!=0)
        *pport=port;
    fcntl(sock,F_SETFD,FD_CLOEXEC);
    return sock;
}
void send_mes(int fd,int32_t ver,int32_t jid,int32_t mstat,int32_t jstat,tdj_usec_t tm){
    char data[sizeof(int32_t)*5+sizeof(tdj_usec_t)];
    int32_t* pdata=(int32_t*)data;
    *pdata++=ver==0?TDJ_VERSION:ver;
    *pdata++=(int32_t)getppid();
    *pdata++=jid;
    *pdata++=mstat;
    *pdata++=jstat;
    *(tdj_usec_t*)(pdata++)=tm;
    if(tm!=0)
        write(fd,data,sizeof(int32_t)*5+sizeof(tdj_usec_t));
    else
        write(fd,data,sizeof(int32_t)*5);
}
ssize_t read_count(int sock,char* buf,size_t cnt){
    ssize_t str_len,now_len=0;
    while(now_len<cnt){
        str_len=read(sock,buf+now_len,cnt-now_len);
        if(str_len==-1) return -1;
        else if(str_len==0) return now_len==0?-2:now_len;
        now_len+=str_len;
    }
    return 0;
}
int infd(int fd){
    FILE *s,*d;
    int pipefd[2];
    pid_t pid;
    if(pipe(pipefd)==-1)
        return -1;
    if((pid=fork())==-1)
        return -1;
    if(pid==0){
        // child
        if(fork()==0){
            // grandchild
            s=fdopen(fd,"r");
            d=fdopen(pipefd[1],"w");
            inf(s,d);
            fflush(d);
            _Exit(0);
        }
        // child
        _Exit(0);
    }
    // parent
    close(pipefd[1]);
    waitpid(pid,0,0);
    fcntl(pipefd[0],FD_CLOEXEC);
    return pipefd[0];
}
int main(int argc,char** argv){
    int ssock,csock,lsock;
    int cfd,jfd,ifd;
    int did,status,r,wait_time;
    int lport;
    struct sockaddr_in saddr,caddr;
    socklen_t caddr_sz;
    const size_t max_buf=1024;
    const char* lang,*sz_backlog,*jp,*cm,*bp,*ip,*pt,*wt;
    char outn[max_buf],fn[max_buf];
    pid_t pid;
    int32_t se,qid,jid=1;
    tdj_usec_t tm;

    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family=AF_INET;
    if(argc==2){
        if((ip=tdj_get_config2(0,"my_server_ip"))==0)
            saddr.sin_addr.s_addr=htonl(INADDR_ANY);
        else
            saddr.sin_addr.s_addr=inet_addr(ip);
        saddr.sin_port=htons(atoi(argv[1]));
    }else if(argc==3){
        saddr.sin_addr.s_addr=inet_addr(argv[1]);
        saddr.sin_port=htons(atoi(argv[2]));
    }else if(argc==1){
        if((ip=tdj_get_config2(0,"my_server_ip"))==0)
            saddr.sin_addr.s_addr=htonl(INADDR_ANY);
        else
            saddr.sin_addr.s_addr=inet_addr(ip);
        if((pt=tdj_get_config2(0,"my_server_port"))==0){
            printf("Usage: %s [IP] port",argv[0]);
            error_handling("");
        }
        saddr.sin_port=htons(atoi(pt));
    }else{
        printf("Usage: %s [IP] port",argv[0]);
        error_handling("");
    }

    if((wt=tdj_get_config2(0,"server_wait_time"))==0)
        error_handling("Error: get config \"server_wait_time\" error");
    wait_time=atoi(wt);
    
    if((ssock=socket(PF_INET,SOCK_STREAM,0))==-1)
        error_handling("Error: socket() error");
    if(bind(ssock,(struct sockaddr*)&saddr,sizeof(saddr))==-1){
        close(ssock);
        error_handling("Error: bind() error");
    }
    
    if((sz_backlog=tdj_get_config2(0,"backlog"))==0){
        close(ssock);
        error_handling("Error: get config \"backlog\" error");
    }

    if(listen(ssock,atoi(sz_backlog))==-1){
        close(ssock);
        error_handling("Error: listen() error");
    }

    signal(SIGCHLD,SIG_IGN);
    
    if((lsock=get_localsock(&lport))==-1){
        puts("Error: cannot get socket to listener");
    }else{
        printf("Listening on port %d\n",lport);
    }

    while(1){
        caddr_sz=sizeof(caddr);
        if((csock=accept(ssock,(struct sockaddr*)&caddr,&caddr_sz))==-1)
            continue;
        pid=fork();
        if(pid==-1){
            send_mes(csock,0,jid,TDJ_OTHERROR,TDJ_JUDGESUCCESS,0);
            send_mes(lsock,0,jid,TDJ_OTHERROR,TDJ_JUDGESUCCESS,0);
            close(csock);
            continue;
        }
        if(pid!=0){
            // parent
            close(csock);
            if(++jid<=0) jid=1;
            if(wait_time)usleep(wait_time);
            continue;
        }
        // child
        goto normal_run;
        error_exit:;
        {
            send_mes(csock,0,jid,TDJ_OTHERROR,TDJ_JUDGESUCCESS,0);
            send_mes(lsock,0,jid,TDJ_OTHERROR,TDJ_JUDGESUCCESS,0);
            close(csock);
            close(lsock);
            return EXIT_FAILURE;
        }
        normal_run:;
        close(ssock);
        fcntl(csock,F_SETFD,FD_CLOEXEC);
        signal(SIGCHLD,tdj_void_signal_handler);
        if(read_count(csock,(char*)&se,sizeof(se))!=0) goto error_exit;
        if(se!=TDJ_VERSION)
            goto error_exit;
        if(read_count(csock,(char*)&se,sizeof(se))!=0) goto error_exit;
        qid=se;
        if(read_count(csock,(char*)&se,sizeof(se))!=0) goto error_exit;
        if(se==TDJ_C)
            lang="c";
        else if(se==TDJ_CPP)
            lang="c++";
        else goto error_exit;
        
        if((jp=tdj_get_config2(qid,"judge_data_path"))==0)
            goto error_exit;
        if((cm=tdj_get_config2(qid,"compare_method"))==0)
            goto error_exit;
        if((bp=tdj_get_config2(qid,"judge_build_path"))==0)
            goto error_exit;
        sprintf(outn,"%s/%d.out",bp,getpid());
        if(tdj_compile(qid,infd(csock),lang,outn,0)==-1){
            send_mes(csock,0,jid,TDJ_COMPILEERROR,TDJ_JUDGESUCCESS,0);
            send_mes(lsock,0,jid,TDJ_COMPILEERROR,TDJ_JUDGESUCCESS,0);
            close(csock);
            close(lsock);
            return EXIT_FAILURE;
        }
        for(did=1;;++did){
            sprintf(fn,"%s/%d/%d.in",jp,qid,did);
            ifd=open(fn,O_RDONLY);
            if(ifd==-1) break;
            close(ifd);
            tm=tdj_time();
            jfd=tdj_judge(qid,did,outn,&status);
            tm=tdj_time()-tm;
            if(tm==0)tm=1;
            if(jfd==-1){
                send_mes(csock,0,jid,TDJ_JE,status,tm);
                send_mes(lsock,0,jid,TDJ_JE,status,tm);
                continue;
            }
            sprintf(fn,"%s/%d/%d.out",jp,qid,did);
            cfd=open(fn,O_RDONLY);
            if(cfd==-1){
                send_mes(csock,0,jid,TDJ_OTHERROR,status,0);
                send_mes(lsock,0,jid,TDJ_OTHERROR,status,0);
                continue;
            }
            if(!strcmp(cm,"lesser"))
                r=tdj_lesser_cmp(jfd,cfd);
            else if(!strcmp(cm,"strict"))
                r=tdj_strict_cmp(jfd,cfd);
            else{
                send_mes(csock,0,jid,TDJ_OTHERROR,status,0);
                send_mes(lsock,0,jid,TDJ_OTHERROR,status,0);
                continue;
            }
            if(r==1){
                send_mes(csock,0,jid,TDJ_AC,status,tm);
                send_mes(lsock,0,jid,TDJ_AC,status,tm);
            }else if(r==0){
                send_mes(csock,0,jid,TDJ_WA,status,tm);
                send_mes(lsock,0,jid,TDJ_WA,status,tm);
            }else if(r==-1){
                send_mes(csock,0,jid,TDJ_OTHERROR,status,0);
                send_mes(lsock,0,jid,TDJ_OTHERROR,status,0);
            }
            close(jfd);
            close(cfd);
        }
        unlink(outn);
        
        close(csock);
        close(lsock);
        return EXIT_SUCCESS;
    }
}
