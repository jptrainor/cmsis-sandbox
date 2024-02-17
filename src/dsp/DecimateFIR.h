#ifndef PICO_CMSIS_SANDBOX_DECIMATIONFIR_H_INCLUDED
#define PICO_CMSIS_SANDBOX_DECIMATIONFIR_H_INCLUDED

#include <memory>
#include <vector>

// Get decimation FIR for decimation factor M. M={2,4,8} are
// supported.
std::unique_ptr<std::vector<double>> getDecimationFIR(unsigned int M);

#endif
