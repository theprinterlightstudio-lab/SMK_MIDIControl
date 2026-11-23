/* SPDX-License-Identifier: GPL-3.0-only */
/* Dials and bars: the big knob and fader graphics on the performance page.
 *
 * Integers only, as the chip demands: angles are 1/1024 of a turn, 0 at 3 o'clock, growing clockwise
 * (the screen's y grows downward). Cosine comes from a quarter-wave table scaled by 4096, so every
 * place that would divide by it shifts by 12 instead.
 *
 * A knob's sweep is the usual 270 degrees: DIAL_A0 at 7-8 o'clock round to 4-5 o'clock. */
#pragma once
#include <stdint.h>

#include "fm1.h"

#define DIAL_ANG 1024 /* angle units in a turn */
#define DIAL_A0 384   /* 135 degrees: the bottom left, where a knob's travel starts */
#define DIAL_SPAN 768 /* 270 degrees of travel */

static const int16_t DIAL_COS[257] = {
    4096, 4096, 4096, 4095, 4095, 4094, 4093, 4092,
    4091, 4090, 4088, 4087, 4085, 4083, 4081, 4079,
    4076, 4074, 4071, 4068, 4065, 4062, 4059, 4055,
    4052, 4048, 4044, 4040, 4036, 4031, 4027, 4022,
    4017, 4012, 4007, 4002, 3996, 3991, 3985, 3979,
    3973, 3967, 3961, 3954, 3948, 3941, 3934, 3927,
    3920, 3912, 3905, 3897, 3889, 3881, 3873, 3865,
    3857, 3848, 3839, 3831, 3822, 3812, 3803, 3794,
    3784, 3775, 3765, 3755, 3745, 3734, 3724, 3713,
    3703, 3692, 3681, 3670, 3659, 3647, 3636, 3624,
    3612, 3600, 3588, 3576, 3564, 3551, 3539, 3526,
    3513, 3500, 3487, 3474, 3461, 3447, 3433, 3420,
    3406, 3392, 3378, 3363, 3349, 3334, 3320, 3305,
    3290, 3275, 3260, 3244, 3229, 3214, 3198, 3182,
    3166, 3150, 3134, 3118, 3102, 3085, 3068, 3052,
    3035, 3018, 3001, 2984, 2967, 2949, 2932, 2914,
    2896, 2878, 2861, 2843, 2824, 2806, 2788, 2769,
    2751, 2732, 2713, 2694, 2675, 2656, 2637, 2618,
    2598, 2579, 2559, 2540, 2520, 2500, 2480, 2460,
    2440, 2420, 2399, 2379, 2359, 2338, 2317, 2296,
    2276, 2255, 2234, 2213, 2191, 2170, 2149, 2127,
    2106, 2084, 2062, 2041, 2019, 1997, 1975, 1953,
    1931, 1909, 1886, 1864, 1842, 1819, 1797, 1774,
    1751, 1729, 1706, 1683, 1660, 1637, 1614, 1591,
    1567, 1544, 1521, 1498, 1474, 1451, 1427, 1404,
    1380, 1356, 1332, 1309, 1285, 1261, 1237, 1213,
    1189, 1165, 1141, 1117, 1092, 1068, 1044, 1020,
    995, 971, 946, 922, 897, 873, 848, 824,
    799, 774, 750, 725, 700, 675, 651, 626,
    601, 576, 551, 526, 501, 476, 451, 426,
    401, 376, 351, 326, 301, 276, 251, 226,
    201, 176, 151, 126, 101, 75, 50, 25,
    0};

static inline int dial_cos(int a) {
  a = ((a % DIAL_ANG) + DIAL_ANG) % DIAL_ANG;
  if (a <= 256) {
    return DIAL_COS[a];
  }
  if (a <= 512) {
    return -DIAL_COS[512 - a];
  }
  if (a <= 768) {
    return -DIAL_COS[a - 512];
  }
  return DIAL_COS[1024 - a];
}

static inline int dial_sin(int a) { return dial_cos(a - 256); }

/* value 0..127 -> where on the sweep it sits */
static inline int dial_angle(int value) {
  value = value < 0 ? 0 : value > 127 ? 127
                                      : value;
  return DIAL_A0 + value * DIAL_SPAN / 127;
}

/* an arc of the ring: radii r-thick+1..r, from a0 for span units. Stepping one angle unit at a time
 * oversamples even the biggest dial here (a 96 px radius is 603 px round), so there are no gaps. */
static inline void dial_arc(int cx, int cy, int r, int thick, int a0, int span, uint16_t color) {
  if (thick < 1) {
    thick = 1;
  }
  for (int i = 0; i <= span; i++) {
    int a = a0 + i;
    int c = dial_cos(a), s = dial_sin(a);
    for (int k = 0; k < thick; k++) {
      int rr = r - k;
      pixel(cx + ((rr * c) >> 12), cy + ((rr * s) >> 12), color);
    }
  }
}

/* the pointer: from inner out to outer, at the value's angle */
static inline void dial_pointer(int cx, int cy, int inner, int outer, int a, uint16_t color) {
  int c = dial_cos(a), s = dial_sin(a);
  line(cx + ((inner * c) >> 12), cy + ((inner * s) >> 12), cx + ((outer * c) >> 12), cy + ((outer * s) >> 12), color);
}
