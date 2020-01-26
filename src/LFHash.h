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
#define TOTDATA 1024*1024*10
#define TotalBuckets   20971520
#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

#ifndef LFHASH_H_
#define LFHASH_H_
class LFHash {
public:
    std::atomic<unsigned long> TimeCounter;
    LFHash();
    ~LFHash(){};
    unsigned int TimeStamp(){
    	return timeStamp.fetch_add(1,std::memory_order_relaxed);
    }
    unsigned int MurmurHash2A (const void * key, int len, unsigned int tabsize)
    {
        const unsigned int m = 0x5bd1e995;
        const int r = 24;
        unsigned int l = len;
        const unsigned char * data = (const unsigned char *)key;
        unsigned int h = seed;
        while(len >= 4)
        {
            unsigned int k = *(unsigned int*)data;

            mmix(h,k);

            data += 4;
            len -= 4;
        }
        unsigned int t = 0;
        switch(len)
        {
            case 3: t ^= data[2] << 16; break;
            case 2: t ^= data[1] << 8; break;
            case 1: t ^= data[0]; break;
        };
        mmix(h,t);
        mmix(h,l);
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;
        return h % tabsize;
    }
    unsigned int CreateBucketEntry(unsigned int key, unsigned long *NewEntry, short int threadID)
    {
        unsigned int InitialSlot =  MurmurHash2A((const void*)&key, 4, TotalBuckets);
        unsigned long Entry = 0;
        short int header=0;
        short int STATE = 3;
        STATE<<=14;
        header|=STATE;
        header|=threadID;
        Entry|=key;
        Entry|=(0L|header)<<48;
        *NewEntry = Entry;
        return InitialSlot;

    }
    unsigned int getKey(unsigned long Entry)
    {
        long Mask = 0xFFFFFFFFFFFFL;
        return (unsigned int)(Entry & Mask);
    }
    unsigned short int getHeader(unsigned long Entry)
    {

        unsigned short int header = Entry >> 48;
        return header;
    }
    unsigned int getBucket(unsigned int index)
    {
        unsigned int StoredKey = getKey(Buckets[index].load());
        unsigned int ind = MurmurHash2A((const void*)&StoredKey, 4, TotalBuckets);
        return ind;
    }
    unsigned short int getThread(unsigned long Entry)
    {
        unsigned short int Header = getHeader(Entry);
        unsigned short int mask=0X3FFF;
        return Header&mask;
    }
    void clearThread(unsigned long *Entry)
    {
        long MASK = 0XC000FFFFFFFFFFFFL;
        *Entry &= MASK;
        MASK = 0X0080000000000000L;
        *Entry |= MASK;
    }
    unsigned int checkIntegrity(int num)
    {
        int counter=0;
        for(int i=0;i<TotalBuckets ; i++)
            if(Buckets[i].load()!=0)
                counter++;
        std::cout <<  " elements " << counter <<"\n";
        return counter;
    }
    bool BucketInsert(unsigned int BaseBucket, unsigned int key){
    	unsigned int index;
    	short int Header;
    	unsigned int Entry = key;
    	unsigned long tag=0;
    	unsigned long _SlotEntry=0;
    	unsigned long cachedEntry;
    	for(int p=0;p<=H;p++){
    		index = BaseBucket + p;
    		_SlotEntry = Buckets[index].load();
    		Header = getHeader(_SlotEntry);
    		Header>>=14;
    		if(Header == 2)
    			cachedEntry = _SlotEntry;
    		if(_SlotEntry == 0)
    			if(Buckets[index].compare_exchange_strong(tag,Entry))
    				return true;
    	}
    	for(int p=0;p<=H;p++){
    		index = BaseBucket + p;
    		_SlotEntry = Buckets[index].load();
    		Header = getHeader(_SlotEntry);
    		Header>>=14;
    		if(Header == 2)
    			if(_SlotEntry != cachedEntry)
    				return false;
    			else
    				return Buckets[index].compare_exchange_strong(_SlotEntry,Entry);
    	}
    	printf("\n\n NEED A REHASHING OF THE HASH MAP \n\n ");
    	exit(0);
    }
    bool add(unsigned int key, int);
    bool Remove(unsigned int key);
    unsigned int Relocate(unsigned int RelocateEntry, long Entry);
    bool contains(unsigned int key);
    bool Eliminate(unsigned int key, int threadId, int, int );
protected:
private:
    std::atomic<unsigned long> *Buckets;
    int ADD_RANGE = 256;//512;
    unsigned int H = 32;//64;
    int KEYINFO = 28;
    unsigned int seed;
public:
    std::atomic<unsigned long> substract;
    std::atomic<unsigned long> timeStamp;
    struct timeval start, stop;

};

#endif /* LFHASH_H_ */