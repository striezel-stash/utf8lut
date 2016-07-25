#include "Base/Timing.h"

TimingData timingData = TimingData();

inline void TimingPrintAll() {
	#define TIMING_X_PRINT(name, idx) \
		if (idx == TIMING_MAX) return; \
		printf("slot %d %10s : %6.3f cyc/el  %12" PRIu64 " elems\n", \
			idx, #name, \
			timingData.totalTime[idx] / (timingData.totalElems[idx] + 1e-9), \
			timingData.totalElems[idx] \
		);
	TIMING_SLOTS(TIMING_X_PRINT);
}
