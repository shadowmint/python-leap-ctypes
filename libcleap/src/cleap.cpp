#include <stdio.h>

extern "C" 
{
  void junk(int x) {
    printf("%d\n", x);
  }
}
