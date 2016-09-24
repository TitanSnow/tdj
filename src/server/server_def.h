#ifndef TDJ_SERVER_DEF_H
#define TDJ_SERVER_DEF_H
#define TDJ_AC 0
#define TDJ_WA 1
#define TDJ_JE 2
#define TDJ_OTHERROR 3
#define TDJ_COMPILEERROR 4
#define TDJ_MAKEVERSION(first,second) (first<<16|second)
#define TDJ_VERSION TDJ_MAKEVERSION(0,2)
#define TDJ_C 0
#define TDJ_CPP 1
#endif
