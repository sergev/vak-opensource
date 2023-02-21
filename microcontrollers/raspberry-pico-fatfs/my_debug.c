/* my_debug.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/
#include "my_debug.h"

#include <stdarg.h>
#include <stdio.h>

void my_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
}

void my_assert_func(const char *file, int line, const char *func, const char *pred)
{
    printf("assertion \"%s\" failed: file \"%s\", line %d, function: %s\n", pred, file, line, func);
    fflush(stdout);

    /* Disable global interrupts. */
    __asm volatile("cpsid i" : : : "memory");

    while (1) {
        // Stop in GUI as if at a breakpoint (if debugging, otherwise loop forever)
        __asm("bkpt #0");
    };
}
