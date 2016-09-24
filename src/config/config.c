#include"config.h"
#include"../../lib/sqlite/sqlite3.h"
#include<string.h>

sqlite3* inizsql();     // from sql.cc

int make_sure(){
    sqlite3 *db=inizsql();
    int nRow,i;
    char **result,*query;
    const char* default_config[]={
        "judge_data_path","./judge_data","time_limit","1000000","compiler","g++","compare_method","lesser","judge_build_path","./judge_build","backlog","64","server_wait_time","0","compress_level","-1",0
    };
    
    if(sqlite3_get_table(db,"select * from sqlite_master where type='table' and name='config' limit 1",&result,&nRow,0,0)!=SQLITE_OK)return -1;
    sqlite3_free_table(result);
    if(nRow>0)
        // has table
        return 0;
    // no table
    sqlite3_exec(db,"begin",0,0,0);
    // create table
    sqlite3_exec(db,"create table config (qid,key,value)",0,0,0);
    // insert default configs
    for(i=0;default_config[i];++i)
        if(i%2==0){
            query=sqlite3_mprintf("insert into config values (0,%Q,%Q)",default_config[i],default_config[i+1]);
            sqlite3_exec(db,query,0,0,0);
            sqlite3_free(query);
        }
    
    if(sqlite3_exec(db,"commit",0,0,0)==SQLITE_OK)return 0;
    return -1;
}

int tdj_get_config(int qid,const char* key,char* dist){
    sqlite3 *db=inizsql();
    char **result,*query;
    int nRow;
    
    if(make_sure()==-1)return -1;
    query=sqlite3_mprintf("select value from config where (qid=0 or qid=%d) and key=%Q order by qid desc limit 1",qid,key);
    if(sqlite3_get_table(db,query,&result,&nRow,0,0)!=SQLITE_OK){
        sqlite3_free(query);
        return -1;
    }
    if(nRow<=0){
        sqlite3_free(query);
        sqlite3_free_table(result);
        return -1;
    }
    
    sqlite3_free(query);
    if(dist)strcpy(dist,result[1]);
    sqlite3_free_table(result);
    return 0;
}

int tdj_set_config(int qid,const char* key,const char* value){
    sqlite3 *db=inizsql();
    char **result,*query;
    int nRow;
    
    if(make_sure()==-1)return -1;
    query=sqlite3_mprintf("select value from config where qid=%d and key=%Q limit 1",qid,key);
    if(sqlite3_get_table(db,query,&result,&nRow,0,0)!=SQLITE_OK){
        sqlite3_free(query);
        return -1;
    }
    sqlite3_free(query);
    sqlite3_free_table(result);
    
    if(nRow>0){
        // update it
        query=sqlite3_mprintf("update config set value=%Q where qid=%d and key=%Q",value,qid,key);
        if(sqlite3_exec(db,query,0,0,0)!=SQLITE_OK){
            sqlite3_free(query);
            return -1;
        }
        sqlite3_free(query);
        return 0;
    }
    
    // insert it
    query=sqlite3_mprintf("insert into config values (%d,%Q,%Q)",qid,key,value);
    if(sqlite3_exec(db,query,0,0,0)!=SQLITE_OK){
        sqlite3_free(query);
        return -1;
    }
    sqlite3_free(query);
    return 0;
}
