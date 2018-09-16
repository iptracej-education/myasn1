#include <iconv.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "myASN1.h"

void writeUTF8String(STREAM *stream, BYTE *string, char *fromtype);
#define BUFSIZE 1028

/********************************************************************

  writeUTF8String function
  Last modifiled: Thursday 10.26.2000 by Kiyoshi Watanabe

*********************************************************************/

int main(int argc, char *argv[])
{
  
  char string[] = "これはテストです";
  char fromtype[] = "EUC-JP"; /* EUC-JP for Japanese */

  writeUTF8String(NULL, string, fromtype);
  exit(0);
}

void writeUTF8String(STREAM *stream, BYTE *string, char *fromtype)
{
  int i, len;
  BYTE outbuf[BUFSIZE];
  const char *ip;           /* should be const char pointer */
  char *op;
  size_t status;
  size_t inbytesleft;
  size_t outbytesleft;
  iconv_t cd;

  cd = iconv_open("UTF-8",fromtype);
  if(cd == (iconv_t)-1){
    perror("Error: iconv_open\n");
    exit(-1);
  }
  
  ip = string;
  op = outbuf;

  inbytesleft = strlen(string) + 1;
  outbytesleft = inbytesleft*3;

  status = iconv(cd, &ip, &inbytesleft, &op, &outbytesleft);
  if(status == (size_t)-1){
    perror("Error:iconv");
  }
  printf("outbuf = %s\n",outbuf);
  
  len = strlen(outbuf);
  printf("len %d\n",len);

  i = 0;
  while(i<len){ /* UTF-8 */
    /* while(outbuf[i] != '\0' || outbuf[i+1] != '\n'){  UCS-2 */
    bit_print_char(outbuf[i]);
    i++;
  }
  iconv_close(cd);
  exit(0);
}

/* 
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

*/
