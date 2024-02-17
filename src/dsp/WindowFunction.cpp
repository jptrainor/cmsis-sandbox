#include "WindowFunction.h"

#include <vector>
#include <cmath>

// The Hanning window function is (sin(pi*n/N))^2 where 0<=n<=N. Where
// N=numSamples=1
std::unique_ptr<WindowFunction> createHanningWindow(unsigned int numSamples) {

  auto window = std::make_unique<std::vector<float>>(numSamples);

  const unsigned int N = window->size()-1;
  for(int n = 0; n <= N; n++) {
    float x = std::sin(M_PI * n / N);
    window->at(n) = x*x;
  }

  return std::make_unique<WindowFunction>("Hanning", std::move(window));
}
