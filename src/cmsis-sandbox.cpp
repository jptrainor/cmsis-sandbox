#include "Platform.h"
#include "DspMain.h"

int main()
{
  platform::init();

  return dsp_main();
}
