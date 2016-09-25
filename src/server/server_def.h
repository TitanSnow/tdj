/*
*
*    server_def.h - a part of the judge tool "tdj". this is a defines header file
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
