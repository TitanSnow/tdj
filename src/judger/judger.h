#include<signal.h>
#ifndef TDJ_JUDGER_H
#define TDJ_JUDGER_H
#ifdef __cplusplus
extern "C"{
#endif
int tdj_compile(int qid,int fd,const char* lang,const char* path,int* pcpfd);
int tdj_judge(int qid,int did,const char* path,int *pstatus);
int tdj_listen_SIGCHLD(struct sigaction* oldact);
#ifdef __cplusplus
}
#endif
#define TDJ_JUDGESUCCESS 0
#define TDJ_PIPEGETTINGERROR 1
#define TDJ_TIMELIMITGETTINGERROR 2
#define TDJ_USLEEPERROR 3
#define TDJ_STILLRUNNINGERROR 4
#define TDJ_EXITERROR 5
#define TDJ_WAITERROR 6
#define TDJ_FORKERROR 7
#define TDJ_JUDGEDATAPATHERROR 8
#define TDJ_NOSTDIN 9
#define TDJ_OPENNULLERROR 10
#define TDJ_DUPERROR 11
#endif
