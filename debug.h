#ifndef _DEBUG_H
#define _DEBUG_H


#ifdef DEBUG
#define DEBUG_ERROR(fmt, args...)    fprintf(stderr, fmt, ## args)
#define DEBUG_INFO(fmt, args...)     fprintf(stdout, fmt, ## args)
#else
#define DEBUG_ERROR(fmt, args...)    /* Don't do anything in release builds */
#define DEBUG_INFO(fmt, args...)     /* Don't do anything */
#endif


#endif   // _DEBUG_H
