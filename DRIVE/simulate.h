#ifndef _SIMULATE_H_
#define _SIMULATE_H_
#include "hal_sys.h"

typedef enum{
	SITE_NTC = 0,
	SITE_VOLTAGE = 1,
}simulate_t;

void simulateInit(void);
u16 simulateRead(simulate_t site);
u16 simulateReadAverage(simulate_t site);
#endif
