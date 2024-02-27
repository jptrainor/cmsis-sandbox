//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#include "CmsisTypeFactory.h"

#include "Source.h"

#include <algorithm>
#include <cmath>

CmsisTypeFactory::CmsisTypeFactory() {}

CmsisTypeFactory::CmsisTypeFactory(std::unique_ptr<Source> source)
  : source(std::move(source))
{}

const Source& CmsisTypeFactory::getSource() {
  return *source;
}
  
std::unique_ptr<std::vector<float64_t>> CmsisTypeFactory::toFloat64() {
  auto f64 = std::make_unique<std::vector<float64_t>>(source->size());
  source->reset();
  for(int i = 0; !source->isEnd(); i++) {
    f64->at(i) = source->next();
  }
  return f64;
}

std::unique_ptr<std::vector<float32_t>> CmsisTypeFactory::toFloat32() {
  auto f32 = std::make_unique<std::vector<float32_t>>(source->size(), 0);
  source->reset();
  for(int i = 0; !source->isEnd(); i++) {
    f32->at(i) = source->next();
  }
  return f32;
}

// See arm_fir_decimate_q31 and arm_fir_decimate_fast_q31
// documentation regarding scaling requirements.
std::unique_ptr<std::vector<q31_t>> CmsisTypeFactory::toQ31(unsigned int rshift) {
  auto q31 = std::make_unique<std::vector<q31_t>>(source->size());
  source->reset();
  for(int i = 0; !source->isEnd(); i++) {
    float s = source->next();
    q31_t v = clip_q63_to_q31((q63_t) (s * 2147483648.0f));
    q31->at(i) = v;
    // printf("toQ31 %f 0x%016llx 0x%08x q31=%ld\n", s, (q63_t)(s * 2147483648.0f), v, q31->at(i));
  }

  if (rshift > 0) {
    std::for_each(q31->begin(), q31->end(), [rshift](q31_t& x) {x = x >> rshift;});
  }

  return q31;
}

#if 0
// clip_q63_to_q15 appears to be broken
// TODO - investigate separately and report a bug if confirmed.
std::unique_ptr<std::vector<q15_t>> CmsisTypeFactory::toQ15() {
  auto q15 = std::make_unique<std::vector<q15_t>>(source->size());
  source->reset();
  for(int i = 0; !source->isEnd(); i++) {
    float32_t s = source->next();
    q15_t v = clip_q63_to_q15((q63_t) (s * 32768.0f));
    q15->at(i) = v;
    printf("toQ15 %f 0x%016llx 0x%04hx q15=%d\n", s, (q63_t)(s * 32768.0f), v, q15->at(i));
  }
  return q15;
}
#else
// The alternative is to allocate a float32_t array and use
// arm_float_to_q15 (which is not implemented with
// clip_q63_to_q15()).
//
// Optionally right shift by rshift bits to satisfy overflow
// limitations of functions such as
// arm_fir_decimate_fast_q15(). Only caller can know how the q15
// data is used hence only the caller can determine the need for
// right shift and the amount of right shift.
std::unique_ptr<std::vector<q15_t>> CmsisTypeFactory::toQ15(unsigned int rshift) {
  std::vector<float32_t> f32(source->size());

  source->reset();
  for(int i = 0; !source->isEnd(); i++) {
    f32.at(i) = source->next();
  }

  auto q15 = std::make_unique<std::vector<q15_t>>(source->size());
  arm_float_to_q15(f32.data(), q15->data(), f32.size());

  if (rshift > 0) {
    std::for_each(q15->begin(), q15->end(), [rshift](q15_t& x) {x = x >> rshift;});
  }
    
  return q15;
}
#endif
