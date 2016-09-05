#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
#include"../config/config.h"
#include"server_def.h"
int has_dot(const char* p){
    for(;*p!='\0';++p)
        if(*p=='.') return 1;
    return 0;
}
int main(int argc,char** argv){
    const size_t max_buf=1024;
    int sock;
    struct sockaddr_in addr;
    int so_brd=1;
    char buf[sizeof(int32_t)*2],pt[max_buf];
    int port=-1;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(14756);
    if(argc==3){
        addr.sin_addr.s_addr=inet_addr(argv[1]);
        port=atoi(argv[2]);
    }else if(argc==2){
        if(has_dot(argv[1]))
            addr.sin_addr.s_addr=inet_addr(argv[1]);
        else port=atoi(argv[1]);
    }
    else if(argc==1)
        addr.sin_addr.s_addr=inet_addr("255.255.255.255");
    else{
        printf("Usage: %s [Broad IP] [Server port]",argv[0]);
        return EXIT_FAILURE;
    }
    if((sock=socket(PF_INET,SOCK_DGRAM,0))==-1){
        puts("Error: socket() error");
        return EXIT_FAILURE;
    }
    if(setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&so_brd,sizeof(so_brd))==-1){
        puts("Error: setsockopt() error");
        return EXIT_FAILURE;
    }
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))==-1){
        puts("Error: connect() error");
        return EXIT_FAILURE;
    }
    if(port==-1){
        if(tdj_get_config(0,"my_server_port",pt)==-1){
            puts("Error: Incorrect server port");
            return EXIT_FAILURE;
        }
        port=atoi(pt);
    }
    buf[0]=TDJ_VERSION;
    buf[1]=port;
    while(1){
        write(sock,buf,sizeof(int32_t)*2);
        usleep(333333);
    }
}
