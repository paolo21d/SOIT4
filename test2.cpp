#include <iostream>
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "MyMonitor.h"

using namespace std;
//#define BUFSIZE 10
#define ILOSC_KONSUMENTOW 5
#define ILOSC_PRODUCENTOW 2
#define ILOSC_KOLEJEK 5

void losujNumeryKolejek(int *tab) {
    for(int i=0; i<ILOSC_KOLEJEK; ++i)
        tab[i]=i;
    int nrZamiana, tmp;
    for(int i=0; i<ILOSC_KOLEJEK; ++i){
        nrZamiana = rand()%ILOSC_KOLEJEK;
        tmp=tab[i];
        tab[i]=tab[nrZamiana];
        tab[nrZamiana]=tmp;
    }
}
SingleMonitor sMonitor[ILOSC_KOLEJEK];
GroupMonitor gMonitor;

///////////////////////////////////////////////////////////
void *Producer(void *idp){
    srand(time(NULL));
    int id = * ((int*)idp);
    cout<<"Producent "<<id<<endl;

    int numeryKolejek[ILOSC_KOLEJEK];
    while(1){
        losujNumeryKolejek(numeryKolejek);
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
    pthread_t producers[ILOSC_PRODUCENTOW];
    pthread_t consumers[ILOSC_KONSUMENTOW];
    int tab[5];
    for(int i=0; i<max(ILOSC_KONSUMENTOW, ILOSC_PRODUCENTOW); ++i) tab[i]=i;
    for(int i=0; i<ILOSC_KONSUMENTOW; ++i) {
        pthread_create(&consumers[i], NULL, Consumer, &tab[i]);
    }
    for(int i=0; i<ILOSC_PRODUCENTOW; ++i){
        pthread_create(&producers[i], NULL, Producer, &tab[i]);
    }
    sleep(30);

    for(int i=0; i<ILOSC_KONSUMENTOW; ++i)
        pthread_cancel(consumers[i]);

    for(int i=0; i<ILOSC_PRODUCENTOW; ++i)
        pthread_cancel(producers[i]);
    return 0;
    //pthread_exit(NULL);
}
