/*
*
*    config.h - a part of the judge tool "tdj". this is the header file of "config.c"
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

#ifndef TDJ_CONFIG_H
#define TDJ_CONFIG_H
#ifdef __cplusplus
extern "C"{
#endif
int tdj_get_config(int qid,const char* key,char* dist);
int tdj_set_config(int qid,const char* key,const char* value);
#ifdef __cplusplus
}
#endif
#endif
