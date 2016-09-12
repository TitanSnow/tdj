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
