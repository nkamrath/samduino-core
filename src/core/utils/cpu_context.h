#ifndef _CPU_CONTEXT_T_
#define _CPU_CONTEXT_T_

#include <asf.h>

#define CPU_CONTEXT__THREAD 0

#define CpuContext_Get() (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)

#endif