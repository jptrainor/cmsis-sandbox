//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#include <map>
#include <string>

namespace {

  // New'ed object size mapped by pointer value
  std::map<void *, unsigned int> mallocRecord;

  // Recursion flag
  bool inProgress = false;

  // Exception safe inProgress flag set/clear guard
  struct InProgressGuard {
    InProgressGuard() {
      inProgress = true;
    }
    ~InProgressGuard() {
      inProgress = false;
    }
  };
  
  void memDump(const char* detail) {
    if (mallocRecord.size() > 0) {
      printf("%s\n", detail);
    }
    for(auto m : mallocRecord) {
      printf("%p %d\n", m.first, m.second);
    }
  }

} // namespace

void memDebugReport(const char *detail) {
  InProgressGuard guard;
  memDump(detail);
}

void* operator new(std::size_t n)
{
  void* p=malloc(n);

  if (!inProgress) {
    InProgressGuard guard;
  }
  
  return p;
}

void operator delete(void * p)
{
  if (!inProgress) {
    InProgressGuard guard;
    mallocRecord.erase(p);
  }

  free(p);
}
