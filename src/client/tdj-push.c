/*
*
*    tdj-push.c - a part of the judge tool "tdj", as a client that communicate with the server
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
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include"time/time.h"
#include"server/server_def.h"
#include"judger/judger.h"
#include"z/zpipe.h"
#include"lib/zlib/zlib.h"
int read_addr(const char** ip,const char** pt){
    static char nip[128],npt[128];
    const size_t max_buf=128;
    FILE* fl;
    int sock;
    struct sockaddr_in addr;
    fd_set rds,tps;
    struct timeval to;
    tdj_usec_t tm;
    int rs;
    ssize_t str_len,now_len;
    const ssize_t cnt_len=sizeof(int32_t)*2;
    int32_t buf[2];
    socklen_t sz_addr;
    struct sockaddr_in addrs[max_buf];
    struct sockaddr* paddr=(struct sockaddr*)addrs;
    struct sockaddr_in *pbaddr;
    int pts[max_buf];
    int* ppt=pts,*pbpt,id;

    fl=fopen(".tdj","r");
    if(fl){
        fscanf(fl,"%s\n%s",nip,npt);
        *ip=nip;
        *pt=npt;
        fclose(fl);
        return 0;
    }
    *ip=tdj_get_config2(0,"target_server_ip");
    if(*ip==0) goto listen_broadcast;
    *pt=tdj_get_config2(0,"target_server_port");
    if(*pt==0) goto listen_broadcast;
    return 0;
listen_broadcast:;
    if((sock=socket(PF_INET,SOCK_DGRAM,0))==-1)return -1;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(14756);
    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr))==-1){
        close(sock);
        return -1;
    }
    FD_ZERO(&rds);
    FD_SET(sock,&rds);
    to.tv_sec=0;
    to.tv_usec=1000000-1;
    while(1){
        tps=rds;
        tm=tdj_time();
        rs=select(sock+1,&tps,0,0,&to);
        tm=tdj_time()-tm;
        if(rs==-1) goto fin_loop;
        else if(rs==0) goto fin_loop;
        else{
            if(FD_ISSET(sock,&tps)){
                now_len=0;
                sz_addr=sizeof(*paddr);
                str_len=recvfrom(sock,(char*)buf+now_len,cnt_len-now_len,0,paddr,&sz_addr);
                if(str_len!=cnt_len) goto fin_loop;
                if(buf[0]!=TDJ_VERSION) goto fin_loop;
                pbaddr=addrs;
                pbpt=pts;
                for(;pbpt<ppt&&(struct sockaddr*)pbaddr<paddr;++pbpt,++pbaddr)
                    if(pbaddr->sin_addr.s_addr==((struct sockaddr_in*)paddr)->sin_addr.s_addr&&*pbpt==buf[1])
                        goto fin_loop;
                *ppt++=buf[1];
                ++paddr;
            }
        }
fin_loop:;
        if(to.tv_usec<=tm) break;
        to.tv_usec-=tm;
    }
    close(sock);
    if(ppt==pts) return -1;
    pbpt=pts;
    pbaddr=addrs;
    puts("Recv broadcast from these servers");
    for(;pbpt<ppt&&(struct sockaddr*)pbaddr<paddr;++pbpt,++pbaddr){
        printf("%ld)\t%s:%d\n",(long)(pbpt-pts),inet_ntoa(pbaddr->sin_addr),*pbpt);
    }
    if(ppt-pts==1){
        puts("Select server 0");
        strcpy(nip,inet_ntoa(addrs[0].sin_addr));
        *ip=nip;
        sprintf(npt,"%d",pts[0]);
        *pt=npt;
        return 0;
    }
    printf("Select which? (0..%ld): ",(long)(ppt-pts-1));
read_id:scanf("%d",&id);
    if(id>=ppt-pts||id<0){
        printf("Out of range, try again: ");
        goto read_id;
    }
    strcpy(nip,inet_ntoa(addrs[id].sin_addr));
    *ip=nip;
    sprintf(npt,"%d",pts[id]);
    *pt=npt;
    return 0;
}
const char* get_filetype(const char* fn,char* ft){
    const size_t max_buf=1024;
    char t[max_buf];
    ssize_t i;
    size_t len;
    char cb[2];
    const char* dot;
    t[0]='\0';
    len=strlen(fn);
    cb[1]='\0';
    for(i=len-1;i>=0;--i){
        if(fn[i]=='.') break;
        cb[0]=fn[i];
        strcat(t,cb);
    }
    if(i>=0) dot=fn+i;
    else dot=fn+len;
    len=strlen(t);
    ft[0]='\0';
    for(i=len-1;i>=0;--i){
        cb[0]=t[i];
        strcat(ft,cb);
    }
    return dot;
}
int get_connected_socket(const char* ip,const char* pt){
    int sock;
    struct sockaddr_in addr;
    if((sock=socket(PF_INET,SOCK_STREAM,0))==-1)return -1;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_port=htons(atoi(pt));
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))==-1){close(sock);return -1;}
    return sock;
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
int main(int argc,char** argv){
    const char* ip=0;
    const char* pt=0;
    const char* fn=0;
    const char* lg=0;
    const char* cl;
    const size_t max_buf=1024;
    char ft[max_buf],qd[max_buf];
    int qid;
    const char* dot=0;
    FILE* fl,*sfl;
    int sock;
    int32_t frt[3];
    ssize_t r;
    int32_t rb[5];
    tdj_usec_t tm;
    int did=0;

    if(argc==4){
        fn=argv[1];
        ip=argv[2];
        pt=argv[3];
    }else if(argc==5){
        fn=argv[1];
        lg=argv[2];
        ip=argv[3];
        pt=argv[4];
    }else if(argc==2){
        fn=argv[1];
    }else if(argc==3){
        fn=argv[1];
        lg=argv[2];
    }else{
        printf("Usage: %s filename [lang] [server_ip] [server_port]\n",argv[0]);
        return EXIT_FAILURE;
    }
    if(ip==0){
        if(read_addr(&ip,&pt)==-1){
            puts("Error: cannot get server's addr");
            return EXIT_FAILURE;
        }
    }
    if(lg==0){
        dot=get_filetype(fn,ft);
        if(!strcmp(ft,"c"))
            lg="c";
        else if(!strcmp(ft,"cc")||!strcmp(ft,"cpp"))
            lg="c++";
        else{
            puts("Error: unknown lang");
            return EXIT_FAILURE;
        }
    }
    if(dot==0) dot=get_filetype(fn,ft);
    strncpy(qd,fn,dot-fn);
    qid=atoi(qd);
    fl=fopen(fn,"r");
    if(!fl){
        puts("Error: cannot open input file");
        return EXIT_FAILURE;
    }
    if((sock=get_connected_socket(ip,pt))==-1||!(sfl=fdopen(sock,"w"))){
        puts("Error: iniz socket error");
        return EXIT_FAILURE;
    }
    frt[0]=TDJ_VERSION;
    frt[1]=qid;
    if(!strcmp(lg,"c"))
        frt[2]=TDJ_C;
    else if(!strcmp(lg,"c++"))
        frt[2]=TDJ_CPP;
    else{
        close(sock);
        fclose(fl);
        puts("Error: unknown lang");
        return EXIT_FAILURE;
    }
    fwrite(frt,sizeof(int32_t),3,sfl);
    if((cl=tdj_get_config2(qid,"compress_level"))==0){
        close(sock);
        fclose(fl);
        puts("Error: cannot get \"compress_level\"");
        return EXIT_FAILURE;
    }
    if(def(fl,sfl,atoi(cl))!=Z_OK){
        close(sock);
        fclose(fl);
        puts("Error: compress error");
        return EXIT_FAILURE;
    }
    fclose(fl);
    fflush(sfl);
    sock=dup(sock);
    fclose(sfl);
    shutdown(sock,SHUT_WR);
    printf("%3s%12s%12s%12s%12s\n","DID","PID","JID","STATUS","TIME(USEC)");
    while((r=read_count(sock,(char*)rb,sizeof(int32_t)*5))==0){
        if(rb[0]!=TDJ_VERSION){
            close(sock);
            puts("Error: server version is different from client");
            return EXIT_FAILURE;
        }
        printf("%3d%12d%12d",++did,rb[1],rb[2]);
        switch(rb[3]){
            case TDJ_AC:case TDJ_WA:case TDJ_JE:
                read_count(sock,(char*)&tm,sizeof(tm));
                break;
            default:
                tm=0;
                break;
        }
        switch(rb[3]){
            case TDJ_AC:
                printf("%12s","AC");
                break;
            case TDJ_WA:
                printf("%12s","WA");
                break;
            case TDJ_OTHERROR:
                printf("%12s","OTHE");
                break;
            case TDJ_COMPILEERROR:
                printf("%12s","CE");
                break;
            case TDJ_JE:
                switch(rb[4]){
                    case TDJ_STILLRUNNINGERROR:
                        printf("%12s","TLE");
                        break;
                    case TDJ_EXITERROR:
                        printf("%12s","EE");
                        break;
                    case TDJ_TERMINATEERROR:
                        printf("%12s","RE");
                        break;
                    default:
                        printf("%12s","JE");
                        break;
                }
                break;
            default:
                printf("%12s","UE");
                break;
        }
        if(tm!=0){
#if __WORDSIZE == 64
            printf("%12lu",tm);
#else
            printf("%12llu",tm);
#endif
        }
        putchar('\n');
    }
    if(r==-1){
        close(sock);
        puts("Error: recv error");
        return EXIT_FAILURE;
    }
    close(sock);
    return EXIT_SUCCESS;
}
