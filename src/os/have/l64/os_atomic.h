
/*------------------------------------
 * atomic.h
 * Create:  2021-01-24
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */


#ifndef INCLUDE_UTIL_ATOMIC_H_
#define INCLUDE_UTIL_ATOMIC_H_
/*================================================================*/
extern int atomic_add(volatile int *val, int incr);
extern int atomic_and(volatile int *val, int incr);
extern int atomic_or(volatile int *val, int incr);
extern int atomic_set(volatile int *val, int set_to);
extern int test_and_set(volatile int *val, int set_to, int test_val);

/*================================================================*/
#endif 
/* end of atomic.h */
