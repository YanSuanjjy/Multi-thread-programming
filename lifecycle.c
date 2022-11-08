#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

void *thread_routine(void *arg){
    printf("1");
    return arg;
}

main(int argc,char *argv[]){
    pthread_t thread_id;
    int status;
    void *thread_res;
    status=pthread_create(&thread_id,NULL,thread_routine,NULL);
    if(status!=0){
        err_abort(status,"Create thread");
    }
    status=pthread_join(thread_id,&thread_res);
    if(status!=0)err_abort(status,"Join thread");
    if(thread_res==NULL)return 0;
    else return 1;
}
