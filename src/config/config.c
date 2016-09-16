#include"config.h"
#include"../../lib/sqlite/sqlite3.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define TDJ_DB_NAME ".tdjconfig.db"

#define TDJ_EXISTS_TABLE \
"select name from sqlite_master where " \
"type='table' and name='%d' " \
"limit 1"

#define TDJ_SELECT \
"select value from `%d` " \
"where key=%Q limit 1"

#define TDJ_CREATE_TABLE \
"create table `%d` (key,value)"

#define TDJ_INSERT \
"insert into `%d` values (%Q,%Q)"

#define TDJ_UPDATE \
"update `%d` set value=%Q " \
"where key=%Q"

sqlite3* inizsql(){
    static int c_req=0;
    static sqlite3* db;
    const size_t max_buf=256;
    char dbp[max_buf];
    if(c_req++==0){
        sprintf(dbp,"%s/" TDJ_DB_NAME,getenv("HOME"));
        if(sqlite3_open(dbp,&db)!=SQLITE_OK) db=0;
    }
    return db;
}
int make_sure_table_0(){
    sqlite3* db=inizsql();
    const char* default_config[]={
        "judge_data_path","./judge_data","time_limit","1000000","compiler","g++","compare_method","lesser","judge_build_path","./judge_build","backlog","64","server_wait_time","0",0
    };
    char **result=0;
    int nr,nc,i;
    char *errmsg;
    char *exists_table=sqlite3_mprintf(TDJ_EXISTS_TABLE,0),*create_table,*insert;
    if(sqlite3_get_table(db,exists_table,&result,&nr,&nc,&errmsg)!=SQLITE_OK){
        sqlite3_free_table(result);
        sqlite3_free(exists_table);
        return -1;
    }
    sqlite3_free_table(result);
    sqlite3_free(exists_table);
    if(nr==1){
        // has table '0'
        return 0;
    }
    // no table '0'
    create_table=sqlite3_mprintf(TDJ_CREATE_TABLE,0);
    if(sqlite3_exec(db,create_table,0,0,&errmsg)!=SQLITE_OK){
        sqlite3_free(create_table);
        return -1;
    }
    sqlite3_free(create_table);
    // insert default
    for(i=0;default_config[i]!=0;++i)
        if(i%2==0){
            insert=sqlite3_mprintf(TDJ_INSERT,0,default_config[i],default_config[i+1]);
            if(sqlite3_exec(db,insert,0,0,&errmsg)!=SQLITE_OK){
                sqlite3_free(insert);
                return -1;
            }
            sqlite3_free(insert);
        }
    
    return 0;
}
int tdj_get_config(int qid,const char* key,char* dist){
    sqlite3* db=inizsql();
    char *exists_table=sqlite3_mprintf(TDJ_EXISTS_TABLE,qid),*select;
    char **result=0;
    int nr,nc;
    char *errmsg;
    if(sqlite3_get_table(db,exists_table,&result,&nr,&nc,&errmsg)!=SQLITE_OK){
        sqlite3_free(exists_table);
        sqlite3_free_table(result);
        return -1;
    }
    sqlite3_free(exists_table);
    sqlite3_free_table(result);
    if(nr==1){
        // has table 'qid'
        result=0;
        select=sqlite3_mprintf(TDJ_SELECT,qid,key);
        if(sqlite3_get_table(db,select,&result,&nr,&nc,&errmsg)==SQLITE_OK){
            sqlite3_free(select);
            if(nr==1){
                // has data
                strcpy(dist,result[1]);
                sqlite3_free_table(result);
                return 0;
            }
            sqlite3_free_table(result);
        }else{
            sqlite3_free(select);
            sqlite3_free_table(result);
        }
    }
    // select from table '0'
    if(make_sure_table_0()==-1) return -1;
    result=0;
    select=sqlite3_mprintf(TDJ_SELECT,0,key);
    if(sqlite3_get_table(db,select,&result,&nr,&nc,&errmsg)==SQLITE_OK){
        sqlite3_free(select);
        if(nr==1){
            // has data
            strcpy(dist,result[1]);
            sqlite3_free_table(result);
            return 0;
        }
        sqlite3_free_table(result);
    }else{
        sqlite3_free(select);
        sqlite3_free_table(result);
    }
    // No data found
    return -1;
}

int tdj_set_config(int qid,const char* key,const char* value){
    sqlite3* db=inizsql();
    char *exists_table,*create_table,*select,*insert,*update;
    char **result=0;
    int nr,nc,r;
    char *errmsg;
    
    exists_table=sqlite3_mprintf(TDJ_EXISTS_TABLE,qid);
    r=sqlite3_get_table(db,exists_table,&result,&nr,&nc,&errmsg);
    sqlite3_free(exists_table);
    sqlite3_free_table(result);
    if(r!=SQLITE_OK)
        return -1;
    if(nr==0){
        // no table 'qid'
        create_table=sqlite3_mprintf(TDJ_CREATE_TABLE,qid);
        r=sqlite3_exec(db,create_table,0,0,&errmsg);
        sqlite3_free(create_table);
        if(r!=SQLITE_OK)
            return -1;
    }
    result=0;
    select=sqlite3_mprintf(TDJ_SELECT,qid,key);
    r=sqlite3_get_table(db,select,&result,&nr,&nc,&errmsg);
    sqlite3_free_table(result);
    sqlite3_free(select);
    if(r!=SQLITE_OK)
        return -1;
    if(nr==0){
        // insert it
        insert=sqlite3_mprintf(TDJ_INSERT,qid,key,value);
        r=sqlite3_exec(db,insert,0,0,&errmsg);
        sqlite3_free(insert);
        if(r!=SQLITE_OK)
            return -1;
        return 0;
    }
    // update it
    update=sqlite3_mprintf(TDJ_UPDATE,qid,value,key);
    r=sqlite3_exec(db,update,0,0,&errmsg);
    sqlite3_free(update);
    if(r!=SQLITE_OK)
        return -1;
    return 0;
}

