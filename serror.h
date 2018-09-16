/**********************************************************************
 *                                                                    *
 *    serror function declarations                                    *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/05/02 02:16:42 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

#ifndef SERROR_H
#define SERROR_H

/* Definitions for serror functions. The functions are basically 
   used as the debug ones.

   NOTE: ANSI C does not allow macro with variable arguments
   */

#define DEBUG(fmt, args...) debug_print( __FILE__, __LINE__, fmt, ##args)

void debug_print( const char *file, int line, const char *fmt, ... );
void error_warn( const char *fmt, ... );
void error_exit( const char *fmt, ... ); 

/* This might be used in the future 
typedef struct ERROR_STATE {
  char *reason;
}ERROR_STATE;
*/

#endif
