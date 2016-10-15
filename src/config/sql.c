/*
*
*    sql.cc - a part of the judge tool "tdj", providing db keeping
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

#include"lib/sqlite/sqlite3.h"
#include<stdlib.h>
#ifndef NO_KEEPER_LOG
#include<stdio.h>
#endif
#ifdef BIND_SIGNAL
#include<signal.h>
#endif
#define TDJ_DB_NAME ".tdjconfig.db"
#ifdef BIND_SIGNAL
sqlite3* _inizsql(int flag);
void tdj_close_db(){
    _inizsql(-1);
}
void tdj_sql_exit_handler(int sig){
    tdj_close_db();
    signal(sig,SIG_DFL);
    raise(sig);
}
#endif
sqlite3* _inizsql(int flag){
    static int inited=0;
    static sqlite3 *db;
    const char* dbp;
    if(flag!=0){
        sqlite3_close(db);
        db=0;
#ifndef NO_KEEPER_LOG
        fputs("db_keeper: destruct\n",stderr);
#endif
        return 0;
    }
    if(inited==0){
        dbp=TDJ_DB_NAME;
#ifdef BIND_SIGNAL
        struct sigaction oldact;
#endif
        if(sqlite3_open(dbp,&db)!=SQLITE_OK) db=0;
#ifdef BIND_SIGNAL
        sigaction(SIGINT,0,&oldact);
        if(oldact.sa_handler==SIG_DFL)
            signal(SIGINT,tdj_sql_exit_handler);
        sigaction(SIGTERM,0,&oldact);
        if(oldact.sa_handler==SIG_DFL)
            signal(SIGTERM,tdj_sql_exit_handler);
#endif
#ifndef NO_KEEPER_LOG
        fputs("db_keeper: construct\n",stderr);
#endif
        atexit(tdj_close_db);
        inited=1;
    }
    return db;
}
sqlite3* inizsql(){
    return _inizsql(0);
}
