#include"config.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int tdj_get_config(int qid,const char* key,char* dist){
    FILE* fl;
    char* ln;
    const size_t max_buf=1024;
    char tk[max_buf],tv[max_buf],tt[max_buf],ttt[max_buf],home_path[max_buf];
    size_t i,len;
    const char* default_config[]={
        "judge_data_path","./judge_data","time_limit","1000",0
    };
    if(qid==0){
        strcpy(home_path,getenv("HOME"));
        strcat(home_path,"/.tdjconfig");
        if((fl=fopen(home_path,"r"))){
            while(!feof(fl)){
                if((ln=fgets(malloc(sizeof(char)*max_buf),max_buf,fl))){
                    len=strlen(ln);
                    for(i=0;i<len;++i){
                        if(ln[i]==' '){
                            if(i+1==len||ln[i+1]!=' '){
                                break;
                            }else ++i;
                        }
                    }
                    ln[i]='\0';
                    strcpy(tk,ln);
                    strcpy(tv,i==len?ln+i:ln+i+1);
                    len=strlen(tk);
                    for(i=0;i<len;++i)
                        if(tk[i]==' '&&tk[i+1]==' '){
                            strcpy(tk+i+1,tk+i+2);
                            --len;
                        }
                    free(ln);
                    ln=0;
                    if(!strcmp(tk,key)){
                        len=strlen(tv);
                        for(i=0;i<len;++i)
                            if(tv[i]==' '&&tv[i+1]==' '){
                                strcpy(tv+i+1,tv+i+2);
                                --len;
                            }
                        tv[len-1]='\0';
                        strcpy(dist,tv);
                        return 0;
                    }
                }
            }
        }
        for(i=0;default_config[i];++i)
            if(i%2==0&&!strcmp(key,default_config[i])){
                strcpy(dist,default_config[i+1]);
                return 0;
            }
    }else{
        tdj_get_config(0,"judge_data_path",tt);
        len=strlen(tt);
        if(tt[len-1]!='/'){
            strcat(tt,"/");
            ++len;
        }
        sprintf(ttt,"%s%d/.tdjconfig",tt,qid);
        if((fl=fopen(ttt,"r"))){
            while(!feof(fl)){
                if((ln=fgets(malloc(sizeof(char)*max_buf),max_buf,fl))){
                    len=strlen(ln);
                    for(i=0;i<len;++i){
                        if(ln[i]==' '){
                            if(i+1==len||ln[i+1]!=' '){
                                break;
                            }else ++i;
                        }
                    }
                    ln[i]='\0';
                    strcpy(tk,ln);
                    strcpy(tv,i==len?ln+i:ln+i+1);
                    len=strlen(tk);
                    for(i=0;i<len;++i)
                        if(tk[i]==' '&&tk[i+1]==' '){
                            strcpy(tk+i+1,tk+i+2);
                            --len;
                        }
                    free(ln);
                    ln=0;
                    if(!strcmp(tk,key)){
                        len=strlen(tv);
                        for(i=0;i<len;++i)
                            if(tv[i]==' '&&tv[i+1]==' '){
                                strcpy(tv+i+1,tv+i+2);
                                --len;
                            }
                        tv[len-1]='\0';
                        strcpy(dist,tv);
                        return 0;
                    }
                }
            }
        }
        return tdj_get_config(0,key,dist);
    }
    return -1;
}
int tdj_set_config(int qid,const char* key,const char* value){
    return -1;
}
