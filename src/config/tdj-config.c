/*
*
*    tdj-config.c - a part of the judge tool "tdj", providing the interface of config getting&setting
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

#include"config.h"
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
int isNum(const char* s){
    size_t i;
    for(i=0;s[i];++i)
        if(!isdigit(s[i])) return 0;
    return 1;
}
void error_usage(const char* pn){
    printf("Usage: %s [problem id] key [value]\n",pn);
}
int main(int argc,char** argv){
    int qid=0;
    char* key=0;
    char* value=0;
    const char *dist;
    if(argc==2)
        key=argv[1];
    else if(argc==3){
        if(isNum(argv[1])){
            qid=atoi(argv[1]);
            key=argv[2];
        }else{
            key=argv[1];
            value=argv[2];
        }
    }else if(argc==4){
        if(!isNum(argv[1])){
            error_usage(argv[0]);
            return EXIT_FAILURE;
        }
        qid=atoi(argv[1]);
        key=argv[2];
        value=argv[3];
    }else{
        error_usage(argv[0]);
        return EXIT_FAILURE;
    }
    if(value){
        if(tdj_set_config(qid,key,value)==-1){
            puts("Error when setting config");
            return EXIT_FAILURE;
        }
    }else{
        if((dist=tdj_get_config2(qid,key))==0){
            puts("Error: undefined key");
            return EXIT_FAILURE;
        }
        else
            puts(dist);
    }
    return EXIT_SUCCESS;
}
