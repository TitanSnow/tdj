#include"../../lib/sqlite/sqlite3.h"
#include<stdlib.h>
#define TDJ_DB_NAME ".tdjconfig.db"
sqlite3* _inizsql(){
    static struct db_keeper{
        sqlite3* db;
        db_keeper(){
            char* dbp=sqlite3_mprintf("%s/" TDJ_DB_NAME,getenv("HOME"));
            if(sqlite3_open(dbp,&db)!=SQLITE_OK) db=0;
            sqlite3_free(dbp);
        }
        ~db_keeper(){
            sqlite3_close(db);
        }
    }dbk;
    return dbk.db;
}
extern "C" sqlite3* inizsql(){
    return _inizsql();
}
