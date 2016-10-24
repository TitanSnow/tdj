/*
*
*    zpipe.h - a part of the judge tool "tdj". this is the header file of "zpipe.c"
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

#include<stdio.h>
#ifndef TDJ_ZPIPE_H
#define TDJ_ZPIPE_H
int def(FILE *source, FILE *dest, int level);
int inf(FILE *source, FILE *dest);
#endif
