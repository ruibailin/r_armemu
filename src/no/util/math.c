
/*------------------------------------
 * math.c
 * Create:  2021-01-30
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */


/*================================================================*/
int clz(unsigned int val);
int clz(unsigned int val)
{
    if (val == 0)
        return 32;
    else
        return __builtin_clz(val); // XXX gccism
}

/*================================================================*/

/* end of math.c */