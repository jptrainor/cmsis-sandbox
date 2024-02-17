#ifndef PICO_CMSIS_SANDBOX_MEMDEBUG_INCLUDED
#define PICO_CMSIS_SANDBOX_MEMDEBUG_INCLUDED

// Dump list of new'ed object sizes. Prints nothing if there are not
// allocations.
void memDebugReport(const char *detail);

#endif // PICO_CMSIS_SANDBOX_MEMDEBUG_INCLUDED
