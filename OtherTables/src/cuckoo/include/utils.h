#ifndef _UTILS_H_INCLUDED_
#define _UTILS_H_INCLUDED_
//some utility functions
//#define USE_MUTEX_LOCKS
//#define ADD_PADDING
/* #define OPTERON */
/* #define OPTERON_OPTIMIZE */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include <inttypes.h>
#include <sys/time.h>
#include <unistd.h>
#ifdef __sparc__
#  include <sys/types.h>
#  include <sys/processor.h>
#  include <sys/procset.h>
#elif defined(__tile__)
#  include <arch/atomic.h>
#  include <arch/cycle.h>
#  include <tmc/cpus.h>
#  include <tmc/task.h>
#  include <tmc/spin.h>
#  include <sched.h>
#else
#  if defined(PLATFORM_MCORE)
#    include <numa.h>
#  endif
#  if defined(__SSE__)
#    include <xmmintrin.h>
#  else
#    define _mm_pause() asm volatile ("nop")
#  endif
#  if defined(__SSE2__)
#    include <emmintrin.h>
#  endif
#endif
#include <pthread.h>
#include "getticks.h"
#include "random.h"
#include "measurements.h"
#include "ssalloc.h"
#include "atomic_ops_if.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DO_ALIGN
  /* #define DO_PAD */


#if !defined(false)
#  define false 0
#endif

#if !defined(true)
#  define true 1
#endif

#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)


#if !defined(UNUSED)
#  define UNUSED __attribute__ ((unused))
#endif

#if defined(DO_ALIGN)
#  define ALIGNED(N) __attribute__ ((aligned (N)))
#else
#  define ALIGNED(N)
#endif

#if !defined(COMPILER_BARRIER)
#  define COMPILER_BARRIER() asm volatile ("" ::: "memory")
#endif

#if !defined(COMPILER_NO_REORDER)
#  define COMPILER_NO_REORDER(exec)		\
  COMPILER_BARRIER();				\
  exec;						\
  COMPILER_BARRIER()
#endif

  static inline int
  is_power_of_two (unsigned int x) 
  {
    return ((x != 0) && !(x & (x - 1)));
  }


#ifdef T44
#  define NUMBER_OF_SOCKETS 4
#  define CORES_PER_SOCKET 64
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 2
  //mapping from threads to cores on the niagara
#  define ALTERNATE_CORES
#  ifdef ALTERNATE_CORES
  static uint8_t UNUSED the_cores[] = {
    0,   8,  16,  24,  32,  40,  48,  56, 
    1,   9,  17,  25,  33,  41,  49,  57, 
    2,  10,  18,  26,  34,  42,  50,  58, 
    3,  11,  19,  27,  35,  43,  51,  59, 
    4,  12,  20,  28,  36,  44,  52,  60, 
    5,  13,  21,  29,  37,  45,  53,  61, 
    6,  14,  22,  30,  38,  46,  54,  62, 
    7,  15,  23,  31,  39,  47,  55,  63, 
    64,  72,  80,  88,  96, 104, 112, 120, 
    65,  73,  81,  89,  97, 105, 113, 121, 
    66,  74,  82,  90,  98, 106, 114, 122, 
    67,  75,  83,  91,  99, 107, 115, 123, 
    68,  76,  84,  92, 100, 108, 116, 124, 
    69,  77,  85,  93, 101, 109, 117, 125, 
    70,  78,  86,  94, 102, 110, 118, 126, 
    71,  79,  87,  95, 103, 111, 119, 127, 
    128, 136, 144, 152, 160, 168, 176, 184, 
    129, 137, 145, 153, 161, 169, 177, 185, 
    130, 138, 146, 154, 162, 170, 178, 186, 
    131, 139, 147, 155, 163, 171, 179, 187, 
    132, 140, 148, 156, 164, 172, 180, 188, 
    133, 141, 149, 157, 165, 173, 181, 189, 
    134, 142, 150, 158, 166, 174, 182, 190, 
    135, 143, 151, 159, 167, 175, 183, 191, 
    192, 200, 208, 216, 224, 232, 240, 248, 
    193, 201, 209, 217, 225, 233, 241, 249, 
    194, 202, 210, 218, 226, 234, 242, 250, 
    195, 203, 211, 219, 227, 235, 243, 251, 
    196, 204, 212, 220, 228, 236, 244, 252, 
    197, 205, 213, 221, 229, 237, 245, 253, 
    198, 206, 214, 222, 230, 238, 246, 254, 
    199, 207, 215, 223, 231, 239, 247, 255, 
  };

  static uint8_t UNUSED the_sockets[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 
  };

#  else	 /* !ALTERNATE_CORES */
  static UNUSED the_cores[] = {
    0,   1,   2,   3,   4,   5,   6,   7, 
    8,   9,  10,  11,  12,  13,  14,  15, 
    16,  17,  18,  19,  20,  21,  22,  23, 
    24,  25,  26,  27,  28,  29,  30,  31, 
    32,  33,  34,  35,  36,  37,  38,  39, 
    40,  41,  42,  43,  44,  45,  46,  47, 
    48,  49,  50,  51,  52,  53,  54,  55, 
    56,  57,  58,  59,  60,  61,  62,  63, 
    64,  65,  66,  67,  68,  69,  70,  71, 
    72,  73,  74,  75,  76,  77,  78,  79, 
    80,  81,  82,  83,  84,  85,  86,  87, 
    88,  89,  90,  91,  92,  93,  94,  95, 
    96,  97,  98,  99, 100, 101, 102, 103, 
    104, 105, 106, 107, 108, 109, 110, 111, 
    112, 113, 114, 115, 116, 117, 118, 119, 
    120, 121, 122, 123, 124, 125, 126, 127, 
    128, 129, 130, 131, 132, 133, 134, 135, 
    136, 137, 138, 139, 140, 141, 142, 143, 
    144, 145, 146, 147, 148, 149, 150, 151, 
    152, 153, 154, 155, 156, 157, 158, 159, 
    160, 161, 162, 163, 164, 165, 166, 167, 
    168, 169, 170, 171, 172, 173, 174, 175, 
    176, 177, 178, 179, 180, 181, 182, 183, 
    184, 185, 186, 187, 188, 189, 190, 191, 
    192, 193, 194, 195, 196, 197, 198, 199, 
    200, 201, 202, 203, 204, 205, 206, 207, 
    208, 209, 210, 211, 212, 213, 214, 215, 
    216, 217, 218, 219, 220, 221, 222, 223, 
    224, 225, 226, 227, 228, 229, 230, 231, 
    232, 233, 234, 235, 236, 237, 238, 239, 
    240, 241, 242, 243, 244, 245, 246, 247, 
    248, 249, 250, 251, 252, 253, 254, 255, 
  };

  static uint8_t UNUSED the_sockets[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 
  };

#  endif  /* ALTERNATE_CORES */
#endif	/* T44*/


#ifdef __sparc__
#  define PAUSE    asm volatile("rd    %%ccr, %%g0\n\t" \
				::: "memory")

#elif defined(__tile__)
#  define PAUSE cycle_relax()
#else
#  define PAUSE _mm_pause()
#endif
  static inline void
  pause_rep(uint32_t num_reps)
  {
    uint32_t i;
    for (i = 0; i < num_reps; i++)
      {
	PAUSE;
	/* PAUSE; */
	/* asm volatile ("NOP"); */
      }
  }

  static inline void
  nop_rep(uint32_t num_reps)
  {
    uint32_t i;
    for (i = 0; i < num_reps; i++)
      {
	asm volatile ("");
      }
  }


  //machine dependent parameters
#if defined(DEFAULT)
#  define NUMBER_OF_SOCKETS 1
#  define CORES_PER_SOCKET  CORE_NUM
#  define CACHE_LINE_SIZE   64
#  define NOP_DURATION      2
  static uint8_t __attribute__ ((unused)) the_cores[] =
    {
      0, 1, 2, 3, 4, 5, 6, 7, 
      8, 9, 10, 11, 12, 13, 14, 15, 
      16, 17, 18, 19, 20, 21, 22, 23, 
      24, 25, 26, 27, 28, 29, 30, 31, 
      32, 33, 34, 35, 36, 37, 38, 39, 
      40, 41, 42, 43, 44, 45, 46, 47  
    };
#endif  /*  */  


#ifdef MAGLITE
#  define NUMBER_OF_SOCKETS 8
#  define CORES_PER_SOCKET 8
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 9
  //mapping from threads to cores on the niagara
#  define ALTERNATE_SOCKETS
#  ifdef ALTERNATE_SOCKETS
  static uint8_t __attribute__ ((unused)) the_cores[] = {
    0, 8, 16, 24, 32, 40, 48, 56, 
    1, 9, 17, 25, 33, 41, 49, 57, 
    2, 10, 18, 26, 34, 42, 50, 58, 
    3, 11, 19, 27, 35, 43, 51, 59, 
    4, 12, 20, 28, 36, 44, 52, 60, 
    5, 13, 21, 29, 37, 45, 53, 61, 
    6, 14, 22, 30, 38, 46, 54, 62, 
    7, 15, 23, 31, 39, 47, 55, 63 
  };

  static uint8_t __attribute__ ((unused)) the_sockets[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 
    2, 2, 2, 2, 2, 2, 2, 2, 
    3, 3, 3, 3, 3, 3, 3, 3, 
    4, 4, 4, 4, 4, 4, 4, 4, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 6, 6, 6, 6, 6, 6, 6, 
    7, 7, 7, 7, 7, 7, 7, 7 
  };

  //static uint8_t __attribute__ ((unused)) the_sockets[] = {
  //    0, 1, 2, 3, 4, 5, 6, 7, 
  //    0, 1, 2, 3, 4, 5, 6, 7, 
  //    0, 1, 2, 3, 4, 5, 6, 7, 
  //    0, 1, 2, 3, 4, 5, 6, 7, 
  //    0, 1, 2, 3, 4, 4, 5, 7, 
  //    0, 1, 2, 3, 4, 5, 6, 7, 
  //    0, 1, 2, 3, 4, 5, 6, 7, 
  //    0, 1, 2, 3, 4, 5, 6, 7 
  //};

#  else
  static uint8_t __attribute__ ((unused)) the_cores[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 
    8, 9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 30, 31, 
    32, 33, 34, 35, 36, 37, 38, 39, 
    40, 41, 42, 43, 44, 45, 46, 47, 
    48, 49, 50, 51, 52, 53, 54, 55, 
    56, 57, 58, 59, 60, 61, 62, 63 
  };
  static uint8_t __attribute__ ((unused)) the_sockets[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 
    2, 2, 2, 2, 2, 2, 2, 2, 
    3, 3, 3, 3, 3, 3, 3, 3, 
    4, 4, 4, 4, 4, 4, 4, 4, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 6, 6, 6, 6, 6, 6, 6, 
    7, 7, 7, 7, 7, 7, 7, 7 
  };

#  endif
#endif	/* MAGLITE */

#if defined __tile__
#  define NUMBER_OF_SOCKETS 1
#  define CORES_PER_SOCKET 36
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 4
  static uint8_t __attribute__ ((unused)) the_cores[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 
    8, 9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 30, 31, 
    32, 33, 34, 35 
  };
#endif	/*  */


#if defined(OPTERON)
#  define NUMBER_OF_SOCKETS 8
#  define CORES_PER_SOCKET 6
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 2
  static uint8_t  UNUSED the_cores[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 
    8, 9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 30, 31, 
    32, 33, 34, 35, 36, 37, 38, 39, 
    40, 41, 42, 43, 44, 45, 46, 47  
  };
#endif	/*  */

#if defined(HASWELL) || defined(IGORLAPTOPLINUX) || defined(LPDPC4)
#  define NUMBER_OF_SOCKETS 1
#  define CORES_PER_SOCKET 8
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 2
  static UNUSED uint8_t  the_cores[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
  };

  static __attribute__ ((unused)) double static_power[3] = { 0, 0, 0 };
  static __attribute__ ((unused)) double eng_per_test_iter_nj[8][5] = 
    {
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
    };
#endif	/*  */

#if defined(OANALAPTOPLINUX)
#  define NUMBER_OF_SOCKETS 1
#  define CORES_PER_SOCKET 2
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 2
  static uint8_t  the_cores[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 
    8, 9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 30, 31, 
    32, 33, 34, 35, 36, 37, 38, 39, 
    40, 41, 42, 43, 44, 45, 46, 47  
  };
#endif  /*  */  

#if defined(XEON)
#  define NUMBER_OF_SOCKETS 8
#  define CORES_PER_SOCKET 10
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 1
  static uint8_t __attribute__ ((unused)) the_cores[] = {
    1,2,3,4,5,6,7,8,9,10,	/* 0 */
    11,12,13,14,15,16,17,18,19,20, /* 1 */
    21,22,23,24,25,26,27,28,29,30, /* 2 */
    31,32,33,34,35,36,37,38,39,40, /* 3 */

    41,42,43,44,45,46,47,48,49,50, /* 0 */
    51,52,53,54,55,56,57,58,59,60, /* 1 */
    61,62,63,64,65,66,67,68,69,70,  /* 2 */
    71,72,73,74,75,76,77,78,79,80, /* 3 */

    0,81,82,83,84,85,86,87,88,89,  /* 4 */
    90,91,92,93,94,95,96,97,98,99, /* 5 */
    100,101,102,103,104,105,106,107,108,109, /* 6 */
    110,111,112,113,114,115,116,117,118,119, /* 7 */

    120,121,122,123,124,125,126,127,128,129, /* 4 */
    130,131,132,133,134,135,136,137,138,139, /* 5 */
    140,141,142,143,144,145,146,147,148,149, /* 6 */
    150,151,152,153,154,155,156,157,158,159,  /* 7 */


  };
  static uint8_t __attribute__ ((unused)) the_sockets[] =
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  };

#endif

#if defined(LPDXEON)
#  define NUMBER_OF_SOCKETS 2
#  define CORES_PER_SOCKET 20
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 2	/* have not actually measured it! */

#  define USE_HYPERTRHEADS 0	/* use first all the hyperthreads of one socket if set */

  static __attribute__ ((unused)) double static_power[3] = { 55.352036, 27.152955, 28.199081 };

#  if USE_HYPERTRHEADS == 1
  static uint8_t UNUSED the_cores[] = 
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
      10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
      30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
    }; 

static __attribute__ ((unused)) double eng_per_test_iter_nj[40][5] = 
  {
    { 229.69729598644526791474, 155.66214662469074066401, 81.83660070338615833062, 74.03514936175452725073, 73.82554592130458233338 },
    { 129.92729806775347849450, 92.87097637973462891891, 56.00234334076527234995, 37.05632168801884957558, 36.86863303896935656896 },
    { 90.41777757464943248965, 65.52330993301908554940, 40.91458270191838825779, 24.89446764163034694025, 24.60872723110069729160 },
    { 71.70503359096182987812, 53.24135231318132056255, 34.80945251967976791088, 18.46368127778050931557, 18.43189979350155265166 },
    { 61.46753870671294508202, 46.78646439258800146428, 32.03633878459513542680, 14.68107315213430217184, 14.75012676998350748336 },
    { 54.97594655229231348292, 42.71548104800787877290, 30.41308814787980294890, 12.26046550428443471002, 12.30239193218561013831 },
    { 51.02304494347293687860, 40.37056455594769643394, 29.76741673354698132698, 10.65248038752524044465, 10.60314782240071510696 },
    { 46.66188950826064796324, 37.48114377688081847622, 28.25001855878092773848, 9.18074573137982948701, 9.23112521809989073774 },
    { 43.82981659933958230283, 35.69367323680874111720, 27.49950147952476895043, 8.13614336253084118562, 8.19417175728397216677 },
    { 41.69095949967046461199, 34.30773768541027309793, 26.91831862599122132592, 7.38322181426019151406, 7.38941905941905177200 },
    { 40.59293050480954019531, 33.45212847545872453059, 26.30114873118361091350, 7.14080202935081566471, 7.15097974427511361709 },
    { 40.44686414650826126024, 33.31671652956449391896, 26.27224100478459902189, 7.13014761694376734127, 7.04447552477989489707 },
    { 38.63400068948118319131, 31.91860005978480917390, 25.19750824179147185110, 6.71540062969637401740, 6.72109181799333732280 },
    { 37.67909047223780921291, 31.18991524978350760173, 24.66098999594509043423, 6.48917522245430161117, 6.52892525383841716750 },
    { 37.48723976518549914509, 31.06633912116336134131, 24.60876432712765744925, 6.42090064402213780378, 6.45757479403570389206 },
    { 35.93501879430925271111, 29.75867009353556265858, 23.58547726220328510678, 6.17634870077369005252, 6.17319283133227755179 },
    { 35.05017234730261831232, 29.09994805102562282020, 23.10239243045587092387, 5.95022429627699549212, 5.99755609020108517101 },
    { 34.33623863130784173523, 28.53358911826720240123, 22.69320957257437978096, 5.80264951304063933400, 5.84037954569282262026 },
    { 33.62212279422026234553, 27.97741535442858221174, 22.29040838672357529177, 5.64470743979168013378, 5.68700696770500691997 },
    { 32.96013040831562785054, 27.45807499800290621390, 21.91855412561404127317, 5.50205541031272163663, 5.53952087238886494072 },
    { 47.28979119370549142660, 37.57742259581472526086, 27.12930484663258689462, 9.71236859789076616574, 10.44811774918213836623 },
    { 45.04223960057763314934, 36.12670245507833406360, 26.35758635758431850058, 8.91553714549929908573, 9.76911609749401556301 },
    { 43.67058189530530121470, 35.30521274013663281700, 26.11846887938988574819, 8.36536915516866839769, 9.18674386074674706881 },
    { 42.60576732090060253270, 34.66262408187614412303, 25.99871539418099355935, 7.94314323902445840966, 8.66390902206491067188 },
    { 41.43785484784768595987, 33.96582093092079759619, 25.76444895626643043996, 7.47203391692688836368, 8.20137197465436715622 },
    { 40.52741384104634466711, 33.41888347027097869475, 25.64406424437515648596, 7.10853037077536597235, 7.77481922589582220879 },
    { 39.73059107478248313233, 32.98011789158887275831, 25.57809009720159869296, 6.75047289800730009821, 7.40202779438727406534 },
    { 38.94634111648463567134, 32.51279081568074916725, 25.46060084018057458151, 6.43355030080388650408, 7.05219024731822665330 },
    { 38.16383202841839362803, 32.03052300604605296144, 25.28963494582244393904, 6.13330902237234066659, 6.74088831997533912918 },
    { 37.63081417152950568538, 31.70806299031871792150, 25.24062036465074840362, 5.92275093244654732551, 6.46744287443220995623 },
    { 37.40460815458195975134, 31.49719966424260718405, 25.10160606515992572487, 5.90740824439235785549, 6.39559384502967617097 },
    { 36.70523797166752357139, 31.01720151497208405688, 24.72886652361023523840, 5.68803645669543951451, 6.28833474977085589582 },
    { 36.36230898406250579150, 30.71837596401063154148, 24.51039400569840274168, 5.64393325838237797544, 6.20798171998172507437 },
    { 35.94203972550205584335, 30.38581980198740920074, 24.26089097066515915434, 5.55621992351464664261, 6.12492883132225004640 },
    { 35.57800034292794851435, 30.07757882236843739458, 24.03853750233998553733, 5.50042152055951111976, 6.03904132002845185724 },
    { 35.39234634574758951660, 29.89126654242762491350, 23.91805792786781963610, 5.50107980331996460310, 5.97320861455980527740 },
    { 34.82555044972040436622, 29.49492132651811652940, 23.61327246397056092310, 5.33062934896130439254, 5.88164886254755560630 },
    { 34.46029740456764074470, 29.19381617075399181019, 23.38893084166544372580, 5.26648123381364893451, 5.80488555171465413568 },
    { 34.12928397922275831757, 28.91513972025352938497, 23.18354067760409305115, 5.21414425896922893259, 5.73159904264943633382 },
    { 34.01931977364001659009, 28.78915228904349525819, 23.09106190310396673079, 5.23016748459652133189, 5.69809038593952852740 },
  };

#  elif USE_HYPERTRHEADS == 2
  static uint8_t UNUSED the_cores[] = 
    {
      0, 20, 1, 21, 2, 22, 3, 23, 4, 24, 5, 25, 6, 26, 7, 27, 8, 28, 9, 29,
      10, 30, 11, 32, 12, 32, 13, 33, 14, 34, 15, 35, 16, 36, 17, 37, 18, 38, 19, 39, 
    }; 

#  else
  
  static uint8_t UNUSED the_cores[] = 
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
      10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
      30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
    };

  static __attribute__ ((unused)) double eng_per_test_iter_nj[40][5] = 
    {
      { 234.60202230853298000266, 157.78963908775246754635, 80.44831687022313379144, 76.81238322078051245630, 77.34131612172547360784 },
      { 130.19996880311263288606, 91.56731092586975276041, 52.80283160655244668894, 38.63265787724288012565, 38.76447931931730607146 },
      { 97.91595546524582008240, 72.39697050602267259573, 46.60342225213650460048, 25.51898495922314748667, 25.79354825388616799524 },
      { 77.04375160600302599006, 57.57828792397870397323, 38.21962801434882946454, 19.46546215830228597312, 19.35866143335191055239 },
      { 66.60063321661194166091, 51.15731276822387036897, 35.66530615337983236951, 15.44332044838807129193, 15.49200661484403799946 },
      { 57.49899387962235952981, 44.58655908661531318309, 31.68513275476549920752, 12.91243479300704634671, 12.90142633184981397557 },
      { 53.27035061266595667227, 42.10006040481277496608, 30.96939543760024990670, 11.17029108323957857484, 11.13066409182612819072 },
      { 48.92342769050576551962, 39.24447490279040370380, 29.56886689364194497347, 9.67895202667359460237, 9.67560800914845873033 },
      { 46.12214510779135114825, 37.45366351767337511821, 28.83407196255519128182, 8.66848226745290259961, 8.61959155511818383639 },
      { 43.63624377096353945724, 35.89729262149872662820, 28.14989135969805710956, 7.73895114946481282903, 7.74740065317834627917 },
      { 62.26695987361801406190, 48.98600780801804534908, 34.50521338743658272535, 13.28095206559996871281, 14.48079498170199685330 },
      { 58.81986970966034325330, 46.64160874974837420037, 33.47459881463837642843, 12.17826045039200038526, 13.16700993510999777193 },
      { 55.77327394848195568283, 44.59252612218050592723, 32.45971838900925961332, 11.18074782630144975560, 12.13280773317124631390 },
      { 53.35587288745431262924, 43.02595839718354181266, 31.76556645416326274830, 10.32991492545868382811, 11.26039194302027906436 },
      { 51.22436410494622750896, 41.60525336637767574119, 31.08686360026960446088, 9.61911114484092710457, 10.51838976610807128030 },
      { 50.72842739157257898552, 41.36400358256455644076, 31.45162462316764386700, 9.36442380900802254475, 9.91237857683589650565 },
      { 48.99046398301495587324, 40.32421848749861061920, 30.99405693133628881203, 8.66624585523240489317, 9.33016155616232180716 },
      { 48.11277574019122570714, 39.71043989030147027573, 30.87415450267398697528, 8.40233584988975543140, 8.83628538762748330044 },
      { 47.27696104262690915577, 39.07910823119677425900, 30.69454806615792056010, 8.19785313422370833973, 8.38455984224528025593 },
      { 45.16170140463251706290, 37.66052939798363717429, 29.68952434502926891488, 7.50117231340289999367, 7.97100474620034815434 },
      { 44.51583685738054533766, 37.15204737263273381387, 29.32370958651938854100, 7.36378948474781152379, 7.82833778611334527287 },
      { 43.43013825275178117605, 36.45475025278843083354, 28.78995224052679450709, 6.97538799996335034250, 7.66479830697727120339 },
      { 42.94690003756602036974, 36.04904963824908174182, 28.51084325552140461565, 6.89785039931693862791, 7.53820638272767712617 },
      { 42.26067973923170856435, 35.53403983813456176836, 28.13027516609941735845, 6.72663990109714679598, 7.40376467203514440991 },
      { 41.98618856775846311797, 35.24569998142351115429, 27.93292096951690480387, 6.74048858633495196368, 7.31277873099243677336 },
      { 41.20836493276575472375, 34.68626534514603305665, 27.50999590394027310286, 6.52209986326137167710, 7.17626916556410994378 },
      { 40.87150736222739389167, 34.36797495572245203629, 27.28895716569301410588, 6.50353267815879815155, 7.07901751837558163423 },
      { 40.25219121707161961689, 33.91928887901263807204, 26.96299045247495756590, 6.33290233805898154485, 6.95629815954399156785 },
      { 39.83391484521466729567, 33.59152539482701028605, 26.73460514635079961834, 6.24238945038765700961, 6.85692024847621066771 },
      { 39.55126277140646402901, 33.33080507859673118252, 26.56300538880562612737, 6.22045769280973284649, 6.76779968979110505514 },
      { 38.96658623593443610402, 32.90791002776678688417, 26.25382275043740832491, 6.05867620816764921985, 6.65408702222620201318 },
      { 38.68918927266693359830, 32.65106439580023766058, 26.07516254009421377975, 6.03812512886984064766, 6.57590185570602388082 },
      { 38.16034503480971312149, 32.31342179350376932966, 25.83713511502098060769, 5.84692324130594379183, 6.47628667848278872196 },
      { 37.91648062860701649610, 32.02772316504159186846, 25.64230586534548387014, 5.88875746356542462763, 6.38541754433334922594 },
      { 37.28148927946042406843, 31.59798786276086677330, 25.31658436857910177437, 5.68350141669955729513, 6.28140349418176499893 },
      { 37.02177497269500577004, 31.36130736103363126266, 25.15403294750756878376, 5.66046761166137450738, 6.20727441352606247890 },
      { 36.52273643324810166725, 30.98044786895787129327, 24.86612807705081168421, 5.54228833027635982942, 6.11431979190705960906 },
      { 36.19653408528347033103, 30.72216019713365534917, 24.68578867382584546101, 5.47437411908338849177, 6.03637152330780988816 },
      { 36.08325306161539608844, 30.59647911797491177361, 24.61054348764035785114, 5.48677371485792065615, 5.98593563033455392247 },
      { 35.52336201872074272167, 30.19903621272641681415, 24.31628487828975205780, 5.32432580599432590751, 5.88275133443666475635 },
    };
#  endif
#endif

#if defined(LAPTOP)
#  define NUMBER_OF_SOCKETS 1
#  define CORES_PER_SOCKET 8
#  define CACHE_LINE_SIZE 64
#  define NOP_DURATION 1
  static uint8_t UNUSED the_cores[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 
    8, 9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 30, 31, 
    32, 33, 34, 35, 36, 37, 38, 39, 
    40, 41, 42, 43, 44, 45, 46, 47  
  };
  static uint8_t UNUSED the_sockets[] = 
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };

#endif


  /* PLATFORM specific -------------------------------------------------------------------- */
#if !defined(PREFETCHW)
#  if defined(__x86_64__)
#    define PREFETCHW(x)		     asm volatile("prefetchw %0" :: "m" (*(unsigned long *)x))
#  elif defined(__sparc__)
#    define PREFETCHW(x)		
#  elif defined(XEON)
#    define PREFETCHW(x)		
#  else
#    define PREFETCHW(x)		
#  endif
#endif 

  //debugging functions
#ifdef DEBUG
#  define DPRINT(args...) fprintf(stderr,args);
#  define DDPRINT(fmt, args...) printf("%s:%s:%d: "fmt, __FILE__, __FUNCTION__, __LINE__, args)
#else
#  define DPRINT(...)
#  define DDPRINT(fmt, ...)
#endif



  static inline int get_cluster(int thread_id) {
#ifdef __solaris__
    if (thread_id>64){
      perror("Thread id too high");
      return 0;
    }
    return thread_id/CORES_PER_SOCKET;
    //    return the_sockets[thread_id];
#elif XEON
    if (thread_id>=80){
      perror("Thread id too high");
      return 0;
    }

    return the_sockets[thread_id];
#elif defined(__tile__)
    return 0;
#else
    return thread_id/CORES_PER_SOCKET;
#endif
  }

  static inline double wtime(void)
  {
    struct timeval t;
    gettimeofday(&t,NULL);
    return (double)t.tv_sec + ((double)t.tv_usec)/1000000.0;
  }

  static inline 
  void set_cpu(int cpu) 
  {
#ifndef NO_SET_CPU
#  ifdef __sparc__
    processor_bind(P_LWPID,P_MYID, cpu, NULL);
#  elif defined(__tile__)
    if (cpu>=tmc_cpus_grid_total()) {
      perror("Thread id too high");
    }
    // cput_set_t cpus;
    if (tmc_cpus_set_my_cpu(cpu)<0) {
      tmc_task_die("tmc_cpus_set_my_cpu() failed."); 
    }    
#  else
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
#    if defined(PLATFORM_NUMA)
    numa_set_preferred(get_cluster(cpu));
#    endif
    pthread_t thread = pthread_self();
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &mask) != 0) 
      {
	fprintf(stderr, "Error setting thread affinity\n");
      }
#  endif
#endif
  }


  static inline void cdelay(ticks cycles){
    ticks __ts_end = getticks() + (ticks) cycles;
    while (getticks() < __ts_end);
  }

  static inline void cpause(ticks cycles){
#if defined(XEON)
    cycles >>= 3;
    ticks i;
    for (i=0;i<cycles;i++) {
      _mm_pause();
    }
#else
    ticks i;
    for (i=0;i<cycles;i++) {
      __asm__ __volatile__("nop");
    }
#endif
  }

  static inline void udelay(unsigned int micros)
  {
    double __ts_end = wtime() + ((double) micros / 1000000);
    while (wtime() < __ts_end);
  }

  //getticks needs to have a correction because the call itself takes a
  //significant number of cycles and skewes the measurement
  extern ticks getticks_correction_calc();

  static inline ticks get_noop_duration() {
#define NOOP_CALC_REPS 1000000
    ticks noop_dur = 0;
    uint32_t i;
    ticks corr = getticks_correction_calc();
    ticks start;
    ticks end;
    start = getticks();
    for (i=0;i<NOOP_CALC_REPS;i++) {
      __asm__ __volatile__("nop");
    }
    end = getticks();
    noop_dur = (ticks)((end-start-corr)/(double)NOOP_CALC_REPS);
    return noop_dur;
  }

  /// Round up to next higher power of 2 (return x if it's already a power
  /// of 2) for 32-bit numbers
  static inline uint32_t pow2roundup (uint32_t x){
    if (x==0) return 1;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
  }

#ifdef __cplusplus
}

#endif


#endif