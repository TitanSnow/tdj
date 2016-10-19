/*
*
*    printf.c - a part of the judge tool "tdj", providing mprintf/vmprintf
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

#include"printf.h"
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
char* mprintf(const char *zFormat, ...){
    va_list ap;
    char *z;
    va_start(ap, zFormat);
    z = vmprintf(zFormat, ap);
    va_end(ap);
    return z;
}
char* vmprintf(const char *zFormat, va_list ap){
    FILE *fm;
    char *buf,*z;
    size_t sizeloc;
    if((fm=open_memstream(&buf,&sizeloc))==NULL) return 0;
    if(vfprintf(fm,zFormat,ap)<0){
        fclose(fm);
        free(buf);
        return 0;
    }
    if(fflush(fm)!=0){
        fclose(fm);
        free(buf);
        return 0;
    }
    if((z=malloc((strlen(buf)+1)*sizeof(char)))==NULL){
        fclose(fm);
        free(buf);
        return 0;
    }
    strcpy(z,buf);
    fclose(fm);
    free(buf);
    return z;
}
