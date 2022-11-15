#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "errors.h"

void *thread_routine(void *arg){
    int my_policy;
    struct sched_param my_param;
    int status;

#if defined (_POSIX_THREAD_PRIORITY_SCHEDULING) && !defined(sun)
    status=pthread_getschedparam(pthread_self(),&my_policy,&my_param);
    if(status!=0){
        err_abort(status,"Get sched");
    }
    printf("thread_routine running at %s/%d",(my_policy==SCHED_FIFO?"FIFO":(my_policy==SCHED_RR?"RR":(my_policy==SCHED_OTHRE?"OTHRE":"unknown"))),my_param.sched_priority);
#else 
    printf("thread_routine running\n");
#endif

    return NULL;
}

int main(int argc,char *argv[]){
    pthread_t thread_id;
    pthread_attr_t thread_attr;
    int thread_policy;
    struct sched_param thread_param;
    int status,rr_min_priority,rr_max_priority;
    status=pthread_attr_init(&thread_attr);
    if(status!=0){
        err_abort(status,"Init attr");
    }

#if defined (_POSIX_THREAD_PRIORITY_SCHEDULING) && !defined(sun)
    status=pthread_attr_getschedpolicy(&thread_attr,&thread_policy);
    if(status!=0){
        err_abort(status,"Get policy");
    }
    status=pthread_attr_getschedparam(&thread_attr,&thread_param);
    if(status!=0){
        err_abort(status,"Get sched param");
    }
    printf("Default policy is %s,priority is %d\n",(thread_policy==SCHED_FIFO?"FIFO":
                (thread_policy==SCHED_RR?"RR":
                    (thread_policy==SCHED_OTHRE?"OTHRE":"unknown"))),thread_param.sched_priority);
    status=pthread_attr_setschedpolicy(&thread_attr,SCHED_RR);
    if(status!=0){
        printf("Unable to set SCHED_RR policy.\n");
    }
    else{
        rr_min_priority=sched_get_priority_min(SCHED_RR);
        if(rr_min_priority==-1){
            errno_abort("Get SCHED_RR min priority");
        }
        rr_max_priority=sched_get_priority_max(SCHED_RR);
        if(rr_max_priority==-1){
            errno_abort("Get SCHED_RR max priority");
        }
        thread_param.sched_priority=(rr_min_priority+rr_max_priority)/2;
        printf("SCHED_RR priority range is %d to %d: using %d\n",rr_min_priority,rr_max_priority,thread_param.sched_priority);
        status=pthread_attr_setschedparam(&thread_attr,&thread_param);
        if(status!=0){
            err_abort(status,"Set params");
        }
        printf("Creating thread at RR/%d\n",thread_param.sched_priority);
        status=pthread_attr_setinheritsched(&thread_attr,PTHREAD_EXPLICIT_SCHED);
        if(status!=0){
            err_abort(status,"Set inherit");
        }
    }
#else
    printf("Priority scheduling not supported\n");
#endif
    status=pthread_create(&thread_id,&thread_attr,thread_routine,NULL);
    if(status!=0){
        err_abort(status,"Create thread");
    }
    status=pthread_join(thread_id,NULL);
    if(status!=0){
        err_abort(status,"Join thread");
    }
    printf("Main exiting\n");
    return 0;
}
