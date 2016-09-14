#include"config.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
int tdj_get_config(int qid,const char* key,char* dist){
    FILE* fl;
    const size_t max_buf=1024;
    char tk[max_buf],tv[max_buf],tt[max_buf],ttt[max_buf],home_path[max_buf],ln[max_buf];
    size_t i,len;
    const char* default_config[]={
        "judge_data_path","./judge_data","time_limit","1000000","compiler","g++","compare_method","lesser","judge_build_path","./judge_build","backlog","64","server_wait_time","0",0
    };
    if(qid==0){
        strcpy(home_path,getenv("HOME"));
        strcat(home_path,"/.tdjconfig");
        if((fl=fopen(home_path,"r"))){
            while(!feof(fl)){
                if((fgets(ln,max_buf,fl))){
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
                if((fgets(ln,max_buf,fl))){
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
    const size_t max_buf=1024;
    char fn[max_buf],t[max_buf],tk[max_buf],tt[max_buf],tv[max_buf];
    FILE* fl,*tfl;
    size_t len,i;
    int ch;
    fn[0]='\0';
    if(qid==0){
        strcpy(fn,getenv("HOME"));
        strcat(fn,"/.tdjconfig");
        if((fl=fopen(fn,"r"))&&(tfl=fopen(".tdj_set_config_temp","w"))){
            while(!feof(fl)){
                if(fgets(t,max_buf,fl)){
                    len=strlen(t);
                    for(i=0;i<len;++i)
                        if(t[i]==' '){
                            if(t[i+1]!=' ')
                                break;
                            else ++i;
                        }
                    t[i]='\0';
                    strcpy(tk,t);
                    if(i!=len) t[i]=' ';
                    len=strlen(tk);
                    for(i=0;i<len;++i)
                        if(tk[i]==' '&&tk[i+1]==' '){
                            strcpy(tk+i+1,tk+i+2);
                            --len;
                        }
                    if(!!strcmp(key,tk)) fprintf(tfl,"%s",t);
                }
            }
            if(!freopen(fn,"w",fl)) return -1;
            if(!freopen(".tdj_set_config_temp","r",tfl)) return -1;
            while((ch=fgetc(tfl))!=EOF)
                fputc(ch,fl);
            fclose(tfl);
            unlink(".tdj_set_config_temp");
            strcpy(tk,key);
            len=strlen(tk);
            for(i=0;i<len;++i)
                if(tk[i]==' '){
                    strcpy(tt,tk+i);
                    strcpy(tk+i+1,tt);
                    ++len;
                }
            strcpy(tv,value);
            len=strlen(tv);
            for(i=0;i<len;++i)
                if(tv[i]==' '){
                    strcpy(tt,tv+i);
                    strcpy(tv+i+1,tt);
                    ++len;
                }
            fprintf(fl,"%s %s\n",tk,tv);
            fclose(fl);
        }else{
            if(errno==ENOENT){
                if((fl=fopen(fn,"w"))){
                    fclose(fl);
                    return tdj_set_config(qid,key,value);
                }
            }
            return -1;
        }
    }else{
        tdj_get_config(0,"judge_data_path",fn);
        sprintf(fn,"%s/%d/.tdjconfig",fn,qid);
        if((fl=fopen(fn,"r"))&&(tfl=fopen(".tdj_set_config_temp","w"))){
            while(!feof(fl)){
                if(fgets(t,max_buf,fl)){
                    len=strlen(t);
                    for(i=0;i<len;++i)
                        if(t[i]==' '){
                            if(t[i+1]!=' ')
                                break;
                            else ++i;
                        }
                    t[i]='\0';
                    strcpy(tk,t);
                    if(i!=len) t[i]=' ';
                    len=strlen(tk);
                    for(i=0;i<len;++i)
                        if(tk[i]==' '&&tk[i+1]==' '){
                            strcpy(tk+i+1,tk+i+2);
                            --len;
                        }
                    if(!!strcmp(key,tk)) fprintf(tfl,"%s",t);
                }
            }
            if(!freopen(fn,"w",fl)) return -1;
            if(!freopen(".tdj_set_config_temp","r",tfl)) return -1;
            while((ch=fgetc(tfl))!=EOF)
                fputc(ch,fl);
            fclose(tfl);
            unlink(".tdj_set_config_temp");
            strcpy(tk,key);
            len=strlen(tk);
            for(i=0;i<len;++i)
                if(tk[i]==' '){
                    strcpy(tt,tk+i);
                    strcpy(tk+i+1,tt);
                    ++len;
                }
            strcpy(tv,value);
            len=strlen(tv);
            for(i=0;i<len;++i)
                if(tv[i]==' '){
                    strcpy(tt,tv+i);
                    strcpy(tv+i+1,tt);
                    ++len;
                }
            fprintf(fl,"%s %s\n",tk,tv);
            fclose(fl);
        }else{
            if(errno==ENOENT){
                if((fl=fopen(fn,"w"))){
                    fclose(fl);
                    return tdj_set_config(qid,key,value);
                }
            }
            return -1;
        }
    }
    return 0;
}
