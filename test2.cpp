#include <iostream>
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "MyMonitor.h"

using namespace std;
//#define BUFSIZE 10
#define CONSUMER_QUANTITY 5
#define PRODUCER_QUANTITY 2
#define QUEUE_QUANTITY 5

void randQueuesNumbers(int *tab) {
    for(int i=0; i<QUEUE_QUANTITY; ++i)
        tab[i]=i;
    int noChange, tmp;
    for(int i=0; i<QUEUE_QUANTITY; ++i){
        noChange = rand()%QUEUE_QUANTITY;
        tmp=tab[i];
        tab[i]=tab[noChange];
        tab[noChange]=tmp;
    }
}
SingleMonitor sMonitor[QUEUE_QUANTITY];
GroupMonitor gMonitor;

///////////////////////////////////////////////////////////
void *Producer(void *idp){
    srand(time(NULL));
    int id = * ((int*)idp);
    cout<<"Producent "<<id<<endl;

    int numeryKolejek[QUEUE_QUANTITY];
    while(1){
        randQueuesNumbers(numeryKolejek);
        //sMonitor[id].add(id, gMonitor);
        gMonitor.groupAdd(id, numeryKolejek, sMonitor);
        sleep(1);
    }
}
void *Consumer(void *idp){
    int id = * ((int*)idp);
    cout<<"Konsument "<<id<<endl;
    while(1){ 
        //cout<<"\tWyjmuje konsument "<<id<<endl;
        sMonitor[id].remove(gMonitor);
        sleep(10);
    }
}

int main(){
    pthread_t producers[PRODUCER_QUANTITY];
    pthread_t consumers[CONSUMER_QUANTITY];
    int tab[5];
    for(int i=0; i<max(CONSUMER_QUANTITY, PRODUCER_QUANTITY); ++i) tab[i]=i;
    for(int i=0; i<CONSUMER_QUANTITY; ++i) {
        pthread_create(&consumers[i], NULL, Consumer, &tab[i]);
    }
    for(int i=0; i<PRODUCER_QUANTITY; ++i){
        pthread_create(&producers[i], NULL, Producer, &tab[i]);
    }
    sleep(30);

    for(int i=0; i<CONSUMER_QUANTITY; ++i)
        pthread_cancel(consumers[i]);

    for(int i=0; i<PRODUCER_QUANTITY; ++i)
        pthread_cancel(producers[i]);
    return 0;
    //pthread_exit(NULL);
}
