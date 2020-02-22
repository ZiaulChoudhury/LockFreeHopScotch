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
#include "papi.h"

int THREADS;
int DATAPERTHREAD;
struct timeval start, stop;
using namespace std;
std::atomic<int> datacounter (0);
unsigned int *dataset;
unsigned int *randArray;
int contains, inserts, deletes;

int QUERIES_TOTAL = 2000000;

int CONTAINS[4] = {2000000, 4000000,4000000};
int INSERTS[4]  = {80000,  150000,  1000000};
int REMOVES[4]  = {80000,  550000,  1000000};
#define SUCCESS_RATE 0.1


void insertKey(LFHash *h, workload *wx, int tid)
{
    unsigned int key = 0;
    for(int i=0;i<DATAPERTHREAD;i++)
    {
        key=wx->get(tid*DATAPERTHREAD + i);
        if(!h->add(key))
            printf("\n Error Key Insertion ");

    }
}
void lookUp(LFHash *h, workload *wx, int tid)
{
    unsigned int key = 0;
    for(int i=0;i<DATAPERTHREAD;i++)
    {
        key=wx->get(tid*DATAPERTHREAD + i);
        h->contains(key);
    }
}
void clearAll(LFHash *h, workload *wx, int tid)
{
    unsigned int key = 0;
    for(int i=0;i<DATAPERTHREAD;i++)
    {
        key=wx->get(tid*DATAPERTHREAD + i);
        h->Remove(key,tid);
    }
}
void worker(LFHash *h, workload *wx, int tid)
{
    for(int i=0;i<wx->tcx;i++)
        //if(!h->contains(wx->lookUps[tid*wx->tcx + i]))
         //printf(" \n Error in Key Retrieval %d ", wx->lookUps[tid*wx->tcx + i]);
        h->contains(wx->lookUps[tid*wx->tcx + i]);

    for(int i=0;i<wx->trx;i++)
        //if(!h->Remove(wx->removes[tid*wx->trx + i],tid))
        //	printf(" \n Error in Key Removal %d ", wx->removes[tid*wx->trx + i]);
        h->Remove(wx->removes[tid*wx->trx + i],tid);

    for(int i=0;i<wx->tix;i++)
       //if(!h->add(wx->adds[tid*wx->tix + i]))
        //	printf(" \n Error in Key Insertion %d ", wx->adds[tid*wx->tix + i]);
        h->add(wx->adds[tid*wx->tix + i]);

}

int main (int argc, char* argv[])
{
    THREADS=atoi(argv[1]);
    DATAPERTHREAD=TOTDATA/THREADS;
    LFHash h;
    workload wx;
    std::vector<std::thread> Threads;
    auto starty = chrono::steady_clock::now();
    for(int i=0;i<THREADS;i++) {
        Threads.push_back(std::thread(insertKey, &h, &wx, i));
    }

    for(int i=0;i<THREADS;i++) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(i%6, &cpuset);
        int rc = pthread_setaffinity_np(Threads[i].native_handle(),
                                        sizeof(cpu_set_t), &cpuset);
        if (rc != 0) {
            std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
        }
    }
    for(int i=0;i<THREADS;i++)
        Threads[i].join();
    auto endy = chrono::steady_clock::now();
    double el = chrono::duration_cast<chrono::milliseconds>(endy - starty).count();
    cout<<"Hash Table Initilized \n";

    //cout << "[ " << 0 << "% contains "<<100 << "% inserts "<<0  << "% removes ] "<<" Time = "
    //   << el <<" milliseconds " <<" Throughput = " << ((TOTDATA/el)*1000)/1000000 <<" MQPS \n";

    el = 0;
    for(int iter = 0;iter < 5; iter++){
        std::vector<std::thread> ContainThreads;
        starty = chrono::steady_clock::now();
        for(int i=0;i<THREADS;i++)
            ContainThreads.push_back(std::thread(lookUp, &h, &wx, i));
        for(int i=0;i<THREADS;i++) {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i%6, &cpuset);
            int rc = pthread_setaffinity_np(ContainThreads[i].native_handle(),
                                            sizeof(cpu_set_t), &cpuset);
            if (rc != 0) {
                std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
            }
        }
        for(int i=0;i<THREADS;i++)
            ContainThreads[i].join();
        endy = chrono::steady_clock::now();
        el += chrono::duration_cast<chrono::milliseconds>(endy - starty).count();
    }
    el = el/5;
    //cout << "[ " << 100 << "% contains "<<0 << "% inserts "<<0  << "% removes ] "<<" Time = "
      //   << el <<" milliseconds " <<" Throughput = " << ((TOTDATA/el)*1000)/1000000 <<" MQPS \n";
    
    int QUERIES;
    double updateRate[10] = {10};
    for(int ixx=0;ixx<1;ixx++) {
        
	double rate = updateRate[ixx]/100;
        int updates = QUERIES_TOTAL*rate;
	int contains = QUERIES_TOTAL;
        wx.workSplit(QUERIES_TOTAL, updates, updates, THREADS, SUCCESS_RATE);
        QUERIES = contains + 2*updates; 
        el = 0;
        for (int iter = 0; iter < 1; iter++) {
            std::vector<std::thread> MixThreads;
            PAPI_hl_region_begin("computation");
            starty = chrono::steady_clock::now();
            for (int i = 0; i < THREADS; i++)
                MixThreads.push_back(std::thread(worker, &h, &wx, i));
            for (int i = 0; i < THREADS; i++) {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET(i % 6, &cpuset);
                int rc = pthread_setaffinity_np(MixThreads[i].native_handle(),
                                                sizeof(cpu_set_t), &cpuset);
                if (rc != 0) {
                    std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
                }
            }
            for (int i = 0; i < THREADS; i++)
                MixThreads[i].join();
            endy = chrono::steady_clock::now();
            el += chrono::duration_cast<chrono::milliseconds>(endy - starty).count();
	    PAPI_hl_region_end("computation");
        }
        el = el;
        cout << "[ " << (double) (contains) / QUERIES * 100 << "% contains " << (double) (updates) / QUERIES * 100
             << "% inserts " << (double) updates / QUERIES * 100 << "% removes ] " << " Time = "
             << el << " milliseconds " << " Throughput = " << (((QUERIES) / el) * 1000) / 1000000 << " MQPS \n";
    }
    cout<<"LOAD FACTOR = " <<(double(TOTDATA)/(TotalBuckets))<<"\n";
    cout<<"TOTAL OPERATIONS = " << (QUERIES + TOTDATA + QUERIES_TOTAL)<<"\n";
    return 0;
}


