#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* choose your poison */

#define MSP (0)
#define PD (!MSP)
/* for compiling under XP */


#ifndef PIOVERTWO
#define PIOVERTWO 1.5707963268
#define TWOPI 6.2831853072
#endif

#if MSP
#include "ext.h"
#include "z_dsp.h"
#include "buffer.h"
#include "ext_obex.h"
#define t_floatarg double
#endif

#if PD
#include "m_pd.h"
#define t_floatarg float
#endif

/* because Max and Pd have different ideas of what A_FLOAT is, use t_floatarg
to force consistency. Otherwise functions that look good will fail on some
hardware. Also note that Pd messages cannot accept arguments of type A_LONG. */





