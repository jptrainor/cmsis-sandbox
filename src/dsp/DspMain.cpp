#include "DspMain.h"

#include "MemDebug.h"
#include "FftTestRunner.h"
#include "DecimateTestRunner.h"
#include "Report.h"
#include "Ex.h"

#include <iostream>
#include <exception>

int dsp_main() {
  int rc = 0;
  
  try {
    std::unique_ptr<fft::NameToElapsedTimeMap> fftResultMap = runAllFftTests();
    std::unique_ptr<decimate::NameToFactorElapsedTimeMap> decimateResultMap = runAllDecimateTests();

    reportFftResults(*fftResultMap);
    reportDecimateResults(*decimateResultMap);

    std::cout << std::endl << "SUCCESS" << std::endl;
  }
  catch ( Fail& ex ) {
    std::cout << std::endl << "FAIL: " << ex.what() << std::endl;
    rc = 1;
  }
  catch (std::exception& ex) {
    std::cout << std::endl << "error: " << ex.what() << std::endl;
    rc = 1;
  }
  
  memDebugReport("allocated memory at exit:");

  return rc;
}

