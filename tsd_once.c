#include <pthread.h>
#include "errors.h"

typedef struct tsd_tag{
    pthread_t thread_id;
    char *string;
}tsd_t;

pthread_key_t tsd_key;
pthread_once_t key_once;

void once_routine(void){
    int status;
    printf("initializing key\n");
    status=pthread_key_create(&tsd_key,NULL);
    if(status!=0){
        err_abort(status,"Create key");
    }
}

void *thread_routine(void *arg){
    tsd_t *val;
    int status;
    status=pthread_once(&key_once,once_routine);
    if(status!=0){
        err_abort(status,"Once init");
    }
    val=(tsd_t *)malloc(sizeof(tsd_t));
    if(val==NULL){
        errno_abort("Allocate key val");
    }
    status=pthread_setspecific(tsd_key,val);
    if(status!=0){
        err_abort(status,"Set tsd");
    }
    printf("%s set tsd value %p\n",arg,val);
    val->thread_id=pthread_self();
    val->string=(char *)arg;
    val=(tsd_t *)pthread_getspecific(tsd_key);
    printf("%s starting...\n",val->string);
    sleep(2);
    val=(tsd_t *)pthread_getspecific(tsd_key);
    printf("%s done...\n",val->string);
    return NULL;
}

void main(int argc,char *argv[]){
    pthread_t thread1,thread2;
    int status;
    status=pthread_create(&thread1,NULL,thread_routine,NULL);
    if(status!=0){
        err_abort(status,"Create thread 1");
    }
    status=pthread_create(&thread2,NULL,thread_routine,NULL);
    if(status!=0){
        err_abort(status,"Create thread 2");
    }
    pthread_exit(NULL);
}
