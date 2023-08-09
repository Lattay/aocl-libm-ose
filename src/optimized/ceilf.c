/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* Implementation notes
    The range is divided into
      > 2^24. ceil will either the number itself or Nan
            always returns a QNan. Raises exception if input is a SNan
      < 1.0   If 0.0 then return with the appropriate sign
            If input is less than -0.0 and greater than -1.0 then return -0.0
            If input is greater than 0.0 and less than 1.0 then return 1.0
      1.0 < |x| < 2^24
            appropriately check the exponent and set the return Value by shifting

      x is +zero or -zero; return the same zero
*/


#include "fn_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>

float ALM_PROTO_OPT(ceilf)(float x) {
    float r;
    int xneg;
    uint32_t ux, ax, ur, mask, rexp;

    ux = asuint32(x);

    /*ax is |x|*/
    ax = ux & (~SIGNBIT_SP32);
    /*xneg stores the sign of the input x*/
    xneg = (ux != ax);
    
    if (unlikely(ax >= TWOPOW24_F32)) /* abs(x) > 2^24*/ {
        /* abs(x) is either NaN, infinity, or >= 2^24 */
        if (unlikely(x != x)) { /* x is NaN */
            #ifdef WINDOWS
            return __alm_handle_errorf(ux | 0x00400000, 0);
            #else
            if (unlikely(!(ax & QNAN_MASK_32))) { //x is snan
                return __alm_handle_errorf(ux | QNAN_MASK_32, AMD_F_INVALID);
            }
			      return x;
            #endif /* #ifdef WINDOWS */
        }
        return x;
    }

    else if (ax < POS_ONE_F32) /* abs(x) < 1.0 */ {
        if (ax == POS_ZERO_F32) {
            return x;
        }
        else if (xneg) { /* x < 0.0 */
            return -0.0F;
        }
        return 1.0F;
    }

    else {
        rexp = ((ux & EXPBITS_SP32) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;
        /* Mask out the bits of r that we don't want */
        mask = (unsigned int)((1 << (EXPSHIFTBITS_SP32 - rexp)) - 1);
        /*Keeps the exponent part and the required mantissa.*/
        ur = (ux & ~mask);
        r = asfloat(ur);

        if (xneg || (ux == ur)) {
            return r;
        }
        /* We threw some bits away and x was positive */
        return r + 1.0F;
    }
}

