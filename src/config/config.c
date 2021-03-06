/*
*
*    config.c - a part of the judge tool "tdj", providing config getting&setting
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
#include"lib/sqlite/sqlite3.h"
#include<string.h>
#include<stdlib.h>

sqlite3* inizsql();     // from sql.cc

int make_sure(){
    sqlite3 *db=inizsql();
    int nRow,i;
    char **result,*query;
    const char* default_config[]={
        "judge_data_path","judge_data","time_limit","1000000","compiler","c++","compare_method","lesser","judge_build_path","judge_build","backlog","64","server_wait_time","0","compress_level","-1","high_precision_timer","","rlimit_as","134217728","rlimit_core","0","rlimit_nofile","20",0
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

char* _get_storehouse(int flag,size_t len){
    static char** rmap=0;
    static size_t lmap=0;
    size_t i;
    if(flag!=0){
        for(i=0;i<lmap;++i)
            free(rmap[i]);
        free(rmap);
        rmap=0;
        lmap=0;
        return 0;
    }
    rmap=realloc(rmap,(++lmap)*sizeof(char*));
    if(rmap==0){
        lmap=0;
        return 0;
    }
    rmap[lmap-1]=malloc(len);
    return rmap[lmap-1];
}

void _clean_storehouse(){
    _get_storehouse(-1,0);
}

char* tdj_get_storehouse(size_t len){
    static int inited=0;
    if(inited==0){
        atexit(_clean_storehouse);
        inited=1;
    }
    return _get_storehouse(0,len);
}

char* tdj_get_config2(int qid,const char* key){
    sqlite3 *db=inizsql();
    char **result,*query;
    int nRow;
    char *dist;
    
    if(db==0)return 0;
    if(make_sure()==-1)return 0;
    query=sqlite3_mprintf("select value from config where (qid=0 or qid=%d) and key=%Q order by qid desc limit 1",qid,key);
    if(sqlite3_get_table(db,query,&result,&nRow,0,0)!=SQLITE_OK){
        sqlite3_free(query);
        return 0;
    }
    if(nRow<=0){
        sqlite3_free(query);
        sqlite3_free_table(result);
        return 0;
    }
    
    sqlite3_free(query);
    if((dist=tdj_get_storehouse(strlen(result[1])+1)))
        strcpy(dist,result[1]);
    sqlite3_free_table(result);
    return dist;
}

int tdj_get_config(int qid,const char* key,char* dist){
    char *r;
    if((r=tdj_get_config2(qid,key))==0) return -1;
    if(dist)strcpy(dist,r);
    return 0;
}

int tdj_set_config(int qid,const char* key,const char* value){
    sqlite3 *db=inizsql();
    char **result,*query;
    int nRow;
    
    if(db==0)return -1;
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
