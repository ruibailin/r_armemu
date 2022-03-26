
/*------------------------------------
 * os_type.h
 * Create:  2021-01-31
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */

#include "0ctr.h"
#ifdef	_OS_HAVE_C64_
#ifndef OS_HAVE_C64_OS_TYPE_H_
#define OS_HAVE_C64_OS_TYPE_H_
/*================================================================*/
#ifndef boolean
#define boolean 	unsigned int
#endif

#ifndef bool
#define bool		unsigned int
#endif

#ifndef BOOL
#define BOOL		unsigned int
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef NULL
#define NULL 		(void *)0L
#endif

/*================================================================*/

#ifndef int8
#define int8 		char
#endif

#ifndef int16
#define int16 		short
#endif

#ifndef int32
#define int32 		int
#endif

#ifndef int64
#define int64 		long
#endif

#ifndef uint8
#define uint8 		unsigned char
#endif

#ifndef uint16
#define uint16 		unsigned short
#endif

#ifndef uint32
#define uint32 		unsigned int
#endif

#ifndef uint64
#define uint64 		unsigned long
#endif

/*================================================================*/

#ifndef int8_t
#define int8_t 		char
#endif

#ifndef int16_t
#define int16_t 	short
#endif

#ifndef int32_t
#define int32_t 	int
#endif

#ifndef int64_t
#define int64_t 	long
#endif

#ifndef uint8_t
#define uint8_t 	unsigned char
#endif

#ifndef uint16_t
#define uint16_t 	unsigned short
#endif

#ifndef uint32_t
#define uint32_t 	unsigned int
#endif

#ifndef uint64_t
#define uint64_t 	unsigned long
#endif

/*================================================================*/
/*My OS Style*/
#ifndef	BYTE
#define BYTE		unsigned char
#endif
#ifndef	WORD
#define WORD		unsigned short
#endif
#ifndef	DWORD
#define DWORD 		unsigned int
#endif
#ifndef	QWORD
#define QWORD		unsigned long
#endif

/*================================================================*/
/*My OS Style*/
#ifndef	byte
#define byte		unsigned char
#endif
#ifndef	word
#define word		unsigned short
#endif
#ifndef	dword
#define dword 		unsigned int
#endif
#ifndef	qword
#define qword		unsigned long
#endif

/*================================================================*/
/*ZTE Style*/
#ifndef	INT8
#define INT8        char
#endif
#ifndef	INT16
#define INT16		short
#endif
#ifndef	INT32
#define INT32		int
#endif
#ifndef	INT64
#define INT64		long
#endif
#ifndef	UINT8
#define UINT8		unsigned char
#endif
#ifndef	UINT16
#define UINT16		unsigned short
#endif
#ifndef	UINT32
#define UINT32		unsigned int
#endif
#ifndef	UINT64
#define UINT64		unsigned long
#endif

/*================================================================*/
/*FFMPEG Style*/
#ifndef Bit8s
#define Bit8s		char
#endif
#ifndef Bit16s
#define Bit16s		short
#endif
#ifndef Bit32s
#define Bit32s		int
#endif
#ifndef Bit64s
#define Bit64s		long
#endif
#ifndef Bit8u
#define Bit8u		unsigned char
#endif
#ifndef Bit16u
#define Bit16u		unsigned short
#endif
#ifndef Bit32u
#define Bit32u		unsigned int
#endif
#ifndef Bit64u
#define Bit64u		unsigned long
#endif

/*================================================================*/
/*Boshs Style*/
#ifndef s8
#define s8			char
#endif
#ifndef s16
#define s16			short
#endif
#ifndef s32
#define s32			int
#endif
#ifndef s64
#define s64			long
#endif
#ifndef u8
#define u8			unsigned char
#endif
#ifndef u16
#define u16			unsigned short
#endif
#ifndef u32
#define u32			unsigned int
#endif
#ifndef u64
#define u64			unsigned long
#endif

/*================================================================*/
#endif
#endif 
/* end of os_type.h */
