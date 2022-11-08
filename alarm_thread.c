#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct alarm_tag{
    int seconds;
    char message[64];
}alarm_t;

void *alarm_thread(void *arg){
    alarm_t *alarm=(alarm_t *)arg;
    int sta;
    sta=pthread_detach(pthread_self());
    if(sta!=0){
        printf("detach thread");
    }
    sleep(alarm->seconds);
    printf("(%d) %s\n",alarm->seconds,alarm->message);
    free(alarm);
    return NULL;
}

int main(int argc,char *argv[]){
    int status;
    char line[128];
    alarm_t *alarm;
    pthread_t thread;
    while(1){
        printf("Alarm> ");
        if(fgets(line,sizeof(line),stdin)==NULL)exit(0);
        if(strlen(line)<=1)continue;
        alarm=(alarm_t *)malloc(sizeof(alarm_t));
        if(alarm==NULL){
            printf("allocate alarm");
        }
        if(sscanf(line,"%d %64[^\n]",&alarm->seconds,alarm->message)<2){
            fprintf(stderr,"bad commend\n");
            free(alarm);
        }
        else{
            status=pthread_create(&thread,NULL,alarm_thread,alarm);
            if(status!=0){
                printf("create alarm thread");
            }
        }
    }
}
