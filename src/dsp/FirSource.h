#ifndef PICO_CMSIS_SANDBOX_FIRSOURCE_H_INCLUDED
#define PICO_CMSIS_SANDBOX_FIRSOURCE_H_INCLUDED

#include "Source.h"

#include <memory>
#include <vector>

std::unique_ptr<Source> createFirSource(std::unique_ptr<std::vector<double>> fir);

#endif
