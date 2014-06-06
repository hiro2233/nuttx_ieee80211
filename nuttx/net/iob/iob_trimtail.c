/****************************************************************************
 * net/iob/iob_trimtail.c
 *
 *   Copyright (C) 2014 Gregory Nutt. All rights reserved.
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <string.h>
#include <queue.h>

#include <nuttx/net/iob.h>

#include "iob.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: iob_trimtail
 *
 * Description:
 *   Remove bytes from the end of an I/O chain
 *
 ****************************************************************************/

FAR struct iob_s *iob_trimtail(FAR struct iob_s *iob, unsigned int trimlen)
{
  FAR struct iob_s *entry;
  FAR struct iob_s *penultimate;
  FAR struct iob_s *last;
  unsigned int iosize;
  int len;

  if (iob && trimlen > 0)
    {
      len = trimlen;

      /* Loop until complete the trim */

      while (len > 0)
        {
          /* Calculate the total length of the data in the I/O buffer
           * chain and find the last entry in the chain.
           */

          penultimate = NULL;
          last = NULL;
          iosize = 0;

          for (entry = iob;
               entry;
               entry = (FAR struct iob_s *)entry->io_link.flink)
            {
              /* Accumulate the total size of all buffers in the list */

              iosize += entry->io_len;

              /* Remember the last and the next to the last in the chain */

              penultimate = last;
              last = entry;
            }

          /* Trim from the last entry in the chain.  Do we trim this entire
           * I/O buffer away?
           */

          if (last->io_len <= len)
            {
              /* Yes.. Consume the entire buffer */

              iob->io_pktlen -= last->io_len;
              len            -= last->io_len;
              last->io_len    = 0;

              /* Free the last, empty buffer in the list */

              iob_free(last);

              /* There should be a buffer before this one */

              if (!penultimate)
                {
                  /* No.. we just freed the head of the chain */

                  return NULL;
                }

              /* Unlink the penultimate from the freed buffer */

              penultimate->io_link.flink = NULL;
            }
               
          else
            {
              /* No, then just take what we need from this I/O buffer and
               * stop the trim.
               */

              iob->io_pktlen -= len;
              last->io_len   -= len;
              len             = 0;
            }
        }
    }

  return iob;
}
