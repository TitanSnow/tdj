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
