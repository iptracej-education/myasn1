/**********************************************************************
 *                                                                    *
 *    General smemory functions                                       *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/02/14 23:51:10 kiyoshi>          *
 *                                                                    *
 **********************************************************************/


#include <assert.h>
#include <stdio.h>
#include "smemory.h"
#undef malloc

/********************************************************************

  MEM_alloc function
  Last Modified: 2001/01/07
*********************************************************************/

/* This fucntion takes the 3 arguments. 
   1) The sizeof(type)*number to be allocated, 
   2) The file that calls this fucntion for error checking, 
   3) The line that calls this function for error checking.

   For RAISE, see the smemory.h in detail.
   */
void *MEM_alloc( size_t size, const char *file, int line )
{
  
  void *new_memory;
  const EXCEPT_T MEM_allocationFailed = {"Allocation Failed"};
  
  /* Ask for the requested memory and check that we
     really got it.
   */

  assert( size > 0 );
  new_memory = malloc( size );
  if( new_memory == NULL ){
    if( file == NULL )
      RAISE( MEM_allocationFailed );
    else
      Except_raise( &MEM_allocationFailed, file, line);
  }
  return new_memory;
}

/********************************************************************

  MEM_realloc function
  Last Modified: 2001/01/07
*********************************************************************/

void *MEM_realloc( void *currentp, size_t nbytes, const char *file, int line )
{
  void *re_memory;
  const EXCEPT_T MEM_reallocationFailed = {"Re-Allocation Failed"};

  /* Ask for the requested memory and check that we really got it. */
  
  assert( nbytes > 0 );
  re_memory = realloc( currentp, nbytes );
  if( re_memory == NULL ){
    if( file == NULL )
      RAISE( MEM_reallocationFailed );
    else
      Except_raise( &MEM_reallocationFailed, file, line );
  }
  return re_memory;
}

/********************************************************************

  MEM_free function
  Last Modified: 2001/01/08
*********************************************************************/

void MEM_free( void *ptr, const char *file, int line ){
  if( ptr ){
    free( ptr );
    ptr = NULL; /* de-reference the pointer at the end here */
  }
  else{
    fprintf( stderr,"cannot free at %s:%d\n", file, line );
    fflush( stderr );
  }
}

/********************************************************************

  Except_raise function
  Last Modified: 2001/01/08
*********************************************************************/

/* This function takes 3 arguments to raise the erorr.
   1) The reason for error,
   2) The file that calls this function,
   3) The line that calls this fucntion.

   */
void Except_raise(const EXCEPT_T *e, const char *file, int line )
{
  assert(e);
  fprintf( stderr, "Exception is raised" );
  if( e->reason )
    fprintf( stderr, " %s", e->reason );
  else
    fprintf( stderr, " at 0x%p", e );
  if( file && line > 0 )
    fprintf( stderr, " raised at %s:%d\n", file, line );
  fprintf( stderr, "aborting...\n");
  fflush(stderr);
  abort();
}
      







