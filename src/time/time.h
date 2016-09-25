/*
*
*    time.h - a part of the judge tool "tdj". this is the header file of "time.c"
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

#include<stdint.h>
#ifndef TDJ_TIME_H
#define TDJ_TIME_H
#ifdef __cplusplus
extern "C"{
#endif
typedef uint64_t tdj_usec_t;
tdj_usec_t tdj_time();
#ifdef __cplusplus
}
#endif
#endif
