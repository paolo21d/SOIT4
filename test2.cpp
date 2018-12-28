#include <iostream>
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include "MyMonitor.h"

using namespace std;
#define BUFSIZE 10
#define ILOSC_KONSUMENTOW 5
#define ILOSC_PRODUCENTOW 3
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
    int id = * ((int*)idp);
    cout<<"Producent "<<id<<endl;

    int numeryKolejek[ILOSC_KOLEJEK];
    //printf("Producent %d\n", id);
    while(1){
        losujNumeryKolejek(numeryKolejek);

        //cout<<"Wsadzam "<<id<<endl;
        //sMonitor[id].add(id, gMonitor);
        gMonitor.groupAdd(id, numeryKolejek, sMonitor);
        sleep(1);
    }
}
void *Consumer(void *idp){
    int id = * ((int*)idp);
    cout<<"Konsument "<<id<<endl;
    //printf("Konsument %d\n", id);
    while(1){
        //cout<<"wyjmuje "<<id<<endl;
        sMonitor[id].remove(gMonitor);
        sleep(10);
    }
}

int main(){
    pthread_t producers[5];
    pthread_t consumers[5];
    int tab[5];
    for(int i=0; i<5; ++i) tab[i]=i;
    for(int i=0; i<1; ++i) {
        pthread_create(&producers[i], NULL, Producer, &tab[i]);
        pthread_create(&consumers[i], NULL, Consumer, &tab[i]);
    }
    sleep(30);
    pthread_exit(NULL);
}
