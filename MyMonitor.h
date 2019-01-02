#include <iostream>
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include "MonitorUnix.h"
using namespace std;
#define BUFSIZE 10
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
        printfQueue();
        //cout<<"Wsadzam "<<val<<", size: "<<length<<endl;
    }
    int getFromBuf(){
        int ret = buf[head];
        head = (head+1)%BUFSIZE;
        length--;
        printf("\tWyjmuje %d\n", ret);
        printfQueue();
        //cout<<"Wyjmuje "<<ret<<endl;
        return ret;
    }
    int size(){
        return length;
    }
    void printfQueue(/*int nr*/){
    //printf("Queue %d size %d: ", nr, length);
    printf("Queue size %d :", length);
    int index = head;
    for(int i=0; i<length; ++i, ++index)
        printf("%d ", buf[index%BUFSIZE]);

    printf("\n");
}
};
class SingleMonitor;
class GroupMonitor;

class SingleMonitor: public Monitor{
    Queue buffer;
    Condition full;
    Condition empty;
public:
    void add(const int &a);
    int remove(GroupMonitor &gm);
    void signalFull();
    int getSize();
};
class GroupMonitor: public Monitor{
    Condition groupEmpty;
    int emptyQuantity;
    public:
    GroupMonitor(): emptyQuantity(QUEUE_QUANTITY*BUFSIZE){};
    void groupAdd(const int &a, const int *tab, SingleMonitor *sm);
    void increase();
    void decrease();
};
//////////////definicje
void SingleMonitor::add(const int &a) {
        enter();
        if(buffer.size() == BUFSIZE)
            wait(empty);
        buffer.putToBuff(a);
        //gm.decrease();
        if(buffer.size() == 1)
            signal(full);
        leave();
    }
int SingleMonitor::remove(GroupMonitor &gm) {
        enter();
        if(buffer.size() == 0)
            wait(full);
        int ret = buffer.getFromBuf();
        if(buffer.size() == BUFSIZE-1)
            signal(empty);
        leave();
        gm.increase();
        return ret;
    }
int SingleMonitor::getSize(){
        enter();
        int size = buffer.size();
        leave();
        return size;
    }

void GroupMonitor::groupAdd(const int &a, const int *tab, SingleMonitor *sm){
        enter();
        if(emptyQuantity==0) //pelne wszystkie bufory
            wait(groupEmpty); //czekam aż będzie 1 kolejka nie pełna
        //cout<<"szukam pustej"<<endl;
        for(int i=0; i<QUEUE_QUANTITY; ++i){
            int id = tab[i];
            if(sm[id].getSize()<BUFSIZE){
                cout<<"Wstawiam do: "<<id<<endl;
                sm[id].add(a);
                break;
            }
        }
        emptyQuantity--;
        leave();
    }
void GroupMonitor::increase(){ //to jest wołane przy wyjmowaniu elementów z jakiejś kolejki przy operacji remove
        enter();
        emptyQuantity++;
        if(emptyQuantity == 1) //czyli mozna juz wstawic cos, bo zostal wyjaty element z jakiejs kolejki
            signal(groupEmpty);
        leave();
    }

void GroupMonitor::decrease(){
        enter();
        emptyQuantity--;
        leave();
    }