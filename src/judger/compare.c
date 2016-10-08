/*
*
*    compare.c - a part of the judge tool "tdj", providing file comparing
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

#include"compare.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int tdj_strict_cmp(int fd1,int fd2){
    FILE* fl1,*fl2;
    int ch1,ch2;
    if(!(fl1=fdopen(fd1,"r"))) return -1;
    if(!(fl2=fdopen(fd2,"r"))) return -1;
    while(1){
        fread(&ch1,1,1,fl1);
        fread(&ch2,1,1,fl2);
        if(ch1!=ch2) return 0;
        if(ch1==EOF) return 1;
    }
}
int tdj_lesser_cmp(int fd1,int fd2){
    FILE* fl1,*fl2;
    const size_t max_buf=1024;
    char st1[max_buf],st2[max_buf];
    int r1,r2;
    if(!(fl1=fdopen(fd1,"r"))) return -1;
    if(!(fl2=fdopen(fd2,"r"))) return -1;
    while(1){
        r1=fscanf(fl1,"%s",st1);
        r2=fscanf(fl2,"%s",st2);
        if(r1==EOF){
            if(r2==EOF) return 1;
            else return 0;
        }else if(r2==EOF) return 0;
        if(strcmp(st1,st2)!=0) return 0;
    }
}
