//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_CMSISDECIMATE_H_INCLUDED
#define PICO_CMSIS_SANDBOX_CMSISDECIMATE_H_INCLUDED

#include "arm_math.h"

#include <string>
#include <memory>
#include <vector>

class Decimate {
 public:

  virtual ~Decimate() {}

  virtual void execute() = 0;

  // the name of the FFT implementation
  virtual const std::string& getName() const = 0;

  // get the decimation factor
  virtual unsigned int getM() = 0;

  virtual const std::unique_ptr<std::vector<float>> getResult() const = 0;
};


// Create floating point decimator.  Implememented using arm_fir_decimate_f32.
std::unique_ptr<Decimate> createFloat32Decimate(std::unique_ptr<std::vector<float32_t>> fir, std::unique_ptr<std::vector<float32_t>> waveform, unsigned int M);

// Create a q15 fixed point decimator. Implemented using
// arm_fir_decimate_q15 and arm_fir_decimate_fast_q15. It's important
// to review the documentationf or the Arm decimation functions to
// understant scaling requirments.
std::unique_ptr<Decimate> createQ15Decimate(std::unique_ptr<std::vector<q15_t>> fir, std::unique_ptr<std::vector<q15_t>> waveform, unsigned int M, bool fast);

// Create a q31 fixed point decimator. Implemented using
// arm_fir_decimate_q31 and arm_fir_decimate_fast_q31. It's important
// to review the documentationf or the Arm decimation functions to
// understant scaling requirments.
std::unique_ptr<Decimate> createQ31Decimate(std::unique_ptr<std::vector<q31_t>> fir, std::unique_ptr<std::vector<q31_t>> waveform, unsigned int M, bool fast);

#endif

