#include "stm8s.h"
#include "io.h"
#include "uart1.h"

#include <stdio.h>

void IO_Init(void)
{  
//    GPIO_DeInit(OUT_UPDOWN_PORT);
//    GPIO_DeInit(OUT_FRONTBACK_PORT);
//    GPIO_DeInit(OUT_SHIFT_PORT);
//    GPIO_DeInit(OUT_POS_PORT);
//    GPIO_DeInit(IN_ABSORB_O_PORT);
//    GPIO_DeInit(IN_ABSORB_L_PORT);
//    GPIO_DeInit(IN_PRINT_O_PORT);
//    GPIO_DeInit(IN_PRINT_L_PORT);
//    GPIO_DeInit(IN_SHIFT_O_PORT);
//    GPIO_DeInit(IN_POS_PORT);
//    GPIO_DeInit(IN_SHIFT_L1_PORT);
//    GPIO_DeInit(IN_SHIFT_L2_PORT);
    
    GPIO_Init(OUT_UPDOWN_PORT, OUT_UPDOWN_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(OUT_FRONTBACK_PORT, OUT_FRONTBACK_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(OUT_SHIFT_PORT, OUT_SHIFT_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(OUT_POS_PORT, OUT_POS_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    
    GPIO_Init(IN_ABSORB_O_PORT, IN_ABSORB_O_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(IN_ABSORB_L_PORT, IN_ABSORB_L_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(IN_PRINT_O_PORT, IN_PRINT_O_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(IN_PRINT_L_PORT, IN_PRINT_L_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(IN_SHIFT_O_PORT, IN_SHIFT_O_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(IN_POS_PORT, IN_POS_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(IN_SHIFT_L1_PORT, IN_SHIFT_L1_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(IN_SHIFT_L2_PORT, IN_SHIFT_L2_PIN, GPIO_MODE_IN_PU_NO_IT);
}