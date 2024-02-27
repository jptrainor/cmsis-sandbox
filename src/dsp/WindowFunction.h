//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_HANNINGWINDOW_H_INCLUDED
#define PICO_CMSIS_SANDBOX_HANNINGWINDOW_H_INCLUDED

#include <vector>
#include <memory>
#include <string>

class WindowFunction {
  std::string name;
  std::unique_ptr<std::vector<float>> window;

 public:
  WindowFunction(const char* name, std::unique_ptr<std::vector<float>> window)
    : name(name),
      window(std::move(window))
  {}

  ~WindowFunction() {}

  const std::string& getName() const {
    return name;
  }
  
  const std::vector<float>& getWindow() const {
    return *window;
  }

  float at(unsigned int i) const {
    return window->at(i);
  }
};


/**
   The Hanning window function is (sin(pi*n/N))^2 where 0<=n<=N.

   Note that this results in N+1 discrete points. Therefore, don't
   confuse N with the sample count. Given a sample count of
   numSamples, N=numbSamples=1.
*/
std::unique_ptr<WindowFunction> createHanningWindow(unsigned int numSamples);

#endif
