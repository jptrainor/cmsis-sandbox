//  SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
//  SPDX-License-Identifier: Apache-2.0

#ifndef PICO_CMSIS_SANDBOX_MEMDEBUG_INCLUDED
#define PICO_CMSIS_SANDBOX_MEMDEBUG_INCLUDED

// Dump list of new'ed object sizes. Prints nothing if there are not
// allocations.
void memDebugReport(const char *detail);

#endif // PICO_CMSIS_SANDBOX_MEMDEBUG_INCLUDED
