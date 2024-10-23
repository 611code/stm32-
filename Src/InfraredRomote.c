//
// Created by 18681 on 24-10-23.
//
#include "InfraredRomote.h"

uint32_t upcount;        //记录更新事件产生的次数
uint32_t valueUp;        //捕获到上升沿时刻的值
uint32_t valueDown;      //捕获到下降沿时刻的值
uint8_t  isUpCapture=1;  //判断是否为上升沿捕获，初始需要捕获上升沿，给1
uint16_t PluseWidth;     //脉宽（上升沿捕获和下降沿捕获时间的差值）
uint16_t buffer[128]={0};//接收数据缓冲区
uint16_t bufferID=0;     //数据存放的位置
uint8_t rcvFlag=0;       //接收成功标志位


char printfbuff[128]={0};
char num[4]={0};

void InfraredRemote_Init(void)
{
    HAL_TIM_Base_Start_IT(&htim1);               //定时器更新时，产出中断
    HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_4);   //开启输入捕获通道中断
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  //更新中断回调函数
{
    upcount++;
    // OLED_ShowNum(0,60,upcount,5,6);OLED_Update();

}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)    //输入捕获中断回调函数
{
    if(isUpCapture)  //如果是上升沿捕获
    {
        valueUp=HAL_TIM_ReadCapturedValue(htim ,TIM_CHANNEL_4);   //读取捕获上升沿时刻的计数器值
        isUpCapture=0;
        __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);  //设置为下降沿捕获
        upcount=0;  //清零更新计数器
    }
    else  //如果是下降沿捕获
    {
        valueDown=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_4);
        isUpCapture=1;
        __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_RISING);
        PluseWidth=valueDown+upcount*65535 -valueUp  ;    //upcount*65535 防止计数器溢出   单位是1us

        //同步码 (低电平9ms,高电平4.5ms)
        if(PluseWidth>4400 && PluseWidth<4600)
        {
            bufferID=0;
            buffer[bufferID++]=PluseWidth;
        }
        else if (bufferID>0)   //收到了同步码
        {
            buffer[bufferID++] = PluseWidth;
            if (bufferID>32)   //如果接收到了32位（指令为32位   1同步码头+8地址码+8地址反码+8控制码+8控制反码）
            {
                rcvFlag=1;
                bufferID=0;    //继续查找同步码
            }
        }
    }

}

void bitbuffer_to_num(char num[])  //将buffer中的数据转换为4字节数字存储在num中
{
    num[0]=0;
    num[1]=0;
    num[2]=0;
    num[3]=0;
    for(int i=0;i<32;i++)       //按8位分割
    {
        if (buffer[i+1] <1000)  //二进制为为0
        {
            num[i/8] = num[i/8]<<1;
        }
        else                    //二进制为为1
        {
            num[i/8] = num[i/8]<<1;
            num[i/8] |= 0x01;
        }
    }
}
uint16_t InfraredRemote_Get_KeyNum(void)
{
    if (rcvFlag==1)
    {
        for(int i=0;i<4;i++)
        {
            bitbuffer_to_num(num);
        }

        rcvFlag=0;
    }

    return (uint16_t)num[2];
}


