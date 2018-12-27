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
#define ILOSC_KOLEJEK 5
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
class GroupMonitor: public Monitor{
    
};
class SingleMonitor: public Monitor{
    Queue buffer;
    Condition full;
    Condition empty;
public:
    void add(int a) {
        enter();
        if(buffer.size() == BUFSIZE)
            wait(empty);
        buffer.putToBuff(a);
        if(buffer.size() == 1)
            signal(full);
        leave();
    }
    int remove() {
        enter();
        if(buffer.size() == 0)
            wait(full);
        int ret = buffer.getFromBuf();
        if(buffer.size() == BUFSIZE-1)
            signal(empty);
        leave();
        return ret;
    }
};
struct params{
    int id;
};
SingleMonitor sMonitor[5];
//GroupMonitor groupMonitor;

void *Producer(void *idp){
    //params *p = (params *) par;
    //int id = p.id;
    int id = * ((int*)idp);
    cout<<"Producent "<<id<<endl;
    //printf("Producent %d\n", id);
    while(1){
        //cout<<"Wsadzam "<<id<<endl;
        sMonitor[id].add(id);
        sleep(1);
    }
}
void *Consumer(void *idp){
    //params *p = (params *) par;
    //int id = p.id;
    int id = * ((int*)idp);
    cout<<"Konsument "<<id<<endl;
    //printf("Konsument %d\n", id);
    while(1){
        //cout<<"wyjmuje "<<id<<endl;
        sMonitor[id].remove();
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
