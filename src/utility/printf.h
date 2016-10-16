#include<stdarg.h>
#ifndef TDJ_PRINTF_H
#define TDJ_PRINTF_H
#ifdef __cplusplus
extern "C"{
#endif
char* vmprintf(const char *zFormat, va_list ap);
char* mprintf(const char *zFormat, ...);
#ifdef __cplusplus
}
#endif
#endif
