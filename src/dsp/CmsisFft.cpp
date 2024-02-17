#include "CmsisFft.h"

#include "Ex.h"

#include <sstream>
#include <iomanip>
#include <cmath>

namespace {

  template <typename T> class CmsisFft : public FFT {

    CmsisFft();

  protected:

    // arm_rfft* inverse fft flag
    const uint8_t ifftFlag = 0;

    // arm_rfft* bit reverse flag
    const uint32_t bitReverseFlag = 1;

    // the data type name
    const std::string name;

    // the waveform length
    const unsigned int length;
  
    // the input vector, it will be modified by fft processing
    std::unique_ptr<std::vector<T>> waveform;

    CmsisFft(const char* name, std::unique_ptr<std::vector<T>> waveform)
      : name(name),
	waveform(std::move(waveform)),
	length(waveform->size())
    {}

    void checkArmInitStatus(arm_status status) {
      if (status == ARM_MATH_ARGUMENT_ERROR) {
	throw Ex("arm " + name + " fft length not supported");
      }
      else if (status != ARM_MATH_SUCCESS) {
	throw Ex("arm " + name + " fft init error");
      }
    }

    virtual std::string toString(const T& val) const = 0;

  public:

    virtual void deleteWaveform() {
      waveform.reset();
    }

    virtual unsigned int getLength() const {
      return length;
    }
  
    virtual const std::string& getName() const {
      return name;
    }
  };

  template <typename T> class RealFft : public CmsisFft<T> {

    RealFft();

  protected:

    enum FftOutputWidth { HALF=1, FULL=2 };

    // This is the ouput buffer passed to the arm_rfft* call.
    std::vector<T> fft;

    // This is the output array passed to the arm_mag* call.
    std::vector<T> mag;

    // The FFT output width, full or half.
    FftOutputWidth fftOutputWidth;

    // Allocate fft and magnitude output buffers. Set fftOutputWidth to
    // FULL to allcoate fft ouput buffer space for waveform->size()
    // complex pairs in the fft output buffer. Set fftOutputWidth to
    // HALF to allocate space for waveform->size() complex pairs in the
    // fft output buffer. The magnitude buffer is always half the size
    // of the fft output buffer.
    RealFft(const char* name, std::unique_ptr<std::vector<T>> waveform, FftOutputWidth fftOutputWidth)
      : CmsisFft<T>(name, std::move(waveform)),
	fft(this->waveform->size() * fftOutputWidth),
	mag(this->fft.size() / 2),
	fftOutputWidth(fftOutputWidth)
    {}

  public:

    virtual std::unique_ptr<std::vector<float>> getNormalizedWaveform() const {
      auto normalizedWaveform = std::make_unique<std::vector<float>>(this->waveform->size());
      int i = 0;
      for (auto x: *this->waveform) {
	normalizedWaveform->at(i++) = x;
      }
      return normalizedWaveform;
    }

    virtual void dump() const {
      for( int i = 0; i < mag.size(); i++ ) {
	printf("%s mag[%d] %s %s\n", this->name.c_str(), i, this->toString(mag[i]).c_str());
      }
      printf("\n");
    }

    // Return the output of arm_rfft_{f32,f64,q15,q31}. The values are
    // packed complex pairs.
    const std::vector<T>& getFFT() const {
      return fft;
    }

    // Get the output of arm_cmplx_mag_{f32,f64,q15,q31) computed over
    // the arm_rfft_{f32,f64,q15,q31} output (as provided by getFFT()).
    const std::vector<T>& getMagnitude() const {
      return mag;
    }

    // Get the FFT computation output widht (full or half frequency
    // band).
    FftOutputWidth getFftOutputWidth() const {
      return fftOutputWidth;
    }
  };

  template <typename T> class RealFloatFft : public RealFft<T> {

    RealFloatFft();

  protected:

    // The N/2 FFT component that was encoded in the
    // arm_rfft_fast_f{32,64} output buffer at index 1.
    T nyquistFrequencyComponent = 0;

  public:
  
    RealFloatFft(const char* name, std::unique_ptr<std::vector<T>> waveform)
      : RealFft<T>(name, std::move(waveform), RealFft<T>::FftOutputWidth::HALF)
    {}

    // Nothing to do for the floating point implementation. Just copy
    // the data and allow the compiler to do implicity double to float
    // if necessary.
    virtual std::unique_ptr<std::vector<float>> getNormalizedMagnitude() const {
      unsigned int len = 2*this->mag.size();
      auto scaledMag = std::make_unique<std::vector<float>>(len);

      // frequency range 0 <= n < N/2
      int n = 0;
      for (; n < len/2; n++) {
	scaledMag->at(n) = this->mag.at(n);
      }

      // nyquist frequency n = N/2
      scaledMag->at(n++) = nyquistFrequencyComponent;

      // symmetric frequency range N/2 < n <= N-1
      for (int j=this->mag.size()-1; j > 1; j--) {
	scaledMag->at(n++) = this->mag.at(j);
      }
    
      return scaledMag;
    }

    // The Nyquist frequency component value (the N/2 real value).
    T getNyquistFrequencyComponent() const {
      return nyquistFrequencyComponent;
    }
  };

  template <typename T> class RealFixedFft : public RealFft<T> {

    RealFixedFft();

  public:
  
    RealFixedFft(const char* name, std::unique_ptr<std::vector<T>> waveform)
      : RealFft<T>(name, std::move(waveform), RealFft<T>::FftOutputWidth::FULL)
    {}

    // The scale of fixed point q15_t and q31_t types is is 2^15 and
    // q31_t is 2^31. Which can be calculated as 2^(8*sizeof(T)-1).
    virtual std::unique_ptr<std::vector<float>> getNormalizedWaveform() const {
      auto normalizedWaveform = std::make_unique<std::vector<float>>(this->waveform->size());
      float scale = ::powf(2.0, 8*sizeof(T)-1);
      int i = 0;
      for (auto x: *this->waveform) {
	normalizedWaveform->at(i++) = x / scale;
      }
      return normalizedWaveform;
    }

    // Apply the scale factor necessary to convert the fixed point
    // output of arm_rfft_q{15,31} to a real valued magnitude that
    // matches the output of the arm_rfft_f{32,64}. See the
    // documentation for arm_cmplx_rfft_q{15,31} and
    // arm_cmplx_mag_q{15,31} to understand the scale factors.
    //
    // The scale factor varies by fft size. For example, for 1024 the
    // arm_cmplx_rfft_q{15,31} scale is documented to be 9 bits
    // (e.g. q1.15 in to q10.22 out). The arm_cmplx_mag_q{15,31) is
    // documented to be 1 bit (e.g. q1.15 in to q2.14). The magnitude
    // therefore would be q11.5 for the q15 result and q11.22 for the
    // q31 result.
    //
    // There is an additional one bit of scaling due to FFT
    // normalization that is a source of confusion (see
    // https://github.com/ARM-software/CMSIS_5/issues/220) but has to be
    // accounted for. Therefore the final fft magnitude scaling for the
    // q15 fft is q12.4, and the final scaling for the q31 result is
    // q12.20.
    //
    // This overall output scaling can be generalized as Qm.n fixed
    // point output where:
    //
    // m = 2 + log2(fftSize)
    // n = 8*sizeof(T) - m
    //
    // Therefore, the fixed point range as a real number is:
    //
    // scale = 2^n
    //
    // Such that:
    //
    // scaledMagnitude = fixedPointMagnitude / scale
  
    virtual std::unique_ptr<std::vector<float>> getNormalizedMagnitude() const {
      float m = 2.0 + log2(this->length);
      float n = 8.0*sizeof(T) - m;
      float scale = ::powf(2.0, n);

      auto scaledMag = std::make_unique<std::vector<float>>(this->mag.size());
      int i = 0;
      for(auto x: this->mag) {
	scaledMag->at(i++) = x / scale;
      }

      return scaledMag;
    }
  };

  class Float64Fft : public RealFloatFft<float64_t> {

    Float64Fft();
  
  public:
  
    Float64Fft(std::unique_ptr<std::vector<float64_t>> waveform)
      : RealFloatFft<float64_t>("f64", std::move(waveform))
    {}

    virtual void execute() {
      // sanity check the fft output buffer size
      if (fft.size() != waveform->size()) {
	throw Ex("f64 sanity");
      }
    
      arm_rfft_fast_instance_f64 inst;    

      checkArmInitStatus( arm_rfft_fast_init_f64(&inst, waveform->size()) );
    
      arm_rfft_fast_f64(&inst, waveform->data(), fft.data(), ifftFlag);
      nyquistFrequencyComponent = fft[1];
      fft[1] = 0.0;

      arm_cmplx_mag_f64(fft.data(), mag.data(), mag.size());
    }

    virtual std::string toString(const float64_t& val) const {
      std::stringstream ss;
      ss << val;
      return ss.str();
    }  
  };

  class Float32Fft : public RealFloatFft<float32_t> {

    Float32Fft();
  
  public:
  
    Float32Fft(std::unique_ptr<std::vector<float32_t>> waveform)
      : RealFloatFft<float32_t>("f32", std::move(waveform))
    {}
  
    virtual void execute() {
      // sanity check the fft output buffer size
      if (fft.size() != waveform->size()) {
	throw Ex("f32 sanity");
      }

      arm_rfft_fast_instance_f32 inst;    

      checkArmInitStatus( arm_rfft_fast_init_f32(&inst, waveform->size()) );

      arm_rfft_fast_f32(&inst, waveform->data(), fft.data(), ifftFlag);
      nyquistFrequencyComponent = fft[1];
      fft[1] = 0.0;

      arm_cmplx_mag_f32(fft.data(), mag.data(), mag.size());
    }

    virtual std::string toString(const float32_t& val) const {
      std::stringstream ss;
      ss << val;
      return ss.str();
    }
  };

  class Q31Fft : public RealFixedFft<q31_t> {

    Q31Fft();
  
  public:
  
    Q31Fft(std::unique_ptr<std::vector<q31_t>> waveform)
      : RealFixedFft<q31_t>("q31", std::move(waveform))
    {}
  
    virtual void execute() {
      // sanity check the fft output buffer size
      if (fft.size() != 2*waveform->size()) {
	throw Ex("q31 sanity");
      }

      arm_rfft_instance_q31 inst;

      checkArmInitStatus(arm_rfft_init_q31(&inst, waveform->size(), ifftFlag, bitReverseFlag));

      arm_rfft_q31(&inst, waveform->data(), fft.data());

      arm_cmplx_mag_q31(fft.data(), mag.data(), mag.size());
    }

    virtual std::string toString(const q31_t& val) const {
      std::stringstream ss;
      ss << "0x" << std::setfill('0') << std::setw(8) << std::hex << val;
      return ss.str();
    }

  };

  class Q15Fft : public RealFixedFft<q15_t> {

    Q15Fft();
  
  public:
  
    Q15Fft(std::unique_ptr<std::vector<q15_t>> waveform)
      : RealFixedFft<q15_t>("q15", std::move(waveform))
    {}
  
    virtual void execute() {
      // sanity check the fft output buffer size
      if (fft.size() != 2*waveform->size()) {
	throw Ex("q15 sanity");
      }

      arm_rfft_instance_q15 inst;

      checkArmInitStatus(arm_rfft_init_q15(&inst, waveform->size(), ifftFlag, bitReverseFlag));

      arm_rfft_q15(&inst, waveform->data(), fft.data());

      arm_cmplx_mag_q15(fft.data(), mag.data(), mag.size());
    }

    virtual std::string toString(const q15_t& val) const {
      std::stringstream ss;
      ss << "0x" << std::setfill('0') << std::setw(4) << std::hex << val;
      return ss.str();
    }
  };

} // namespace
  
std::unique_ptr<FFT> createFloat64Fft(std::unique_ptr<std::vector<float64_t>> waveform) {
  return std::unique_ptr<FFT>(new Float64Fft(std::move(waveform)));
}

std::unique_ptr<FFT> createFloat32Fft(std::unique_ptr<std::vector<float32_t>> waveform) {
  return std::unique_ptr<FFT>(new Float32Fft(std::move(waveform)));
}

std::unique_ptr<FFT> createQ31Fft(std::unique_ptr<std::vector<q31_t>> waveform) {
  return std::unique_ptr<FFT>(new Q31Fft(std::move(waveform)));
}

std::unique_ptr<FFT> createQ15Fft(std::unique_ptr<std::vector<q15_t>> waveform) {
  return std::unique_ptr<FFT>(new Q15Fft(std::move(waveform)));
}
