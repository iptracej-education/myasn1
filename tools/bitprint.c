#include <stdio.h>
#include <limits.h>

void bit_print_char(char a)
{
  int i;
  char mask;
  int n = CHAR_BIT;
  mask = 1 << (n-1);

  for(i=1;i<=n;++i){
    putchar(((a & mask) == 0) ? '0':'1');
    a <<=1;
    if(i % CHAR_BIT == 0 && i < n)
      putchar(' ');
  }
  putchar('\n');
}

void bit_print_int(int a)
{
  int i;
  int mask;
  int n = sizeof(int)*CHAR_BIT;
  mask = 1 << (n-1);

  for(i=1;i<=n;++i){
    putchar(((a & mask) == 0) ? '0':'1');
    a <<=1;
    if(i % CHAR_BIT == 0 && i < n)
      putchar(' ');
  }
  putchar('\n');
}





