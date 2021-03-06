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
#include <unordered_map>
#include <mutex>
#define TOTDATA 1024*1024*12
#ifndef WORKLOAD_H_
#define WORKLOAD_H_

class workload {
public:
    workload(){
        for(int i=0;i<TOTDATA;i++) {
            uint32_t key;
            do {
                key = (rand() % (1024 * 1024 * 100)) + 1;
            } while (dataset.find(key) != dataset.end());
            dataset[key] = 12;
            store[i] = key;
        }
    }

    void workSplit(int _NoContains, int _NoInserts, int _NoRemove, int threads, double successPercentage)
    {
        tcx = _NoContains/threads;
        tix = _NoInserts/threads;
        trx = _NoRemove/threads;
	for(int i = 0; i<_NoContains; i++){
            lookUps[i] = store[i+_NoRemove];}

	int i=0;
        for(i = 0; i<_NoRemove; i++){
            removes[i] = store[i+offset];}

	offset = i;	
        for(i=0;i<_NoInserts/2;i++) {
                    uint32_t key;
                    do {
                        key = (rand() % (1024 * 1024 * 100)) + 1;
                    } while (dataset.find(key) != dataset.end() && key != 0);
                    dataset[key] = 12;
                    adds[i] = key;
                }
	
        for(int j=0;i<_NoInserts;j++,i++) {
		adds[i] = lookUps[j];
	}
	

    }

    uint32_t get(int index){
        return store[index];
    }

    std::unordered_map<std::uint32_t , std::uint8_t > dataset;
    uint32_t *lookUps = new uint32_t[TOTDATA];
    uint32_t *store   = new uint32_t[TOTDATA];
    uint32_t *adds    = new uint32_t[TOTDATA];
    uint32_t *removes = new uint32_t[TOTDATA];
    int tcx = 0;
    int offset = 0;
    int tix = 0;
    int trx = 0;
};

#endif /* WORKLOAD_H_ */
