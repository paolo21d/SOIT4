#include <iostream>
#include <pthread.h>


#define BUFSIZE 10
#define ILOSC_KONSUMENTOW 5
#define ILOSC_PRODUCENTOW 3
#define ILOSC_KOLEJEK 5
struct Queue {
    char buf[BUFSIZE];
    int head, tail, length;
};
void Producer(int id){
}
void Consumer(int id){
}
int main(){

}
