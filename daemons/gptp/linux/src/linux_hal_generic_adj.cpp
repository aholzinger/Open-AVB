/******************************************************************************

  Copyright (c) 2009-2012, Intel Corporation 
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions are met:
  
   1. Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
  
   2. Redistributions in binary form must reproduce the above copyright 
      notice, this list of conditions and the following disclaimer in the 
      documentation and/or other materials provided with the distribution.
  
   3. Neither the name of the Intel Corporation nor the names of its 
      contributors may be used to endorse or promote products derived from 
      this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

#include <linux/timex.h>
#include <syscall.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

// we would like to include sys/types.h here for clockid_t, but this
// also includes sys/time.h, which leads to a type redefinition problem
// due to a conflict between linux/timex.h and sys/time.h. The workaround
// we employ here uses the type __clockid_t defined in bits/types.h, which
// ought to be the same as clockid_t (if it isn't, the linker will
// complain about the two missing functions)
#include <bits/types.h>

// resetting frequency adjustment works by passing 0.0 as the offset.
bool syscallAdjFrequency(__clockid_t clockid, float frequencyOffset) {
	struct timex tx;
	tx.modes = ADJ_FREQUENCY;
	tx.freq = long(frequencyOffset) << 16;
	tx.freq += long(fmodf(frequencyOffset, 1.0)*65536.0);

	return syscall(__NR_clock_adjtime, clockid, tx) == 0;
}

bool syscallSetOffset(__clockid_t clockid, int64_t phase_adjust) {
	struct timex tx = { 0 };
	tx.modes = ADJ_SETOFFSET | ADJ_NANO;
	if (phase_adjust >= 0) {
		tx.time.tv_sec = phase_adjust / 1000000000LL;
		tx.time.tv_usec = phase_adjust % 1000000000LL;
	}
	else {
		tx.time.tv_sec = (phase_adjust / 1000000000LL) - 1;
		tx.time.tv_usec = (phase_adjust % 1000000000LL) + 1000000000;
	}


	return syscall(__NR_clock_adjtime, clockid, tx) == 0;
}
