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
#include "workload.h"

int THREADS;
int DATAPERTHREAD;
struct timeval start, stop;
using namespace std;
std::atomic<int> datacounter (0);
unsigned int *dataset;
unsigned int *randArray;
int contains, inserts, deletes;

void insertKey(LFHash *h, workload *wx, int tid)
{
    unsigned int key = 0;
    for(int i=0;i<DATAPERTHREAD;i++)
    {
        key=wx->get(tid*DATAPERTHREAD + i);
        if(!h->add(key, tid))
        	printf("\n Error Key Insertion ");

    }
}
void lookUp(LFHash *h, workload *wx, int tid)
{
	unsigned int key = 0;
    for(int i=0;i<DATAPERTHREAD;i++)
    {
    	key=wx->get(tid*DATAPERTHREAD + i);
        if(!h->contains(key))
        	printf(" \n Error in Key Retrieval %d ", key);
    }
}
void clearAll(LFHash *h, workload *wx, int tid)
{
	unsigned int key = 0;
    for(int i=0;i<DATAPERTHREAD;i++)
    {
    	key=wx->get(tid*DATAPERTHREAD + i);
        if(!h->Remove(key))
        	printf(" \n Error in Key Removal %d ", key);
    }
}
void worker(LFHash *h, workload *wx, int tid)
{
    int key=0;
    for(int i=0;i<wx->tcx;i++)
    	if(!h->contains(wx->lookUps[tid*wx->tcx + i]))
    		printf(" \n Error in Key Retrieval %d ", wx->lookUps[tid*wx->tcx + i]);

    for(int i=0;i<wx->trx;i++)
    	if(!h->Remove(wx->removes[tid*wx->trx + i]))
    	    printf(" \n Error in Key Removal %d ", wx->removes[tid*wx->trx + i]);

    for(int i=0;i<wx->tix;i++)
        	if(!h->add(wx->adds[tid*wx->tix + i],tid))
        	    printf(" \n Error in Key Insertion %d ", wx->adds[tid*wx->tix + i]);

}

int main (int argc, char* argv[])
{

    THREADS=atoi(argv[1]);
    DATAPERTHREAD=TOTDATA/THREADS;
    LFHash h;
    workload wx;
    std::vector<std::thread> Threads;
    auto starty = chrono::steady_clock::now();
    for(int i=0;i<THREADS;i++)
        Threads.push_back(std::thread(insertKey,&h,&wx,i));
    gettimeofday(&start, NULL);
    for(int i=0;i<THREADS;i++)
        Threads[i].join();
    auto endy = chrono::steady_clock::now();
    double el = chrono::duration_cast<chrono::milliseconds>(endy - starty).count();
    cout << "[100% Insertion] " << " Time = "
         << el <<" milliseconds " <<" Throughput = " << ((TOTDATA/el)*1000)/1000000 <<" MQPS \n";


    std::vector<std::thread> ContainThreads;
    starty = chrono::steady_clock::now();
    for(int i=0;i<THREADS;i++)
        ContainThreads.push_back(std::thread(lookUp, &h, &wx, i));
    for(int i=0;i<THREADS;i++)
        ContainThreads[i].join();
    endy = chrono::steady_clock::now();
    el = chrono::duration_cast<chrono::milliseconds>(endy - starty).count();
    cout << "[100% Look  Ups] " << " Time = "
             << el <<" milliseconds " <<" Throughput = " << ((TOTDATA/el)*1000)/1000000 <<" MQPS \n";

    wx.workSplit(1000000,50000,50000,THREADS);
    std::vector<std::thread> MixThreads;
    starty = chrono::steady_clock::now();
    for(int i=0;i<THREADS;i++)
            MixThreads.push_back(std::thread(worker, &h, &wx, i));
    for(int i=0;i<THREADS;i++)
            MixThreads[i].join();
    endy = chrono::steady_clock::now();
    el = chrono::duration_cast<chrono::milliseconds>(endy - starty).count();
    cout << "[Mix Workload  ] " << " Time = "
                 << el <<" milliseconds " <<" Throughput = " << (((1000000+100000)/el)*1000)/1000000 <<" MQPS \n";

    return 0;
}
