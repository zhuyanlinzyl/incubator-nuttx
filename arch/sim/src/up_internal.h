/**************************************************************************
 * up_internal.h
 *
 *   Copyright (C) 2007, 2009, 2011 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************/

#ifndef __ARCH_UP_INTERNAL_H
#define __ARCH_UP_INTERNAL_H

/**************************************************************************
 * Included Files
 **************************************************************************/

#include <nuttx/config.h>
#include <sys/types.h>
#include <nuttx/irq.h>

/**************************************************************************
 * Pre-processor Definitions
 **************************************************************************/
/* Configuration **********************************************************/

#ifndef CONFIG_SIM_X11FB
#  ifdef CONFIG_SIM_TOUCHSCREEN
#    error "CONFIG_SIM_TOUCHSCREEN depends on CONFIG_SIM_X11FB"
#    undef CONFIG_SIM_TOUCHSCREEN
#  endif
#endif

#ifndef CONFIG_INPUT
#  ifdef CONFIG_SIM_TOUCHSCREEN
#    error "CONFIG_SIM_TOUCHSCREEN depends on CONFIG_INPUT"
#    undef CONFIG_SIM_TOUCHSCREEN
#  endif
#endif

#ifdef CONFIG_SIM_TOUCHSCREEN
#  ifndef CONFIG_SIM_EVLOOPPRIORITY
#    define CONFIG_SIM_EVLOOPPRIORITY 50
#  endif
#  ifndef CONFIG_SIM_EVLOOPSTACKSIZE
#    define CONFIG_SIM_EVLOOPSTACKSIZE 4096
#  endif
#else
#  undef CONFIG_SIM_EVLOOPPRIORITY
#  undef CONFIG_SIM_EVLOOPSTACKSIZE
#endif

/* Context Switching Definitions ******************************************/
/* Storage order: %ebx, $esi, %edi, %ebp, sp, and return PC */

#ifdef __ASSEMBLY__
#  define JB_EBX (0*4)
#  define JB_ESI (1*4)
#  define JB_EDI (2*4)
#  define JB_EBP (3*4)
#  define JB_SP  (4*4)
#  define JB_PC  (5*4)
#else
#  define JB_EBX (0)
#  define JB_ESI (1)
#  define JB_EDI (2)
#  define JB_EBP (3)
#  define JB_SP  (4)
#  define JB_PC  (5)
#endif /* __ASSEMBLY__ */

/* Simulated Heap Definitions **********************************************/
/* Size of the simulated heap */

#if CONFIG_MM_SMALL
#  define SIM_HEAP_SIZE (64*1024)
#else
#  define SIM_HEAP_SIZE (4*1024*1024)
#endif

/* File System Definitions **************************************************/
/* These definitions characterize the compressed filesystem image */

#define BLOCK_COUNT         1024
#define SECTOR_OF_BACKUPT   6
#define NUMBER_OF_FATS      2
#define FAT_SIZE            32
#define NUM_HIDDEN_SECTORS  0
#define VOLUME_NAME         "NuttXTestVol"
#define USE_WHOLE_DEVICE    1
#define ROOT_DIR_ENTRIES    512
#define RESERVED_SECTORS    32
#define SECTORS_PER_CLUSTER 4
#define LOGICAL_SECTOR_SIZE 512

/**************************************************************************
 * Public Types
 **************************************************************************/

/**************************************************************************
 * Public Variables
 **************************************************************************/

/**************************************************************************
 * Public Function Prototypes
 **************************************************************************/

#ifndef __ASSEMBLY__

/* up_setjmp.S ************************************************************/

extern int  up_setjmp(int *jb);
extern void up_longjmp(int *jb, int val) __attribute__ ((noreturn));

/* up_devconsole.c ********************************************************/

extern void up_devconsole(void);
extern void up_registerblockdevice(void);

/* up_deviceimage.c *******************************************************/

extern char *up_deviceimage(void);

/* up_stdio.c *************************************************************/

extern size_t up_hostread(void *buffer, size_t len);
extern size_t up_hostwrite(const void *buffer, size_t len);

/* up_netdev.c ************************************************************/

#ifdef CONFIG_NET
extern unsigned long up_getwalltime( void );
#endif

/* up_x11framebuffer.c ******************************************************/

#ifdef CONFIG_SIM_X11FB
extern int up_x11initialize(unsigned short width, unsigned short height,
                            void **fbmem, unsigned int *fblen, unsigned char *bpp,
                            unsigned short *stride);
#ifdef CONFIG_FB_CMAP
extern int up_x11cmap(unsigned short first, unsigned short len,
                      unsigned char *red, unsigned char *green,
                      unsigned char *blue, unsigned char  *transp);
#endif
#endif

/* up_eventloop.c ***********************************************************/

#ifdef CONFIG_SIM_X11FB
#ifdef CONFIG_SIM_TOUCHSCREEN
extern int up_x11eventloop(int argc, char *argv[]);
#endif
#endif

/* up_eventloop.c ***********************************************************/

#ifdef CONFIG_SIM_X11FB
#ifdef CONFIG_SIM_TOUCHSCREEN
extern int up_tcenter(int x, int y, int buttons);
extern int up_tcleave(int x, int y, int buttons);
#endif
#endif

/* up_tapdev.c ************************************************************/

#if defined(CONFIG_NET) && !defined(__CYGWIN__)
extern void tapdev_init(void);
extern unsigned int tapdev_read(unsigned char *buf, unsigned int buflen);
extern void tapdev_send(unsigned char *buf, unsigned int buflen);

#define netdev_init()           tapdev_init()
#define netdev_read(buf,buflen) tapdev_read(buf,buflen)
#define netdev_send(buf,buflen) tapdev_send(buf,buflen)
#endif

/* up_wpcap.c *************************************************************/

#if defined(CONFIG_NET) && defined(__CYGWIN__)
extern void wpcap_init(void);
extern unsigned int wpcap_read(unsigned char *buf, unsigned int buflen);
extern void wpcap_send(unsigned char *buf, unsigned int buflen);

#define netdev_init()           wpcap_init()
#define netdev_read(buf,buflen) wpcap_read(buf,buflen)
#define netdev_send(buf,buflen) wpcap_send(buf,buflen)
#endif

/* up_uipdriver.c *********************************************************/

#ifdef CONFIG_NET
extern int uipdriver_init(void);
extern int uipdriver_setmacaddr(unsigned char *macaddr);
extern void uipdriver_loop(void);
#endif

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_UP_INTERNAL_H */
