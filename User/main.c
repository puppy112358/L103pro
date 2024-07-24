
#include <string.h>
#include "debug.h"
#include "ina226.h"
#include "uart.h"
#include "KAIGUAN.h"
#include "TJCScreen.h"
#include "WS2818.h"
/* EEPROM DATA ADDRESS Length Definition */
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#define Address_8bit  0
#define Address_16bit  1

/* Global Variable */
u16 RxBuffer1[6] = {0};                                             /* USART2 Using */
volatile u8 RxCnt1 = 0;
float Bus_V[8] = {0};
float current[8] = {0};
float power[8] = {0};
int COM_Mode[8] = {0};
int time = 0;
int key = 0;
int old_key = 0;
char isLongTouch = 0;

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


/* EERPOM DATA ADDRESS Length Selection */
#define Address_Lenth   Address_8bit
//#define Address_Lenth   Address_16bit

/* Global Variable */


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
/*
 * 5A  5V
 *  计算公式：
 *  电流LSB = 预计最大电流 / 2 ^ 15       0.0002

    校准寄存器=0.00512/电流LSB/0.002      12800  3200
    测量电流 = 电流寄存器值*电流LSB = 并联电压寄存器值*校准寄存器值/2048
 *  总线电压 = 总线电压寄存器值*1.25
 *  功率 = 功率寄存器值*25*电流LSB=电流*总线电压
 * */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%lu\r\n", SystemCoreClock);
    printf("ChipID:%08lx\r\n", DBGMCU_GetCHIPID());

    TIM1_Config();
    TIM1_NVIC_Config();

    USART2_Init();
    TJCScreenInit(USART2);
    WS2812Init();
    INA226_Init();
    kaiguan_Init();
    clear_TJC();

//  串口屏文本
    char Screen_Txt[20];
    char Index[3];
    volatile uint8_t led = 0;
    u16 temp = 0;

    int j = 0;
    int n = 0;//循环20次刷新一次数据

    COM_Mode[7] = WORKING;
    while (1)
    {

        key = ReadKey();
        if (old_key != key)
        {
            if (key != 0)
            {
                if (COM_Mode[key - 1] == ALARMING)//只有报警状态下有资格进
                {
                    time = 0;
                    RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1, ENABLE);//开始计时
                }
            } else
            {//松开就进
//                RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1, DISABLE);
//                time = 0;
//                if (LongTouch[old_key - 1] == 1 && COM_Mode[old_key - 1] == ALARMING)
//                {//长按
//                    COM_Mode[old_key - 1] = CLOSING;
//                    LongTouch[old_key - 1] = 0;
//                    /// TODO: 发给串口屏关闭警告
//                } else
//                {//短按
                if (isLongTouch)
                {
                    isLongTouch = 0;
                    sprintf(Screen_Txt,"p%d",old_key-1);
                    TCJSetPic(Screen_Txt,0);
                } else
                {
                    COM_Mode[old_key - 1] = !COM_Mode[old_key - 1];
                    switch (old_key)
                    {
                        case 1:
                            kaiguan1;
                            TCJSetPic("p0",COM_Mode[old_key-1]);
                            break;
                        case 2:
                            kaiguan2;
                            TCJSetPic("p1",COM_Mode[old_key-1]);

                            break;
                        case 3:
                            kaiguan3;
                            TCJSetPic("p2",COM_Mode[old_key-1]);

                            break;
                        case 4:
                            kaiguan4;
                            TCJSetPic("p3",COM_Mode[old_key-1]);

                            break;
                        case 5:
                            kaiguan5;
                            TCJSetPic("p4",COM_Mode[old_key-1]);

                            break;
                        case 6:
                            kaiguan6;
                            TCJSetPic("p5",COM_Mode[old_key-1]);

                            break;
                        case 7:
                            kaiguan7;
                            TCJSetPic("p6",COM_Mode[old_key-1]);

                            break;
                        case 8:
                            kaiguan8;
                            break;
                    }
                }

                ///  TODO:  发给串口屏开关

            }
            old_key = key;
        }

        Delay_Ms(10);
        //电压监测

        if (n++ == 20)
        {
            n = 0;
            if (led == 1)
            {
                led = 0;
            } else
            {
                led = 1;
            }
            GPIO_WriteBit(GPIOA, GPIO_Pin_0, led);
int tt;
            if (INA226_Read2Byte_I2C1(addr1, Bus_V_Reg, &temp) == 0)
            {
                Bus_V[0] = temp * (double) 0.00125003;
                if (Bus_V[0] < 1 && COM_Mode[0] == WORKING)
                {
                    COM_Mode[0] = ALARMING;
                    kaiguan1;
                    INA226_Read2Byte_I2C1(addr1, Mask_En_Reg, &tt);
                    TCJSetPic("p0", 4);
                }
            }
            if (INA226_Read2Byte_I2C1(addr2, Bus_V_Reg, &temp) == 0)
            {
                Bus_V[1] = temp * (double) 0.00125003;
                if (Bus_V[1] < 1 && COM_Mode[1] == WORKING)
                {
                    COM_Mode[1] = ALARMING;
                    INA226_Read2Byte_I2C1(addr2, Mask_En_Reg, &tt);
                    kaiguan2;
                    TCJSetPic("p1", 4);
                }
            }
            if (INA226_Read2Byte_I2C1(addr3, Bus_V_Reg, &temp) == 0)
            {
                Bus_V[2] = temp * (double) 0.00125003;
                if (Bus_V[2] < 1 && COM_Mode[2] == WORKING)
                {
                    COM_Mode[2] = ALARMING;
                    INA226_Read2Byte_I2C1(addr3, Mask_En_Reg, &tt);
                    kaiguan3;
                    TCJSetPic("p2", 4);
                }
            }
            if (INA226_Read2Byte_I2C1(addr4, Bus_V_Reg, &temp) == 0)
            {
                Bus_V[3] = temp * (double) 0.00125003;
                if (Bus_V[3] < 1 && COM_Mode[3] == WORKING)
                {
                    COM_Mode[3] = ALARMING;
                    INA226_Read2Byte_I2C1(addr4, Mask_En_Reg, &tt);
                    kaiguan4;
                    TCJSetPic("p3", 4);
                }
            }
            if (INA226_Read2Byte_I2C1(addr5, Bus_V_Reg, &temp) == 0)
            {
                Bus_V[4] = temp * (double) 0.00125003;
                if (Bus_V[4] < 1 && COM_Mode[4] == WORKING)
                {
                    COM_Mode[4] = ALARMING;
                    INA226_Read2Byte_I2C1(addr5, Mask_En_Reg, &tt);
                    kaiguan5;
                    TCJSetPic("p4", 4);
                }
            }
            if (INA226_Read2Byte_I2C2(addr6, Bus_V_Reg, &temp) == 0)
            {
                Bus_V[5] = temp * (double) 0.00125003;
                if (Bus_V[5] < 1 && COM_Mode[5] == WORKING)
                {
                    COM_Mode[5] = ALARMING;
                    INA226_Read2Byte_I2C2(addr6, Mask_En_Reg, &tt);
                    kaiguan6;
                    TCJSetPic("p5", 4);
                }
            }
            if (INA226_Read2Byte_I2C2(addr7, Bus_V_Reg, &temp) == 0)
            {
                Bus_V[6] = temp * (double) 0.00125003;
                if (Bus_V[6] < 1 && COM_Mode[6] == WORKING)
                {
                    COM_Mode[6] = ALARMING;
                    INA226_Read2Byte_I2C2(addr7, Mask_En_Reg, &tt);
                    kaiguan7;
                    TCJSetPic("p6", 4);
                }
            }
            if (INA226_Read2Byte_I2C2(addr8, Bus_V_Reg, &temp) == 0)
            {
                Bus_V[7] = temp * (double) 0.00125003;
                if (Bus_V[7] < 1 && COM_Mode[7] == WORKING)
                {
                    COM_Mode[7] = ALARMING;
                    INA226_Read2Byte_I2C2(addr8, Mask_En_Reg, &tt);
                    kaiguan8;
//                    TCJSetPic("p7",4);      第八个端口
                }
            }
            // 更新串口屏电压数据
            int j = 0;
            for (int i = 1; i <= 22; i += 3)
            {
                sprintf(Screen_Txt, "电压：%.4fV", Bus_V[j]);
                sprintf(Index, "t%d", i);
                TCJSendTxt(Index, Screen_Txt);
                j++;
            }
            //电流监测
            if (INA226_Read2Byte_I2C1(addr1, Current_Reg, &temp) == 0)
            {
                if (temp < 65000)
                {
                    current[0] = temp * 0.0001916;
                } else
                {
                    current[0] = 0;
                }
            }
            if (INA226_Read2Byte_I2C1(addr2, Current_Reg, &temp) == 0)
            {
                if (temp < 65000)
                {
                    current[1] = temp * 0.0001916;
                } else
                {
                    current[1] = 0;
                }
            }
            if (INA226_Read2Byte_I2C1(addr3, Current_Reg, &temp) == 0)
            {
                if (temp < 65000)
                {
                    current[2] = temp * 0.0001916;
                } else
                {
                    current[2] = 0;
                }
            }
            if (INA226_Read2Byte_I2C1(addr4, Current_Reg, &temp) == 0)
            {
                if (temp < 65000)
                {
                    current[3] = temp * 0.0001916;
                } else
                {
                    current[3] = 0;
                }
            }
            if (INA226_Read2Byte_I2C1(addr5, Current_Reg, &temp) == 0)
            {
                if (temp < 65000)
                {
                    current[4] = temp * 0.0001916;
                } else
                {
                    current[4] = 0;
                }
            }
            if (INA226_Read2Byte_I2C2(addr6, Current_Reg, &temp) == 0)
            {
                if (temp < 65000)
                {
                    current[5] = temp * 0.0001916;
                } else
                {
                    current[5] = 0;
                }
            }
            if (INA226_Read2Byte_I2C2(addr7, Current_Reg, &temp) == 0)
            {
                if (temp < 65000)
                {
                    current[6] = temp * 0.0001916;
                } else
                {
                    current[6] = 0;
                }
            }
            if (INA226_Read2Byte_I2C2(addr8, Current_Reg, &temp) == 0)
            {
                if (temp < 65000)
                {
                    current[7] = temp * 0.0001916;
                } else
                {
                    current[7] = 0;
                }
            }
            // 更新串口屏电流数据
            j = 0;
            for (int i = 0; i <= 21; i += 3)
            {
                sprintf(Screen_Txt, "电流：%.4fA", current[j]);
                sprintf(Index, "t%d", i);
                TCJSendTxt(Index, Screen_Txt);
                j++;
            }

            power[0] = Bus_V[0] * current[0];
            power[1] = Bus_V[1] * current[1];
            power[2] = Bus_V[2] * current[2];
            power[3] = Bus_V[3] * current[3];
            power[4] = Bus_V[4] * current[4];
            power[5] = Bus_V[5] * current[5];
            power[6] = Bus_V[6] * current[6];
            power[7] = Bus_V[7] * current[7];
            // 更新串口屏功耗数据
            j = 0;
            for (int i = 2; i <= 23; i += 3)
            {
                sprintf(Screen_Txt, "功耗：%.4fW", power[j]);
                sprintf(Index, "t%d", i);
                TCJSendTxt(Index, Screen_Txt);
                j++;
            }
        }
        uint16_t t;
        if (RxBuffer1[0])
        {
            switch (RxBuffer1[0])
            {
                case kaiguan:
                    COM_Mode[RxBuffer1[1] - 1] = RxBuffer1[2];
                    switch (RxBuffer1[1])
                    {
                        case 1:
                            kaiguan1;
                            break;
                        case 2:
                            kaiguan2;
                            break;
                        case 3:
                            kaiguan3;
                            break;
                        case 4:
                            kaiguan4;
                            break;
                        case 5:
                            kaiguan5;
                            break;
                        case 6:
                            kaiguan6;
                            break;
                        case 7:
                            kaiguan7;
                            break;
                        case 8:
                            kaiguan8;
                            break;
                    }
                    break;
                case not_baojing:
                    switch (RxBuffer1[1])
                    {
                        case 1:
                            INA226_Read2Byte_I2C1(addr1, Mask_En_Reg, &t);
                            GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
                            COM_Mode[0] = CLOSING;
                            break;
                        case 2:
                            INA226_Read2Byte_I2C1(addr2, Mask_En_Reg, &t);
                            GPIO_WriteBit(GPIOB, GPIO_Pin_13, 0);
                            COM_Mode[1] = CLOSING;
                            break;
                        case 3:
                            INA226_Read2Byte_I2C1(addr3, Mask_En_Reg, &t);
                            GPIO_WriteBit(GPIOB, GPIO_Pin_14, 0);
                            COM_Mode[2] = CLOSING;
                            break;
                        case 4:
                            INA226_Read2Byte_I2C1(addr4, Mask_En_Reg, &t);
                            GPIO_WriteBit(GPIOB, GPIO_Pin_15, 0);
                            COM_Mode[3] = CLOSING;
                            break;
                        case 5:
                            INA226_Read2Byte_I2C1(addr5, Mask_En_Reg, &t);
                            GPIO_WriteBit(GPIOA, GPIO_Pin_8, 0);
                            COM_Mode[4] = CLOSING;
                            break;
                        case 6:
                            INA226_Read2Byte_I2C2(addr6, Mask_En_Reg, &t);
                            GPIO_WriteBit(GPIOA, GPIO_Pin_11, 0);
                            COM_Mode[5] = CLOSING;
                            break;
                        case 7:
                            INA226_Read2Byte_I2C2(addr7, Mask_En_Reg, &t);
                            GPIO_WriteBit(GPIOA, GPIO_Pin_12, 0);
                            COM_Mode[6] = CLOSING;
                            break;
                        case 8:
                            INA226_Read2Byte_I2C2(addr8, Mask_En_Reg, &t);
                            GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
                            COM_Mode[7] = CLOSING;
                            break;
                    }
                    break;
                case dianliu:
                    switch (RxBuffer1[1])
                    {
                        case 1:
                            INA226_Write2Byte_I2C1(addr1,
                                                   Alert_Reg_limit,
                                                   (RxBuffer1[2] | (RxBuffer1[3] << 8)) * 800);//上限5A
                            break;
                        case 2:
                            INA226_Write2Byte_I2C1(addr2,
                                                   Alert_Reg_limit,
                                                   (RxBuffer1[2] | (RxBuffer1[3] << 8)) * 800);
                            break;
                        case 3:
                            INA226_Write2Byte_I2C1(addr3,
                                                   Alert_Reg_limit,
                                                   (RxBuffer1[2] | (RxBuffer1[3] << 8)) * 800);
                            break;
                        case 4:
                            INA226_Write2Byte_I2C1(addr4,
                                                   Alert_Reg_limit,
                                                   (RxBuffer1[2] | (RxBuffer1[3] << 8)) * 800);
                            break;
                        case 5:
                            INA226_Write2Byte_I2C1(addr5,
                                                   Alert_Reg_limit,
                                                   (RxBuffer1[2] | (RxBuffer1[3] << 8)) * 800);
                            break;
                        case 6:
                            INA226_Write2Byte_I2C2(addr6,
                                                   Alert_Reg_limit,
                                                   (RxBuffer1[2] | (RxBuffer1[3] << 8)) * 800);
                            break;
                        case 7:
                            INA226_Write2Byte_I2C2(addr7,
                                                   Alert_Reg_limit,
                                                   (RxBuffer1[2] | (RxBuffer1[3] << 8)) * 800);
                            break;
                        case 8:
                            INA226_Write2Byte_I2C2(addr8,
                                                   Alert_Reg_limit,
                                                   (RxBuffer1[2] | (RxBuffer1[3] << 8)) * 800);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            memset(RxBuffer1, 0, sizeof(RxBuffer1));
            RxCnt1 = 0;
        }
        for (int i = 0; i < 5; ++i)
        {
            if (COM_Mode[i] == CLOSING)
            {
                set5PixelColor(i, 100, 100, 100);
            } else if (COM_Mode[i] == WORKING)
            {
                set5PixelColor(i, 0, 255, 0);
            } else
            {
                set5PixelColor(i, 255, 0, 0);
            }

        }
        for (int i = 2; i >= 0; i--)
        {
            if (COM_Mode[i + 5] == CLOSING)
            {
                set3PixelColor(2 - i, 100, 100, 100);
            } else if (COM_Mode[i + 5] == WORKING)
            {
                set3PixelColor(2 - i, 0, 255, 0);
            } else
            {
                set3PixelColor(2 - i, 255, 0, 0);
            }

        }

        w2812_sync();
    }

}

void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        RxBuffer1[RxCnt1++] = USART_ReceiveData(USART2);
        USART_SendData(USART1, RxBuffer1[RxCnt1 - 1]);
//        if (RxCnt1 == 6) {
//            RxCnt1 = 0;
//        }
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA1_FLAG_TC2))
    {
        TIM_Cmd(TIM2, DISABLE);
        DMA_Cmd(DMA1_Channel2, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC2);
    }
}

void DMA1_Channel3_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA1_FLAG_TC3))
    {
        TIM_Cmd(TIM3, DISABLE);
        DMA_Cmd(DMA1_Channel3, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC3);
    }
}

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        time++;
        if (time == 2000)
        {
            time = 0;
            isLongTouch = 1;
//            LongTouch[old_key - 1] = 1;
            COM_Mode[old_key - 1] = CLOSING;
            RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1, DISABLE);
        }

    }
}

#pragma clang diagnostic pop