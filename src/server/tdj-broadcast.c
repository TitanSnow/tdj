/*
*
*    tdj-broadcast.c - a part of the judge tool "tdj", providing server's addr broadcasting
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

#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include"config/config.h"
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
    int32_t buf[2];
    char pt[max_buf];
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
        usleep(200000);
    }
}
