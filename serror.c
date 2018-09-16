/**********************************************************************
 *                                                                    *
 *    serror functions                                                *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/05/02 02:21:49 kiyoshi>          *
 *                                                                    *
 **********************************************************************/


#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "serror.h"


/********************************************************************

  ERR_warn function
  Last Modified: 2001/01/08
*********************************************************************/

/* This function takes variable arguments to print the erorr.
   and continue the program.

   */

void error_warn( const char *fmt, ... )
{
 
  va_list args;
  assert(fmt);

  fflush(stderr);
  fprintf( stderr, "warning: " );

  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  if( fmt[0] != '\0' && fmt[strlen(fmt) - 1] == ':')
    fprintf(stderr, " %s", strerror(errno));
  fprintf( stderr, "\n");

}

/********************************************************************

  error_exit function
  Last Modified: 2001/01/08
*********************************************************************/

/* This function takes variable arguments to print the erorr.
   and exit the program.

   */
void error_exit( const char *fmt, ... )
{
 
  va_list args;
  assert(fmt);

  fflush(stderr);

  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  if( fmt[0] != '\0' && fmt[strlen(fmt) - 1] == ':')
    fprintf(stderr, " %s", strerror(errno));
  fprintf( stderr, "\n");
  exit(2);

}


void debug_print( const char *file, int line, const char *fmt, ... )
{
 
  va_list args;
  assert(fmt);

  fflush(stderr);
  fprintf( stderr, "debug: " );

  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  if( fmt[0] != '\0' && fmt[strlen(fmt) - 1] == ':')
    fprintf(stderr, "%s", strerror(errno));
  if( file && line > 0 )
    fprintf( stderr, " raised at %s:%d", file, line );

  fprintf( stderr, "\n");

}
