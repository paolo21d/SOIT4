#include <iostream>
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include "Monitor.h"

using namespace std;
#define BUFSIZE 10
#define ILOSC_KONSUMENTOW 5
#define ILOSC_PRODUCENTOW 3
#define QUEUE_QUANTITY 5
class Queue {
    int buf[BUFSIZE];
    int head, tail, length;
    public:
    Queue(){
        head=0;
        tail=0;
        length=0;
        for (unsigned i=0; i<BUFSIZE; ++i)
            buf[i]=0;
    }
    void putToBuff(int val){
        buf[tail] = val;
        tail = (tail+1)%BUFSIZE;
        length++;
        printf("Wsadzam %d, size: %d\n", val, length);
        //cout<<"Wsadzam "<<val<<", size: "<<length<<endl;
    }
    int getFromBuf(){
        int ret = buf[head];
        head = (head+1)%BUFSIZE;
        length--;
        printf("\tWyjmuje %d\n", ret);
        //cout<<"Wyjmuje "<<ret<<endl;
        return ret;
    }
    int size(){
        return length;
    }
};
class SingleMonitor;
class GroupMonitor;

class SingleMonitor: public Monitor{
    Queue buffer;
    Condition full;
    Condition empty;
public:
    void add(int a, GroupMonitor &gm) {
        enter();
        if(buffer.size() == BUFSIZE)
            wait(empty);
        buffer.putToBuff(a);
        //gm.decrease();
        if(buffer.size() == 1)
            signal(full);
        leave();
    }
    int remove(GroupMonitor &gm) {
        enter();
        if(buffer.size() == 0)
            wait(full);
        int ret = buffer.getFromBuf();
        gm.increase();
        if(buffer.size() == BUFSIZE-1)
            signal(empty);
        leave();
        return ret;
    }
    int getSize(){
        enter();
        int size = buffer.size();
        leave();
        return size;
    }
    /*void checkAndAdd(int a, int &tab){
    }*/
};
SingleMonitor sMonitor[QUEUE_QUANTITY];

class GroupMonitor: public Monitor{
    Condition groupEmpty;
    int emptyQuantity;
    public:
    GroupMonitor(): emptyQuantity(QUEUE_QUANTITY*BUFSIZE){};
    void groupAdd(int a, int *tab, SingleMonitor *sm){
        enter();
        if(emptyQuantity==0) //pelne wszystkie bufory
            wait(groupEmpty);
        //int i=0;
        for(int i=0; i<QUEUE_QUANTITY; ++i){
            int id = tab[i];
            if(sm[id].getSize()>0){
                sm[id].add(a, *this);
                break;
            }
        }
        emptyQuantity--;
        leave();
    }
    void increase(){ //to jest wołane przy wyjmowaniu elementów z jakiejś kolejki przy operacji remove
        enter();
        emptyQuantity++;
        if(emptyQuantity == 1) //czyli mozna juz wstawic cos, bo zostal wyjaty element z jakiejs kolejki
            signal(groupEmpty);
        leave();
    }
    void decrease(){
        enter();
        emptyQuantity--;
        leave();
    }
};
GroupMonitor gMonitor;
void losujNumeryKolejek(int *tab) {
    for(int i=0; i<QUEUE_QUANTITY; ++i)
        tab[i]=i;
    int nrZamiana, tmp;
    for(int i=0; i<QUEUE_QUANTITY; ++i){
        nrZamiana = rand()%QUEUE_QUANTITY;
        tmp=tab[i];
        tab[i]=tab[nrZamiana];
        tab[nrZamiana]=tmp;
    }
}
///////////////////////////////////////////////////////////


void *Producer(void *idp){
    int id = * ((int*)idp);
    cout<<"Producent "<<id<<endl;

    int numeryKolejek[QUEUE_QUANTITY];
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
