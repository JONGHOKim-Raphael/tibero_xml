#ifndef _DEBUG_H
#define _DEBUG_H


#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)    /* Don't do anything in release builds */
#endif


#endif   // _DEBUG_H
