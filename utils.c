/**********************************************************************
 *                                                                    *
 *    myASN1 utilities functions                                      *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe   cert.bbcert.org                  *
 *    Last modified: Time-stamp: <00/12/20 01:01:11 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

#include <limits.h> 
#include "myASN1.h"

void bit_print_char(char input)
{
  int i;
  const int max_c = CHAR_BIT;
  char bit_mask = 1 << (max_c - 1);

  for(i = 1; i <= max_c; ++i){
    putchar(((input & bit_mask) == 0) ? '0':'1');
    input <<= 1;
    if(i % CHAR_BIT == 0 && i < max_c)
      putchar(' ');
  }
  putchar('\n');
}

void bit_print_int(int input)
{
  int i;
  int max_c = sizeof(int)*CHAR_BIT;
  int bit_mask = bit_mask = 1 << (max_c - 1);

  for(i=1;i<=max_c;++i){
    putchar(((input & bit_mask) == 0) ? '0':'1');
    input <<= 1;
    if(i % CHAR_BIT == 0 && i < max_c)
      putchar(' ');
  }
  putchar('\n');
}
