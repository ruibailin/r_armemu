
/*---------------------------
 * rgb2yuv.c
 * Create:  2021-02-18
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */


#include "../1imp.h"

/**************************************************/
BYTE rgb_get_y(BYTE r,BYTE g,BYTE b);
BYTE rgb_get_y(BYTE r,BYTE g,BYTE b)
{
	short int c;
	BYTE t;
	c= 66 * r + 129 * g +  25 * b + 128;
	c >>= 8;

	t=(BYTE)c;
	t += 16;
	return(t);
}
/**************************************************/
BYTE rgb_get_u(BYTE r,BYTE g,BYTE b);
BYTE rgb_get_u(BYTE r,BYTE g,BYTE b)
{
	short int c;
	BYTE t;
	c= -38 * r -  74 * g + 112 * b + 128;
	c >>= 8;

	t=(BYTE)c;
	t += 128;
	return(t);
}

/**************************************************/
BYTE rgb_get_v(BYTE r,BYTE g,BYTE b);
BYTE rgb_get_v(BYTE r,BYTE g,BYTE b)
{
	short int c;
	BYTE t;
	c= 112 * r -  94 * g -  18 * b + 128;
	c >>= 8;

	t=(BYTE)c;
	t += 128;
	return(t);
}

/* end of rgb2yuv.c */
