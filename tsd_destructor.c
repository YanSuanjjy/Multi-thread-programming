#include <pthread.h>
#include "errors.h"

typedef struct private_tag{
    pthread_t thread_id;
    char *string;
}private_t;

pthread_key_t identity_key;
pthread_mutex_t identity_key_mutex=PTHREAD_MUTEX_INITIALIZER;
long identity_key_counter=0;

void identity_key_destructor(void *value){
    private_t *private=(private_t *)value;
    int status;

    printf("thread \%s\ exiting...\n",private->string);
    free(value);
    status=pthread_mutex_lock(&identity_key_mutex);
    if(status!=0){
        err_abort(status,"lock key mutex");
    }
    identity_key_counter--;
    if(identity_key_counter<=0){
        status=pthread_key_delete(identity_key);
        if(status!=0){
            err_abort(status,"Delete key");
        }
        printf("key deleted...\n");
    }
    status=pthread_mutex_unlock(&identity_key_mutex);
    if(status!=0){
        err_abort(status,"Unlock key mutex");
    }
}

void *identity_key_get(void){
    void *val;
    int status=0;
    val=pthread_getspecific(identity_key);
    if(val==NULL){
        val=malloc(sizeof(private_t));
        if(val==NULL){
            errno_abort("Allocate key value");
        }
        status=pthread_setspecific(identity_key,(void*)val);
        if(status!=0){
            err_abort(status,"Set TSD");
        }
    }
    return val;
}

void *thread_routine(void *arg){
    private_t *val;
    val=(private_t*)identity_key_get();
    val->thread_id=pthread_self();
    val->string=(char*)arg;
    printf("thread \%s\ starting..\n",val->string);
    sleep(2);
    return NULL;
}

int main(int argc,char *argv[]){
    pthread_t thread1,thread2;
    private_t *val;
    int status;

    status=pthread_key_create(&identity_key,identity_key_destructor);
    if(status!=0){
        err_abort(status,"Create key");
    }
    identity_key_counter=3;
    val=(private_t *)identity_key_get();
    val->thread_id=pthread_self();
    val->string="Main thread";
    status=pthread_create(&thread1,NULL,thread_routine,NULL);
    if(status!=0){
        err_abort(status,"Create thread 1");
    }
    status=pthread_create(&thread2,NULL,thread_routine,NULL);
    if(status!=0){
        err_abort(status,"Create thread 2");
    }
    pthread_exit(NULL);
    return 0;
}


