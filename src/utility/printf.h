/*
*
*    printf.h - the header file of "printf.c"
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
