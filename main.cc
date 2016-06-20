#include <iostream>
#include <cassert>
#include <time.h>
#include <vector>
#include <stack>

#include "memory.h"

using namespace std;

#define ELEMS 100000
#define REPS  50

int main() {
   clock_t start;
   
   MemoryPool<size_t> pool;
   start = clock();
   for (int i=0; i < REPS; ++i) {
      for (int j = 0; j< ELEMS; ++j) {
          size_t s = 4; 
          size_t* x = pool.newElement(s);
          pool.deleteElement(x);
      }
   }
   cout << "Memory Pool Time: ";
   cout << (((double)clock()-start) / CLOCKS_PER_SEC) << "\n\n";

   start = clock();
   for (int i = 0; i < REPS; ++i) {
       for (int j = 0; j < ELEMS; ++j) {
          size_t* x = new size_t;
          delete x;
       }
   }
   cout << "new/delete Time: ";
   cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";
   return 0;
} 
