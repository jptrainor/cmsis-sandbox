//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#include "Platform.h"
#include "DspMain.h"

int main()
{
  platform::init();

  return dsp_main();
}
