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
class SingleMonitor;
class GroupMonitor;

class SingleMonitor: public Monitor{
    Queue buffer;
    Condition full;
    Condition empty;
public:
    void add(const int &a);
    int remove(GroupMonitor &gm);
    int getSize();
};
class GroupMonitor: public Monitor{
    Condition groupEmpty;
    int iloscPustych;
    public:
    GroupMonitor(): iloscPustych(ILOSC_KOLEJEK*BUFSIZE){};
    void groupAdd(const int &a, const int *tab, SingleMonitor *sm);
    void zwieksz();
    void zmniejsz();
};
//////////////definicje
void SingleMonitor::add(const int &a) {
        enter();
        if(buffer.size() == BUFSIZE)
            wait(empty);
        buffer.putToBuff(a);
        //gm.zmniejsz();
        if(buffer.size() == 1)
            signal(full);
        cout<<"po signal"<<endl;
        leave();
    }
int SingleMonitor::remove(GroupMonitor &gm) {
        enter();
        if(buffer.size() == 0)
            wait(full);
        int ret = buffer.getFromBuf();
        gm.zwieksz();
        if(buffer.size() == BUFSIZE-1)
            signal(empty);
        leave();
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
        if(iloscPustych==0) //pelne wszystkie bufory
            wait(groupEmpty);
        //int i=0;
        //cout<<"szukam pustej"<<endl;
        for(int i=0; i<ILOSC_KOLEJEK; ++i){
            int id = tab[i];
            if(sm[id].getSize()<BUFSIZE){
                cout<<"Probuje wstawiac do: "<<id<<endl;
                sm[id].add(a);
                cout<<"PO WSADZENIU!!!!!!"<<endl;
                break;
            }
        }
        iloscPustych--;
        leave();
    }
void GroupMonitor::zwieksz(){ //to jest wołane przy wyjmowaniu elementów z jakiejś kolejki przy operacji remove
        enter();
        iloscPustych++;
        if(iloscPustych == 1) //czyli mozna juz wstawic cos, bo zostal wyjaty element z jakiejs kolejki
            signal(groupEmpty);
        leave();
    }

void GroupMonitor::zmniejsz(){
        enter();
        iloscPustych--;
        leave();
    }