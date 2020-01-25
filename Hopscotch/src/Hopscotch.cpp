#include <iostream>
#include <atomic>
#include <thread>
#include <climits>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <chrono>
#include <unistd.h>
#include "LFHash.h"

int THREADS;
int DATAPERTHREAD;
struct timeval start, stop;
using namespace std;
std::atomic<int> datacounter (0);
unsigned int *dataset;
unsigned int *randArray;
int contains, inserts, deletes;

void insertKey(LFHash *h, int tid)
{
    unsigned int key = 0;
    for(int i=0;i<DATAPERTHREAD;i++)
    {
        key=tid*DATAPERTHREAD + i  +1;
        if(h->add(key, tid))
            dataset[datacounter.fetch_add(1)]=key;
    }
}
void checkKeys(LFHash *h, int index)
{

    for(int i=0;i<DATAPERTHREAD;i++)
    {
        if(!h->contains(dataset[index*DATAPERTHREAD+i]))
        {
            std::cout <<" FAILURE " << dataset[index*DATAPERTHREAD+i]<<"\n ";
        }
    }
}
void worker(LFHash *h, int tid)
{
    int key=0;
    for(int i=0;i<contains;i++)
        if(!h->contains(dataset[randArray[tid * contains + i]]))
            printf(" \n ERRRO PROPAGATED ");

    for(int x=0;x<inserts;x++)
    {
        key = randArray[tid * inserts + x];
        h->add(key,tid);
    }
    /*for(int x=0;x<deletes;x++)
    {
        key = -1
        h->Remove(key);
    }*/
}

int main (int argc, char* argv[])
{

    THREADS=atoi(argv[1]);
    DATAPERTHREAD=TOTDATA/THREADS;
    dataset = new unsigned int [TOTDATA];
    randArray = new unsigned int[TOTDATA];

    for(int i=0;i<TOTDATA ;i++)
    {
        dataset[i]=0;
        randArray[i]=rand()% TOTDATA + 1;
    }
    LFHash h;
    std::vector<std::thread> Threads;
    std::vector<std::thread> ContainThreads;
    auto starty = chrono::steady_clock::now();
    for(int i=0;i<THREADS;i++)
    {
        Threads.push_back(std::thread(insertKey, &h, i));
    }
    gettimeofday(&start, NULL);
    for(int i=0;i<THREADS;i++)
    {
        Threads[i].join();
    }
    auto endy = chrono::steady_clock::now();
    cout << "Build Elapsed time in milliseconds : "
         << chrono::duration_cast<chrono::milliseconds>(endy - starty).count()
         << " ms" << endl;

    starty = chrono::steady_clock::now();
    for(int i=0;i<THREADS;i++) {
        ContainThreads.push_back(std::thread(checkKeys, &h, i));
    }
    for(int i=0;i<THREADS;i++)
    {
        ContainThreads[i].join();
    }



    endy = chrono::steady_clock::now();
    double el = chrono::duration_cast<chrono::milliseconds>(endy - starty).count();
    cout << "Retreival Elapsed time in milliseconds : "
         << el
         << " ms" << endl;
    cout<<" Throughput " << ((TOTDATA/el)*1000)/1000000 <<"\n";


    cout<<"\n [90-5-5] (1) [80-10-10] (2) [60-30-30] (3) [30-30-30] (4) \n";
    int choice;
    cin>>choice;
    switch(choice)
    {
        case 1:
        {
            contains = (int)(TOTDATA*0.9)/THREADS;
            inserts =  (int)(TOTDATA*0.05)/THREADS;
            deletes =  (int)(TOTDATA*0.05)/THREADS;
            break;
        }
        case 2:
        {
            contains = (int)(TOTDATA*0.8)/THREADS;
            inserts =  (int)(TOTDATA*0.1)/THREADS;
            deletes =  (int)(TOTDATA*0.1)/THREADS;
            break;
        }
        case 3:
        {
            contains = (int)(TOTDATA*0.6)/THREADS;
            inserts =  (int)(TOTDATA*0.2)/THREADS;
            deletes =  (int)(TOTDATA*0.2)/THREADS;
            break;
        }
        case 4:
        {
            contains = (int)(TOTDATA*0.4)/THREADS;
            inserts =  (int)(TOTDATA*0.3)/THREADS;
            deletes =  (int)(TOTDATA*0.3)/THREADS;
            cout<< contains <<"  " << inserts <<"  "<<deletes;
            break;
        }
    }
    std::vector<std::thread> threads;
    starty = chrono::steady_clock::now();
    for(int i=0;i<THREADS;i++) {
        threads.push_back(std::thread(worker, &h, i));
    }
    for(int i=0;i<THREADS;i++)
        threads[i].join();
    endy = chrono::steady_clock::now();
    el = chrono::duration_cast<chrono::milliseconds>(endy - starty).count();
    cout << "Elapsed time in milliseconds : "
         << el
         << " ms" << endl;
    return 0;
}
