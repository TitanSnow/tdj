#include<stdio.h>
#include<stdlib.h>
int main(int argc,char** argv){
    FILE* fl;
    if(argc!=3){
        printf("Usage: %s Server_IP Server_port",argv[0]);
        return EXIT_FAILURE;
    }
    fl=fopen(".tdj","w");
    fprintf(fl,"%s\n%s",argv[1],argv[2]);
    fclose(fl);
    return EXIT_SUCCESS;
}
