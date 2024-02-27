//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_SOURCE_INCLUDED
#define PICO_CMSIS_SANDBOX_SOURCE_INCLUDED

class Source {
 public:

  virtual ~Source() {};

  virtual unsigned int size() const = 0;

  virtual bool isEnd() const = 0;

  virtual void reset() = 0;

  virtual double next() = 0;
};

#endif
