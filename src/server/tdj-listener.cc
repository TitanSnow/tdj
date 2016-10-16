/*
*
*    tdj-listener.cc - a part of the judge tool "tdj", providing the listening to server
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

#include"server_def.h"
#include"judger/judger.h"
#include"config/config.h"
#include"time/time.h"
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<map>
#include<netinet/in.h>
using std::map;
using std::pair;
using std::make_pair;
pair<u_int64_t,int32_t> MAKESJID(const struct sockaddr_in* paddr,int32_t jid){
    u_int64_t adr;
    adr=paddr->sin_addr.s_addr;
    adr<<=32;
    adr|=paddr->sin_port;
    return make_pair(adr,jid);
}
int main(){
    int sock;
    struct sockaddr_in addr,faddr;
    socklen_t addrlen;
    const char *pt;
    int32_t buf[7];
    ssize_t r;
    map<pair<u_int64_t,int32_t>,int> didmap;
    
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    if((pt=tdj_get_config2(0,"listen_port"))==0) pt="25864";
    addr.sin_port=htons(atoi(pt));
    addr.sin_addr.s_addr=htonl(INADDR_ANY);

    if((sock=socket(PF_INET,SOCK_DGRAM,0))==-1){
        puts("Error: socket() error");
        return EXIT_FAILURE;
    }

    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr))==-1){
        puts("Error: bind() error");
        return EXIT_FAILURE;
    }

    printf("%15s:%5s%7s%12s%12s%12s%12s\n","SERVER ADDR","PORT","DID","PID","JID","STATUS","TIME(USEC)");
    while(1){
        addrlen=sizeof(faddr);
        if((r=recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr*)&faddr,&addrlen))==-1) continue;
        if(buf[0]!=TDJ_VERSION) continue;
        printf("%15s:%5d",inet_ntoa(faddr.sin_addr),ntohs(faddr.sin_port));
        printf("%7d%12d%12d",++didmap[MAKESJID(&faddr,buf[2])],buf[1],buf[2]);
        switch(buf[3]){
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
                switch(buf[4]){
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
        switch(buf[3]){
            case TDJ_AC:case TDJ_WA:case TDJ_JE:
#if __WORDSIZE == 64
                printf("%12lu",*(tdj_usec_t*)(buf+5));
#else
                printf("%12llu",*(tdj_usec_t*)(buf+5));
#endif
                break;
        }
        putchar('\n');
    }
}
