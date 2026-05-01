/*
 * Copyright (c) 2014-2016, Pelion and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RANDLIB_H_
#define RANDLIB_H_

#include "stdint.h"
#include <zephyr/random/random.h>

/**
 * This library is made for getting random numbers for Timing needs in protocols.
 *
 * **not safe to use for security or cryptographic operations.**
 *
 */

/**
 * \brief Init seed for Pseudo Random.
 *
 * Makes call(s) to the platform's arm_random_seed_get() to seed the
 * pseudo-random generator.
 *
 * \return None
 *
 */
static inline void randLIB_seed_random(void);

/**
 * \brief Update seed for pseudo-random generator
 *
 * Adds seed information to existing generator, to perturb the
 * sequence.
 * \param seed 64 bits of data to add to the seed.
 */
static inline void randLIB_add_seed(uint64_t seed);

/**
 * \brief Generate 8-bit random number.
 *
 * \param None
 * \return 8-bit random number
 *
 */
static inline uint8_t randLIB_get_8bit(void)
{
    return sys_rand8_get();
}

/**
 * \brief Generate 16-bit random number.
 *
 * \param None
 * \return 16-bit random number
 *
 */
static inline uint16_t randLIB_get_16bit(void)
{
    return sys_rand16_get();
}

/**
 * \brief Generate 32-bit random number.
 *
 * \param None
 * \return 32-bit random number
 *
 */
static inline uint32_t randLIB_get_32bit(void)
{
    return sys_rand32_get();
}

/**
 * \brief Generate 64-bit random number.
 *
 * \param None
 * \return 64-bit random number
 *
 */
static inline uint64_t randLIB_get_64bit(void)
{
    return sys_rand64_get();
}

/**
 * \brief Generate n-bytes random numbers.
 *
 * \param ptr pointer where random will be stored
 * \param count how many bytes need random
 *
 * \return data_ptr
 */
static inline void *randLIB_get_n_bytes_random(void *ptr, uint8_t count)
{
    uint8_t *data_ptr = ptr;
    uint64_t r = 0;
    for (uint_fast8_t i = 0; i < count; i++)
    {
        /* Take 8 bytes at a time */
        if (i % 8 == 0)
        {
            r = randLIB_get_64bit();
        }
        else
        {
            r >>= 8;
        }
        data_ptr[i] = (uint8_t)r;
    }
    return data_ptr;
}

/**
 * \brief Generate a random number within a range.
 *
 * The result is linearly distributed in the range [min..max], inclusive.
 *
 * \param min minimum value that can be generated
 * \param max maximum value that can be generated
 */
static inline uint16_t randLIB_get_random_in_range(uint16_t min, uint16_t max)
{
    /* This special case is potentially common, particularly in this routine's
     * first user (Trickle), so worth catching immediately */
    if (min == max)
    {
        return min;
    }

#if UINT_MAX >= 0xFFFFFFFF
    const unsigned int rand_max = 0xFFFFFFFFu; // will use rand32
#else
    const unsigned int rand_max = 0xFFFFu; // will use rand16

    /* 16-bit arithmetic below fails in this extreme case; we can optimise it */
    if (max - min == 0xFFFF)
    {
        return randLIB_get_16bit();
    }
#endif

    /* We get rand_max values from rand16 or 32() in the range [0..rand_max-1], and
     * need to divvy them up into the number of values we need. And reroll any
     * odd values off the end as we insist every value having equal chance.
     *
     * Using the range [0..rand_max-1] saves long division on the band
     * calculation - it means rand_max ends up always being rerolled.
     *
     * Eg, range(1,2), rand_max = 0xFFFF:
     * We have 2 bands of size 0x7FFF (0xFFFF/2).
     *
     * We roll: 0x0000..0x7FFE -> 1
     *          0x7FFF..0xFFFD -> 2
     *          0xFFFE..0xFFFF -> reroll
     * (calculating band size as 0x10000/2 would have avoided the reroll cases)
     *
     * Eg, range(1,3), rand_max = 0xFFFFFFFF:
     * We have 3 bands of size 0x55555555 (0xFFFFFFFF/3).
     *
     * We roll: 0x00000000..0x555555554 -> 1
     *          0x55555555..0xAAAAAAAA9 -> 2
     *          0xAAAAAAAA..0xFFFFFFFFE -> 3
     *          0xFFFFFFFF              -> reroll
     *
     * (Bias problem clearly pretty insignificant there, but gets worse as
     * range increases).
     */
    const unsigned int values_needed = max + 1 - min;
    /* Avoid the need for long division, at the expense of fractionally
     * increasing reroll chance. */
    const unsigned int band_size = rand_max / values_needed;
    const unsigned int top_of_bands = band_size * values_needed;
    unsigned int result;
    do
    {
#if UINT_MAX > 0xFFFF
        result = randLIB_get_32bit();
#else
        result = randLIB_get_16bit();
#endif
    } while (result >= top_of_bands);

    return min + (uint16_t)(result / band_size);
}

/**
 * \brief Randomise a base 32-bit number by a jitter factor
 *
 * The result is linearly distributed in the jitter range, which is expressed
 * as fixed-point unsigned 1.15 values. For example, to produce a number in the
 * range [0.75 * base, 1.25 * base], set min_factor to 0x6000 and max_factor to
 * 0xA000.
 *
 * Result is clamped to 0xFFFFFFFF if it overflows.
 *
 * \param base The base 32-bit value
 * \param min_factor The minimum value for the random factor
 * \param max_factor The maximum value for the random factor
 */
static inline uint32_t randLIB_randomise_base(uint32_t base, uint16_t min_factor, uint16_t max_factor)
{
    uint16_t random_factor = randLIB_get_random_in_range(min_factor, max_factor);

    /* 32x16-bit long multiplication, to get 48-bit result */
    uint32_t hi = (base >> 16) * random_factor;
    uint32_t lo = (base & 0xFFFF) * random_factor;
    /* Add halves, and take top 32 bits of 48-bit result */
    uint32_t res = hi + (lo >> 16);

    /* Randomisation factor is *2^15, so need to shift up 1 more bit, avoiding overflow */
    if (res & 0x80000000)
    {
        res = 0xFFFFFFFF;
    }
    else
    {
        res = (res << 1) | ((lo >> 15) & 1);
    }

    return res;
}

#ifdef RANDLIB_PRNG
/* \internal Reset the PRNG state to zero (invalid) */
void randLIB_reset(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* RANDLIB_H_ */