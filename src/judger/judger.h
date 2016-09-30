/*
*
*    judger.h - a part of the judge tool "tdj". this is the header file of "judger.c"
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

#include<signal.h>
#ifndef TDJ_JUDGER_H
#define TDJ_JUDGER_H
#ifdef __cplusplus
extern "C"{
#endif
int tdj_compile(int qid,int fd,const char* lang,const char* path,int* pcpfd);
int tdj_judge(int qid,int did,const char* path,int *pstatus);
int tdj_listen_SIGCHLD(void (*oldsh)(int));
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
