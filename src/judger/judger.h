#ifndef TDJ_JUDGER_H
#define TDJ_JUDGER_H
int tdj_compile(int qid,int fd,const char* lang,const char* path);
int tdj_judge(int qid,int did,const char* path,int *pstatus);
#define TDJ_JUDGESUCCESS 0
#define TDJ_PIPEGETTINGERROR 1
#define TDJ_TIMELIMITGETTINGERROR 2
#define TDJ_USLEEPERROR 3
#define TDJ_STILLRUNNINGERROR 4
#define TDJ_EXITERROR 5
#define TDJ_WAITERROR 6
#endif
