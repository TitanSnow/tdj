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

#include"../../lib/sqlite/sqlite3.h"
#include<stdlib.h>
#ifndef NO_KEEPER_LOG
#include<stdio.h>
#endif
#ifdef BIND_SIGNAL
#include<signal.h>
#endif
#define TDJ_DB_NAME ".tdjconfig.db"
void tdj_sql_exit_handler(int sig){
    exit(EXIT_FAILURE);
}
struct db_keeper{
    sqlite3* db;
    db_keeper(){
        char* dbp=sqlite3_mprintf("%s/" TDJ_DB_NAME,getenv("HOME"));
        void (*sh)(int);
        if(sqlite3_open(dbp,&db)!=SQLITE_OK) db=0;
        sqlite3_free(dbp);
#ifdef BIND_SIGNAL
        if((sh=signal(SIGHUP,tdj_sql_exit_handler))!=SIG_DFL)
            signal(SIGHUP,sh);
        if((sh=signal(SIGINT,tdj_sql_exit_handler))!=SIG_DFL)
            signal(SIGINT,sh);
        if((sh=signal(SIGTERM,tdj_sql_exit_handler))!=SIG_DFL)
            signal(SIGTERM,sh);
#endif
#ifndef NO_KEEPER_LOG
        fputs("db_keeper: construct\n",stderr);
#endif
    }
    ~db_keeper(){
        sqlite3_close(db);
#ifndef NO_KEEPER_LOG
        fputs("db_keeper: destruct\n",stderr);
#endif
    }
}_dbk;
sqlite3* _inizsql(){
    return _dbk.db;
}
extern "C" sqlite3* inizsql(){
    return _inizsql();
}
