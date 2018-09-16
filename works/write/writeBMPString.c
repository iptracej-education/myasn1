#include <iconv.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "myASN1.h"

void writeBMPString(STREAM *stream, BYTE *string, char *fromtype);

/********************************************************************

  writeBMPString function
  Last modifiled: Thursday 10.26.2000 by Kiyoshi Watanabe

*********************************************************************/
/* This is a bit strange, but will use the "\n" as the
   delimiter for the string when converting the ASCII or
   EUC-JP character to the UCS-2.  Please use "\n"
   on every BIT String to be converted. */


int main(int argc, char *argv[])
{

  char string[] = "こ";
  char fromtype[] = "ISO-2022-JP";

  /*
  char string[] = "KIYOSHI";
  char fromtype[] = "ASCII";
  */

  writeBMPString(NULL, string, fromtype);
  exit(0);
}

void writeBMPString(STREAM *stream, BYTE *string, char *fromtype)
{
  int i, len;
  BYTE *outbuf;
  const char *ip;           /* should be const char pointer */
  char *op;
  size_t status;
  size_t inbytesleft;
  size_t outbytesleft;
  iconv_t cd;

  cd = iconv_open("UCS-2",fromtype);
  if(cd == (iconv_t)-1){
    perror("Error: iconv_open\n");
    exit(-1);
  }

  inbytesleft = strlen(string) + 1;
  outbytesleft = inbytesleft*2;
  outbuf = MALLOC( char, outbytesleft );

  ip = string;
  op = outbuf;

  status = iconv(cd, &ip, &inbytesleft, &op, &outbytesleft);
  if(status == (size_t)-1){
    perror("Error:iconv");
  }
  
  printf("len %d\n",strlen( outbuf ) );

  i = 0;
  while(1){
    if( outbuf[i] == '\0' && outbuf[i+1] == '\0' ){
	break;
    }
    printf(" %x\n",outbuf[i]);
    /* bit_print_char(outbuf[i]); */
    i++;
  }
  
  FREE( outbuf );
  DEREF( outbuf );
  iconv_close(cd);
  exit(0);
}




