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


#include "precision.h"


#if defined(FLOAT)
#define FUNC_FMA alm_mp_fmaf

#elif defined(DOUBLE)
#define FUNC_FMA alm_mp_fma

#else
#error
#endif

#include <mpfr.h>

REAL_L FUNC_FMA(REAL x, REAL y, REAL z)
{
    REAL_L r;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy, mpz, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mpy, mpz, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
    mpfr_set_d(mpy, y, rnd);
    mpfr_set_d(mpz, z, rnd);
#elif defined(DOUBLE)
    mpfr_set_ld(mpx, x, rnd);
    mpfr_set_ld(mpy, y, rnd);
    mpfr_set_ld(mpz, z, rnd);
#endif

    mpfr_fma(mp_rop, mpx, mpy, mpz, rnd);

#if defined(FLOAT)
    r = mpfr_get_d(mp_rop, rnd);
#elif defined(DOUBLE)
    r = mpfr_get_ld(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mpy, mpz, mp_rop, (mpfr_ptr) 0);
    return r;
}


