#ifndef PICO_CMSIS_SANDBOX_DECIMATEST_H_INCLUDED
#define PICO_CMSIS_SANDBOX_DECIMATEST_H_INCLUDED

#include <memory>
#include <string>

class Decimate;

struct DecimateTestResult {
  const std::string name;
  const unsigned long elapsedTime;
  
  DecimateTestResult(const std::string& name, unsigned long elapsedTime)
    :name(name),
     elapsedTime(elapsedTime)
  {}
};

DecimateTestResult executeDecimateTest(unsigned int k, std::unique_ptr<Decimate> decimator);

#endif
