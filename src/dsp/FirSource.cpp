#include "FirSource.h"

#include "Source.h"

#include "Ex.h"

namespace {

  class FirSource : public Source {

    std::unique_ptr<std::vector<double>> fir;

    unsigned int n = 0;

  public:

    FirSource(std::unique_ptr<std::vector<double>> fir)
      : fir(std::move(fir))
    {}

    virtual ~FirSource() {};

    virtual unsigned int size() const {
      return fir->size();
    }

    virtual bool isEnd() const {
      return n == fir->size();
    }

    virtual void reset() {
      n = 0;
    }

    virtual double next() {
      if (isEnd()) {
	throw new Ex("end of FIR");
      }

      return fir->at(n++);
    }
  };

} // namespace

std::unique_ptr<Source> createFirSource(std::unique_ptr<std::vector<double>> fir) {
  return std::make_unique<FirSource>(std::move(fir));
}
