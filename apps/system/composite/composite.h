/****************************************************************************
 * system/composite/composite.h
 *
 *   Copyright (C) 2012-2013 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
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
 ****************************************************************************/

#ifndef __SYSTEM_COMPOSITE_COMPOSITE_H
#define __SYSTEM_COMPOSITE_COMPOSITE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdlib.h>

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/
/* Configuration ************************************************************/
/* OS/Driver configuration checkes */

#ifndef CONFIG_USBDEV
#  error "USB device support is not enabled (CONFIG_USBDEV)"
#endif

#ifndef CONFIG_USBDEV_COMPOSITE
#  error "USB composite device support is not enabled (CONFIG_USBDEV_COMPOSITE)"
#endif

#ifndef CONFIG_CDCACM
#  error "USB CDC/ACM serial device support is not enabled (CONFIG_CDCACM)"
#endif

#ifndef CONFIG_CDCACM_COMPOSITE
#  error "USB CDC/ACM serial composite device support is not enabled (CONFIG_CDCACM_COMPOSITE)"
#endif

#ifndef CONFIG_USBMSC
#  error "USB mass storage device support is not enabled (CONFIG_USBMSC)"
#endif

#ifndef CONFIG_USBMSC_COMPOSITE
#  error "USB mass storage composite device support is not enabled (CONFIG_USBMSC_COMPOSITE)"
#endif

/* Add-on Mass Storagte Class default values */

#ifndef CONFIG_SYSTEM_COMPOSITE_NLUNS
#  define CONFIG_SYSTEM_COMPOSITE_NLUNS 1
#endif

#ifndef CONFIG_SYSTEM_COMPOSITE_DEVMINOR1
#  define CONFIG_SYSTEM_COMPOSITE_DEVMINOR1 0
#endif

#ifndef CONFIG_SYSTEM_COMPOSITE_DEVPATH1
#  define CONFIG_SYSTEM_COMPOSITE_DEVPATH1 "/dev/mmcsd0"
#endif

#if CONFIG_SYSTEM_COMPOSITE_NLUNS > 1
#  ifndef CONFIG_SYSTEM_COMPOSITE_DEVMINOR2
#    error "CONFIG_SYSTEM_COMPOSITE_DEVMINOR2 for LUN=2"
#  endif
#  ifndef CONFIG_SYSTEM_COMPOSITE_DEVPATH2
#    error "CONFIG_SYSTEM_COMPOSITE_DEVPATH2 for LUN=2"
#  endif
#  if CONFIG_SYSTEM_COMPOSITE_NLUNS > 2
#    ifndef CONFIG_SYSTEM_COMPOSITE_DEVMINOR3
#      error "CONFIG_SYSTEM_COMPOSITE_DEVMINOR2 for LUN=3"
#    endif
#    ifndef CONFIG_SYSTEM_COMPOSITE_DEVPATH2
#      error "CONFIG_SYSTEM_COMPOSITE_DEVPATH2 for LUN=3"
#    endif
#    if CONFIG_SYSTEM_COMPOSITE_NLUNS > 3
#      error "CONFIG_SYSTEM_COMPOSITE_NLUNS must be {1,2,3}"
#    endif
#  endif
#endif

/* Add-on CDC/ACM default values */

#ifndef CONFIG_SYSTEM_COMPOSITE_TTYUSB
#  define CONFIG_SYSTEM_COMPOSITE_TTYUSB 0
#endif

#ifndef CONFIG_SYSTEM_COMPOSITE_SERDEV
#  if CONFIG_SYSTEM_COMPOSITE_TTYUSB != 0
#    error "Serial device unknown (CONFIG_SYSTEM_COMPOSITE_SERDEV)"
#  elif defined(CONFIG_CDCACM)
#    define CONFIG_SYSTEM_COMPOSITE_SERDEV "/dev/ttyACM0"
#  else
#    define CONFIG_SYSTEM_COMPOSITE_SERDEV "/dev/ttyUSB0"
#  endif
#endif

#ifndef CONFIG_SYSTEM_COMPOSITE_BUFSIZE
#  define CONFIG_SYSTEM_COMPOSITE_BUFSIZE 256
#endif

/* Trace initialization *****************************************************/

#ifndef CONFIG_USBDEV_TRACE_INITIALIDSET
#  define CONFIG_USBDEV_TRACE_INITIALIDSET 0
#endif

#ifdef CONFIG_SYSTEM_COMPOSITE_TRACEINIT
#  define TRACE_INIT_BITS       (TRACE_INIT_BIT)
#else
#  define TRACE_INIT_BITS       (0)
#endif

#define TRACE_ERROR_BITS        (TRACE_DEVERROR_BIT|TRACE_CLSERROR_BIT)

#ifdef CONFIG_SYSTEM_COMPOSITE_TRACECLASS
#  define TRACE_CLASS_BITS      (TRACE_CLASS_BIT|TRACE_CLASSAPI_BIT|TRACE_CLASSSTATE_BIT)
#else
#  define TRACE_CLASS_BITS      (0)
#endif

#ifdef CONFIG_SYSTEM_COMPOSITE_TRACETRANSFERS
#  define TRACE_TRANSFER_BITS   (TRACE_OUTREQQUEUED_BIT|TRACE_INREQQUEUED_BIT|TRACE_READ_BIT|\
                                 TRACE_WRITE_BIT|TRACE_COMPLETE_BIT)
#else
#  define TRACE_TRANSFER_BITS   (0)
#endif

#ifdef CONFIG_SYSTEM_COMPOSITE_TRACECONTROLLER
#  define TRACE_CONTROLLER_BITS (TRACE_EP_BIT|TRACE_DEV_BIT)
#else
#  define TRACE_CONTROLLER_BITS (0)
#endif

#ifdef CONFIG_SYSTEM_COMPOSITE_TRACEINTERRUPTS
#  define TRACE_INTERRUPT_BITS  (TRACE_INTENTRY_BIT|TRACE_INTDECODE_BIT|TRACE_INTEXIT_BIT)
#else
#  define TRACE_INTERRUPT_BITS  (0)
#endif

#define TRACE_BITSET            (TRACE_INIT_BITS|TRACE_ERROR_BITS|TRACE_CLASS_BITS|\
                                 TRACE_TRANSFER_BITS|TRACE_CONTROLLER_BITS|TRACE_INTERRUPT_BITS)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* All global variables used by this add-on are packed into a structure in
 * order to avoid name collisions.
 */

struct composite_state_s
{
  /* This is the handle that references to this particular USB composite
   * driver instance.  It is only needed if the add-on is built using
   * CONFIG_NSH_BUILTIN_APPS.  In this case, the value of the driver handle
   * must be remembered between the 'conn' and 'disconn' commands.
   */

  FAR void *cmphandle;        /* Composite device handle */
  FAR void *mschandle;        /* Mass storage device handle */

  /* Serial file descriptors */

#if !defined(CONFIG_NSH_BUILTIN_APPS) || defined(CONFIG_DISABLE_SIGNALS)
  int outfd;                  /* Blocking write-only */
  int infd;                   /* Non-blockig read-only */
#endif

  /* Heap usage samples.  These are useful for checking USB storage memory
   * usage and for tracking down memoryh leaks.
   */

#ifdef CONFIG_SYSTEM_COMPOSITE_DEBUGMM
  struct mallinfo mmstart;    /* Memory usage before the connection */
  struct mallinfo mmprevious; /* The last memory usage sample */
  struct mallinfo mmcurrent;  /* The current memory usage sample */
#endif

  /* Serial I/O buffer */

#if !defined(CONFIG_NSH_BUILTIN_APPS) || defined(CONFIG_DISABLE_SIGNALS)
  uint8_t serbuf[CONFIG_SYSTEM_COMPOSITE_BUFSIZE];
#endif
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* All global variables used by this add-on are packed into a structure in
 * order to avoid name collisions.
 */

extern struct composite_state_s g_composite;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#endif /* __SYSTEM_COMPOSITE_COMPOSITE_H */
