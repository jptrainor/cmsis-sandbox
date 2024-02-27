//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_CMSISTYPEFACTORY_INCLUDED
#define PICO_CMSIS_SANDBOX_CMSISTYPEFACTORY_INCLUDED

#include "arm_math.h"

#include <memory>
#include <vector>

class Source;

class CmsisTypeFactory {

  std::unique_ptr<Source> source;
  
  CmsisTypeFactory();

public:

  CmsisTypeFactory(std::unique_ptr<Source> source);

  const Source& getSource();

  std::unique_ptr<std::vector<float64_t>> toFloat64();

  std::unique_ptr<std::vector<float32_t>> toFloat32();

  // Convert source to q31_t fixed point with optional right shift
  // scaling. See arm_fir_decimate_q31 and arm_fir_decimate_fast_q31
  // documentation regarding scaling requirements. Scaling is done
  // after thre real to fixed point conversion.
  std::unique_ptr<std::vector<q31_t>> toQ31(unsigned int rshift = 0);

  // Convert source to q15_t fixed point with optional right shift
  // scaling. See arm_fir_decimate_fast_q15 documentation regarding
  // scaling requirements. Scaling is done after the real to fixed
  // point conversion.
  std::unique_ptr<std::vector<q15_t>> toQ15(unsigned int rshift = 0);
};

#endif
