/*
*
*    time.c - a part of the judge tool "tdj", providing time getting
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

#include"time.h"
#include<sys/time.h>
tdj_usec_t tdj_time(){
    struct timeval tv;
    tdj_usec_t tm;
    gettimeofday(&tv,0);
    tm=tv.tv_sec;
    tm*=1000000;
    tm+=tv.tv_usec;
    return tm;
}
