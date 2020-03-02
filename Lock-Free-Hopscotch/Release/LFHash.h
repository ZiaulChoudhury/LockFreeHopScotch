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
#include <bitset>         
#define TOTDATA 1024*1024*12
#define TotalBuckets   28311551
#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

#ifndef LFHASH_H_
#define LFHASH_H_
#define HOP 16

std::atomic<unsigned long> Buckets[TotalBuckets];
class LFHash {
public:
    std::atomic<unsigned long> TimeCounter;
    LFHash(){
    srand(time(NULL));
    substract.store(0);
    timeStamp.store(0);
    seed = rand() % LONG_MAX +1;
    for(int i=0;i<TotalBuckets + H; i++)
        Buckets[i].store(0);
    };
    ~LFHash(){};
    inline
    unsigned int TimeStamp(){
    	return timeStamp++;} //fetch_add(1,std::memory_order_relaxed);}

    inline
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
    inline
    unsigned int getKey(unsigned long Entry)
    {
        long Mask = 0xFFFFFFFFFFFFL;
        return (unsigned int)(Entry & Mask);
    }
    inline
    unsigned short int getHeader(unsigned long Entry)
    {

        unsigned short int header = Entry >> 48;
        return header;
    }
    inline 
    unsigned int getBucket(unsigned int index)
    {
        unsigned int StoredKey = getKey(Buckets[index].load());
        unsigned int ind = MurmurHash2A((const void*)&StoredKey, 4, TotalBuckets);
        return ind;
    }
    inline
    bool BucketInsert(unsigned int BaseBucket, unsigned int key){
    	unsigned int  index;
    	short int     Header;
    	unsigned int  Entry = key;
    	unsigned long tag=0;
    	unsigned long _SlotEntry=0;
    	unsigned long cachedEntry;
        unsigned long *a = ( unsigned long *) __builtin_assume_aligned(Buckets, 32) ;
    	std::bitset<HOP> cxx;
    	#pragma GCC ivdep
    	for(int p=0;p<=H;p++){
    		if((a[BaseBucket + p]>>62) == 2)
    			cachedEntry = a[BaseBucket + p];

    		if(a[BaseBucket + p] == 0)
			cxx[p] = __sync_bool_compare_and_swap(&a[BaseBucket+p], tag, Entry);
    	}
	
        if(cxx.any())
		return true;
	
    	#pragma GCC ivdep
    	for(int p=0;p<=H;p++){
    		if((a[BaseBucket + p]>>62) == 2)
    			if(_SlotEntry != cachedEntry)
    				return false;
    			else
    				return Buckets[index].compare_exchange_weak(_SlotEntry,Entry);
    	}
    	printf("\n\n NEED A REHASHING OF THE HASH MAP \n\n ");
    	exit(0);
    }
    bool  add(unsigned int key)
    {
    	unsigned long  Entry;
    	unsigned long tag=0;
    	unsigned int SlotIndex = CreateBucketEntry(key, &Entry, 0);
    	unsigned int BaseBucket = SlotIndex;
    	int trails=0;
    	if(contains(key))
        	return true;
    	do
    	{
        	while(trails++ < ADD_RANGE && Buckets[SlotIndex].load()!=0)
            		SlotIndex++;
    	}while(!Buckets[SlotIndex].compare_exchange_weak(tag,Entry) && trails < ADD_RANGE);
    	if(trails >= ADD_RANGE){std::cout<< " insertion failure "  << trails;exit(0);}
    	if((SlotIndex - BaseBucket)> H){
        	do{
            		unsigned int prevSlot = SlotIndex;
            		SlotIndex = this->Relocate(SlotIndex, Entry);
            		if(prevSlot == SlotIndex){exit(0);}
        	}while((SlotIndex - BaseBucket) > H);}

    	unsigned long Exp = Buckets[SlotIndex].load();
    	Buckets[SlotIndex].compare_exchange_weak(Exp,0);
    	while(!BucketInsert(BaseBucket,key)){}
    	return true;
    }

    unsigned int  Relocate(unsigned int Loc, long NewEntry)
    {
    unsigned int Replace = Loc - H;
    unsigned long Exp, Fin, Dest;
    unsigned int Ind=Replace;
    int index;
    std::bitset<HOP> cxx;
      do{
        unsigned int BaseLoc = getBucket(Ind);
        unsigned int HopDist = (Loc - BaseLoc);
        while(HopDist >= H && Replace < Loc){
            Replace++;
            BaseLoc=getBucket(Replace);
            HopDist = (Loc - BaseLoc);
        }
        if(HopDist > H || Replace >= Loc){ std::cout<< "\n\nRESIZE()\n\n"; exit(0); }
        Exp = Buckets[Replace].load();
        Exp = ((Exp << 2) >> 2);
        Fin = ((Exp << 2) >> 2)| (1L<<62);
        Ind = Replace+1;
        }while(!Buckets[Replace].compare_exchange_weak(Exp, Fin));

        Dest = Buckets[Loc].load();
        Dest = ((Dest << 2) >>2)| (3L<<62);
        Fin = Buckets[Replace].load();
        Fin = ((Fin<<2)>>2);
        Buckets[Loc].compare_exchange_weak(Dest, Fin);
        
	Exp = Buckets[Replace].load();
        Exp = ((Exp <<2 )>>2) | (1L<<62);
        Buckets[Replace].compare_exchange_weak(Exp, NewEntry);

     	return Replace;
     }

     inline bool  contains(unsigned int key)
     {
    	std::bitset<HOP> cxx;
    	unsigned int  InitialSlot =  MurmurHash2A((const void*)&key, 4, TotalBuckets);
    	unsigned long _SlotEntry=0;
    	unsigned long _KeyEntry1=key;
    	unsigned long _KeyEntry2=key;
    	_KeyEntry2 = ((_KeyEntry2 << 2) >> 2)| (1L<<62);
    	unsigned long *a = ( unsigned long *) __builtin_assume_aligned(Buckets, 16) ;

    	#pragma GCC ivdep
    	for(unsigned int p=0;p<=H ; p++)
    	{
        	_SlotEntry = a[InitialSlot+p];
        	if(_KeyEntry1 == _SlotEntry || _SlotEntry == _KeyEntry2)
                	cxx[p] = 1;
    	}
    	return  cxx.any();
    	}
     bool Remove(unsigned int key,int tid)
     {       
            unsigned int  InitialSlot =  MurmurHash2A((const void*)&key, 4, TotalBuckets);
            unsigned long _Slot = 0;
            unsigned int  FinalEntry = tid; //TimeStamp();
            FinalEntry =  ((FinalEntry<<2)>>2)|(2L << 62);
            bool rem1 = false;
            bool rem2 = false;
            unsigned long random;
            random = INT_MAX;
            random = ((random<<2)>>2)|(3L << 62);
    	    std::bitset<HOP> cxx;
    	    std::bitset<HOP> cxx2;
            unsigned long _KeyEntry1=key;
            unsigned long _KeyEntry2=key;
            _KeyEntry2 = ((_KeyEntry2 << 2) >> 2)| (1L<<62);
            unsigned long *a = ( unsigned long *) __builtin_assume_aligned(Buckets, 32) ;

    	    #pragma GCC ivdep
    	    for(unsigned int p=0;p<=H ; p++)
            {   
        	if(_KeyEntry1 == a[InitialSlot+p] || a[InitialSlot+p] == _KeyEntry2)
                	cxx[p] = 1;
	    }

            int i1 = cxx._Find_first();
            int i2 = cxx._Find_next(i1);
            _Slot  = key;
            rem1   = __sync_bool_compare_and_swap(&a[InitialSlot+i1], _Slot, FinalEntry); //a[InitialSlot+i1].compare_exchange_weak(_Slot,FinalEntry);
                
            _Slot |= (1L<<62);  
            rem2 =  __sync_bool_compare_and_swap(&a[InitialSlot+i2], _Slot, FinalEntry); //Buckets[InitialSlot+i2].compare_exchange_weak(_Slot,FinalEntry);
            
            /*for(int p=0;p<=H;p++){
                _Slot = Buckets[InitialSlot + p];
                _Slot =((_Slot<<2)>>2)|(3L << 62); 
                if((Buckets[InitialSlot+p]>>62) == 3)
                        Buckets[InitialSlot+p].compare_exchange_weak(_Slot, random);}*/
            return rem1 or rem2;
     }
protected:
private:
    int ADD_RANGE = 256;//512;
    unsigned int H = HOP; //64;
    unsigned int seed;
public:
    std::atomic<unsigned long> substract;
    std::atomic<unsigned long> timeStamp;
    //std::atomic<int> timeStamp;
    struct timeval start, stop;

};

#endif /* LFHASH_H_ */
