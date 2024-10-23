//
// Created by 18681 on 24-10-23.
//

#ifndef _INFRAREDROMOTE_H
#define _INFRAREDROMOTE_H
#include "OLED.h"
#include "tim.h"



// uint16_t InfraredRemote_Get_KeyNum2(void);
void InfraredRemote_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void bitbuffer_to_num(char num[]);
uint16_t InfraredRemote_Get_KeyNum(void);

#endif //_INFRAREDROMOTE_H
