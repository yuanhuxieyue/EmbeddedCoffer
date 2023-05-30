#include "stm32f4xx_hal.h"
#include "iwdg.h"

void IWDG_Init(IWDG_HandleTypeDef *hiwdg){
    hiwdg->Instance = IWDG;
    hiwdg->Init.Prescaler = IWDG_PRESCALER_4;
    hiwdg->Init.Reload = 4095;
    if(HAL_IWDG_Init(hiwdg) != HAL_OK){
        printf("ERROR!\n");
    }
}