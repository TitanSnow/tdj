#ifndef TDJ_CONFIG_H
#define TDJ_CONFIG_H
int tdj_get_config(int qid,const char* key,char* dist);
int tdj_set_config(int qid,const char* key,const char* value);
#endif
