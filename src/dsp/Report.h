#ifndef PICO_CMSIS_SANDBOX_REPORT_H_INCLUDED
#define PICO_CMSIS_SANDBOX_REPORT_H_INCLUDED

#include "FftTestRunner.h"
#include "DecimateTestRunner.h"

void reportFftResults(const fft::NameToElapsedTimeMap& fftResultMap);
void reportDecimateResults(const decimate::NameToFactorElapsedTimeMap& decimateResultMap);

#endif
