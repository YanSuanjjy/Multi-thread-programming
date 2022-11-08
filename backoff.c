#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

#define INTERATIONS 10

pthread_mutex_t mutex[3]={
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

int backoff=1;
int yield_flag=0;

void *lock_forward(void *arg){
    int i,iterate,backoffs;
    int status;
    for(iterate = 0;iterate<INTERATIONS;iterate++){
        backoffs=0;
        for(i=0;i<3;i++){
            if(i==0){
                status=pthread_mutex_lock(&mutex[i]);
                if(status!=0){
                    err_abort(status,"first lock");
                }
            }else{
                if(backoff){
                    status=pthread_mutex_trylock(&mutex[i]);
                }else{
                    status=pthread_mutex_lock(&mutex[i]);
                }
                if(status==EBUSY){
                    backoffs++;
                   // DPRINTF(("(forward locker backing off at %d )\n",i));
                    for(;i>=0;i--){
                        status=pthread_mutex_unlock(&mutex[i]);
                        if(status!=0){
                            err_abort(status,"Backoff");
                        }
                    }
                }else{
                    if(status!=0){
                         err_abort(status,"lock mutex");
                    }

                   // DPRINTF(("(forward locker got %d )\n",i));
                }
                if(yield_flag){
                    if(yield_flag>0){
                        sched_yield();
                    }else sleep(1);
                }
            }
        }
        printf("lock forword got all locks, %d backoffs\n",backoffs);
        pthread_mutex_unlock(&mutex[2]);
        pthread_mutex_unlock(&mutex[1]);
        pthread_mutex_unlock(&mutex[0]);
    }
    return NULL;
}

void *lock_backward(void *arg){
    int i,iterate,backoffs;
    int status;
    for(iterate=0;iterate<INTERATIONS;iterate++){
        backoffs=0;
        for(i=2;i>=0;i--){
            if(i==2){
                status=pthread_mutex_lock(&mutex[i]);
                if(status!=0){
                    err_abort(status,"First lock");
                }
            }else{
                if(backoff){
                    status=pthread_mutex_trylock(&mutex[i]);
                }else{
                    status=pthread_mutex_lock(&mutex[i]);
                }
                if(status==EBUSY){
                    backoffs++;

                   // DPRINTF(("(backward locker backing off at %d )\n",i));
                    for(;i<3;i++){
                        status=pthread_mutex_unlock(&mutex[i]);
                        if(status!=0){
                            err_abort(status,"Backoff");
                        }
                    }
                }else{
                    if(status!=0){
                        err_abort(status,"Lock mutex");
                    }
                   // DPRINTF(("(backward locker got %d )\n",i));
                }
                if(yield_flag){
                    if(yield_flag>0){
                        sched_yield();
                    }else sleep(1);
                }
            }
        }
        printf("lock backward got all locks, %d backoffs\n",backoffs);
        pthread_mutex_unlock(&mutex[0]);
        pthread_mutex_unlock(&mutex[1]);
        pthread_mutex_unlock(&mutex[2]);
    }
    return NULL;
}

int mian(int argc,char *argv[]){
        pthread_t forward,backward;
        int status;
#ifdef sun
        DPRINTF(("Setting concurrency level to 2\n"));
        thr_serconcurrency(2);
#endif
        if(argc>1){
            backoff=atoi(argv[1]);
        }
        if(argc>2){
            yield_flag=atoi(argv[2]);
        }
        status=pthread_create(&forward,NULL,lock_forward,NULL);
        if(status!=0){
            err_abort(status,"Create forward");
        }
        
        status=pthread_create(&backward,NULL,lock_backward,NULL);
        
        if(status!=0){
            err_abort(status,"Create backward");
        }
        pthread_exit(NULL);
        return 0;
}
