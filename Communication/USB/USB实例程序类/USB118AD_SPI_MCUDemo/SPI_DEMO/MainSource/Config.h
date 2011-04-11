//============================================================================================================
//
//		头文件：		Target.h
//
//============================================================================================================

//============================================================================================================
#ifndef __My_Target_H_
#define __My_Target_H_
//============================================================================================================



//============================================================================================================
//	引用头文件
//============================================================================================================
#include	<LPC2103.h>


//============================================================================================================
#define	cTarget_CClk	48000000
#define	cTarget_PClk	cTarget_CClk/4
//============================================================================================================

//============================================================================================================
#define	cPB_LED0	(1<<10)
#define	cPB_LED1	(1<<11)
#define	cPB_LED2	(1<<12)
#define	cPB_LED3	(1<<13)

#define	cPB_BUT0	(1<<15)
#define	cPB_BUT1	(1<<16)

#define	cPB_UMnRST	(1<<18)

#define	cPB_SSEL0	(1<<7 )
#define	cPB_SSEL1	(1<<21)

#define	cPB_OutPort	(cPB_LED0|cPB_LED1|cPB_LED2|cPB_LED3|cPB_UMnRST|cPB_SSEL0|cPB_SSEL1)
//============================================================================================================

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;


typedef unsigned char  __u8;
typedef unsigned short __u16;
typedef unsigned long  __u32;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

#define __swap_32(x) \
	(\
		((x & 0x000000FF) << 24) | \
		((x & 0x0000FF00) <<	 8) | \
		((x & 0x00FF0000) >>	 8) | \
		((x & 0xFF000000) >> 24)  \
	)




//============================================================================================================
#endif  // __My_Target_H_
//============================================================================================================
//
//			End	of	File
//
//============================================================================================================
