//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#include "CmsisDecimate.h"
#include "Ex.h"

namespace {

  template <typename T> class CmsisDecimate : public Decimate {

  protected:
    // the data type name
    const std::string name;

    // the filter
    std::unique_ptr<std::vector<T>> fir;
  
    // the input vector, it will be modified by fft processing
    std::unique_ptr<std::vector<T>> waveform;

    // the decimation factor
    const unsigned int M;

    // the decimated output vector
    std::vector<T> result;
  
    void checkArmInitStatus(arm_status status) {
      if (status == ARM_MATH_LENGTH_ERROR ) {
	throw Ex("arm " + name + " decimation blockSize is not a multple of M");
      }
      else if (status != ARM_MATH_SUCCESS) {
	throw Ex("arm " + name + " decimatin init error");
      }
    }
  
  public:

    CmsisDecimate(const char* name, std::unique_ptr<std::vector<T>> fir, std::unique_ptr<std::vector<T>> waveform, unsigned int M)
      : name(name),
	fir(std::move(fir)),
	waveform(std::move(waveform)),
	M(M),
	result(this->waveform->size()/M)
    {
      // This is a restriction of this test, not of arm_fir_decimate_*.
      if ( M*result.size() != this->waveform->size() ) {
	throw Ex("waveform size is not a multiple of decimation factor");
      }
    }
  
    virtual ~CmsisDecimate() {}

    virtual const std::string& getName() const {
      return name;
    }

    virtual unsigned int getM() {
      return M;
    }

    virtual unsigned int getNumTaps() {
      return fir->size();
    }

    const std::unique_ptr<std::vector<float>> getResult() const {
      auto floatResult = std::make_unique<std::vector<float>>(result.size());
      std::copy(result.cbegin(), result.cend(), floatResult->begin());
      return floatResult;
    }
  };

  class Float32Decimate: public CmsisDecimate<float32_t> {

  public:

    Float32Decimate(std::unique_ptr<std::vector<float32_t>> fir, std::unique_ptr<std::vector<float32_t>> waveform, unsigned int M)
      : CmsisDecimate<float32_t>("f32", std::move(fir), std::move(waveform), M)
    {}
  
    virtual ~Float32Decimate() {}

    virtual void execute() {

      uint16_t numTaps =  fir->size();
      uint32_t blockSize= waveform->size();
      std::vector<float32_t> state(numTaps+blockSize-1);

      arm_fir_decimate_instance_f32 init;
      checkArmInitStatus( arm_fir_decimate_init_f32(&init, numTaps, M, fir->data(), state.data(), blockSize) );

      arm_fir_decimate_f32(&init, waveform->data(), result.data(), waveform->size());
    }
  };

  class Q15Decimate: public CmsisDecimate<q15_t> {

    bool fast;
  
  public:

    Q15Decimate(std::unique_ptr<std::vector<q15_t>> fir, std::unique_ptr<std::vector<q15_t>> waveform, unsigned int M, bool fast)
      : CmsisDecimate<q15_t>(fast ? "q15_fast" : "q15", std::move(fir), std::move(waveform), M),
	fast(fast)
    {}
  
    virtual ~Q15Decimate() {}

    void decimate_q15() {
      uint16_t numTaps =  fir->size();
      uint32_t blockSize= waveform->size();
      std::vector<q15_t> state(numTaps+blockSize-1);
      arm_fir_decimate_instance_q15 init;

      checkArmInitStatus( arm_fir_decimate_init_q15(&init, numTaps, M, fir->data(), state.data(), blockSize) );

      arm_fir_decimate_q15(&init, waveform->data(), result.data(), waveform->size());
    }

    void decimate_fast_q15() {
      uint16_t numTaps =  fir->size();
      uint32_t blockSize= waveform->size();
      std::vector<q15_t> state(numTaps+blockSize-1);
      arm_fir_decimate_instance_q15 init;

      checkArmInitStatus( arm_fir_decimate_init_q15(&init, numTaps, M, fir->data(), state.data(), blockSize) );

      arm_fir_decimate_fast_q15(&init, waveform->data(), result.data(), waveform->size());
    }
  
    virtual void execute() {
      if (fast) {
	decimate_fast_q15();
      }
      else {
	decimate_q15();
      }
    }
  };

  class Q31Decimate: public CmsisDecimate<q31_t> {

    bool fast;
  
  public:

    Q31Decimate(std::unique_ptr<std::vector<q31_t>> fir, std::unique_ptr<std::vector<q31_t>> waveform, unsigned int M, bool fast)
      : CmsisDecimate<q31_t>(fast ? "q31_fast" : "q31", std::move(fir), std::move(waveform), M),
	fast(fast)
    {}
  
    virtual ~Q31Decimate() {}

    void decimate_q31() {
      uint16_t numTaps =  fir->size();
      uint32_t blockSize= waveform->size();
      std::vector<q31_t> state(numTaps+blockSize-1);
      arm_fir_decimate_instance_q31 init;

      checkArmInitStatus( arm_fir_decimate_init_q31(&init, numTaps, M, fir->data(), state.data(), blockSize) );

      arm_fir_decimate_q31(&init, waveform->data(), result.data(), waveform->size());
    }

    void decimate_fast_q31() {
      uint16_t numTaps =  fir->size();
      uint32_t blockSize= waveform->size();
      std::vector<q31_t> state(numTaps+blockSize-1);
      arm_fir_decimate_instance_q31 init;

      checkArmInitStatus( arm_fir_decimate_init_q31(&init, numTaps, M, fir->data(), state.data(), blockSize) );

      arm_fir_decimate_fast_q31(&init, waveform->data(), result.data(), waveform->size());
    }
  
    virtual void execute() {
      if (fast) {
	decimate_fast_q31();
      }
      else {
	decimate_q31();
      }
    }
  };

} // namespace

std::unique_ptr<Decimate> createFloat32Decimate(std::unique_ptr<std::vector<float32_t>> fir, std::unique_ptr<std::vector<float32_t>> waveform, unsigned int M) {
  return std::unique_ptr<Decimate>(new Float32Decimate(std::move(fir), std::move(waveform), M));
}

std::unique_ptr<Decimate> createQ15Decimate(std::unique_ptr<std::vector<q15_t>> fir, std::unique_ptr<std::vector<q15_t>> waveform, unsigned int M, bool fast) {
  return std::unique_ptr<Decimate>(new Q15Decimate(std::move(fir), std::move(waveform), M, fast));
}

std::unique_ptr<Decimate> createQ31Decimate(std::unique_ptr<std::vector<q31_t>> fir, std::unique_ptr<std::vector<q31_t>> waveform, unsigned int M, bool fast) {
  return std::unique_ptr<Decimate>(new Q31Decimate(std::move(fir), std::move(waveform), M, fast));
}
