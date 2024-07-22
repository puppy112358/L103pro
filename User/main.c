
#include <string.h>
#include "debug.h"
#include "ina226.h"
#include "uart.h"
#include "KAIGUAN.h"
#include "TJCScreen.h"
#include "WS2818.h"
/* EEPROM DATA ADDRESS Length Definition */
#define Address_8bit  0
#define Address_16bit  1

/* Global Variable */
u16 RxBuffer1[6] = {0};                                             /* USART2 Using */
volatile u8 RxCnt1 = 0;
int dianliu_yuzhi[8] = {0};
float Bus_V[8] = {0};
float current[8] = {0};
float power[8] = {0};
int kaiguanmode[8] = {0};
int time = 0;
int chang_an[8] = {0};
int key = 0;
int old_key = 0;
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void BASIC_TIM_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));


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
int main(void) {
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

    USART2_Init();
    TJCScreenInit(USART2);
    BASIC_TIM_Config();
    BASIC_TIM_NVIC_Config();
//    WS2812Init();
    INA226_Init();
    kaiguan_Init();
    clear_TJC();

//  串口屏文本
    char Screen_Txt[20];
    char Index[3];
    uint8_t led = 0;
    u16 temp = 0;

    int j = 0;
    int n = 0;//循环20次刷新一次数据

    uint32_t color__list[] = {
        0x00aa00,
        0x00ff00,
        0x0000ff,
        0x00ffaa,
        0x00aaff,
        0x00ffff,
        0x00fbbf,
    };
    uint32_t c;
/*
 * red ff 00 00
 * xx  00 ff 00 1 00ff00
 * gua 00 00 00 0
 */
    while (1) {
        c = color__list[n%8];//变色

        key = ReadKey();
        if (old_key != key){
            if (key != 0) {
                RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1, ENABLE);//开始计时
            } else
            {
                RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1,DISABLE);
                time = 0;
            }
            old_key = key;
        }
        n++;
        if (chang_an[old_key-1] == 1){
            kaiguanmode[old_key-1] = working;
            chang_an[old_key-1] = 0;
            set5PixelColor(2,hex2rgb(c));


        }


        if (led == 1) {
            led = 0;
        } else {
            led = 1;
        }

        GPIO_WriteBit(GPIOA, GPIO_Pin_0, led);

        Delay_Ms(1);
        //电压监测
        if (n == 20) {
            n = 0;
            if (INA226_Read2Byte_I2C1(addr1, Bus_V_Reg, &temp) == 0) {
                Bus_V[0] = temp * (double) 0.00125;
                if (Bus_V[0] < 1 && kaiguanmode[0] == working) {
                    kaiguanmode[0] = baojing;
                    set5PixelColor(0, 0xff, 0, 0);
                }
            }
            if (INA226_Read2Byte_I2C1(addr2, Bus_V_Reg, &temp) == 0) {
                Bus_V[1] = temp * (double) 0.00125;
                if (Bus_V[1] < 1 && kaiguanmode[1] == working) {
                    kaiguanmode[1] = baojing;
                    set5PixelColor(1, 0xff, 0, 0);
                }
            }
            if (INA226_Read2Byte_I2C1(addr3, Bus_V_Reg, &temp) == 0) {
                Bus_V[2] = temp * (double) 0.00125;
                if (Bus_V[2] < 1 && kaiguanmode[2] == working) {
                    kaiguanmode[2] = baojing;
                    set5PixelColor(2, 0xff, 0, 0);
                }
            }
            if (INA226_Read2Byte_I2C1(addr4, Bus_V_Reg, &temp) == 0) {
                Bus_V[3] = temp * (double) 0.00125;
                if (Bus_V[3] < 1 && kaiguanmode[3] == working) {
                    kaiguanmode[3] = baojing;
                    set5PixelColor(3, 0xff, 0, 0);
                }
            }
            if (INA226_Read2Byte_I2C1(addr5, Bus_V_Reg, &temp) == 0) {
                Bus_V[4] = temp * (double) 0.00125;
                if (Bus_V[4] < 1 && kaiguanmode[4] == working) {
                    kaiguanmode[4] = baojing;
                    set5PixelColor(4, 0xff, 0, 0);
                }
            }
            if (INA226_Read2Byte_I2C2(addr6, Bus_V_Reg, &temp) == 0) {
                Bus_V[5] = temp * (double) 0.00125;
                if (Bus_V[5] < 1 && kaiguanmode[5] == working) {
                    kaiguanmode[5] = baojing;
                    set3PixelColor(0, 0xff, 0, 0);
                }
            }
            if (INA226_Read2Byte_I2C2(addr7, Bus_V_Reg, &temp) == 0) {
                Bus_V[6] = temp * (double) 0.00125;
                if (Bus_V[6] < 1 && kaiguanmode[6] == working) {
                    kaiguanmode[6] = baojing;
                    set3PixelColor(1, 0xff, 0, 0);
                }
            }
            if (INA226_Read2Byte_I2C2(addr8, Bus_V_Reg, &temp) == 0) {
                Bus_V[7] = temp * (double) 0.00125;
                if (Bus_V[7] < 1 && kaiguanmode[7] == working) {
                    kaiguanmode[7] = baojing;
                    set3PixelColor(2, 0xff, 0, 0);
                }
            }
            // 更新串口屏电压数据
            int j = 0;
            for (int i = 1; i < 22; i += 3) {
                sprintf(Screen_Txt, "电压：%.4fV", Bus_V[j]);
                sprintf(Index, "t%d", i);
                TCJSendTxt(Index, Screen_Txt);
                j++;
            }
            //电流监测
            if (INA226_Read2Byte_I2C1(addr1, Current_Reg, &temp) == 0) {
                if (temp < 65000) {
                    current[0] = temp * 0.0002;
                } else {
                    current[0] = 0;
                }
            }
            if (INA226_Read2Byte_I2C1(addr2, Current_Reg, &temp) == 0) {
                if (temp < 65000) {
                    current[1] = temp * 0.0002;
                } else {
                    current[1] = 0;
                }
            }
            if (INA226_Read2Byte_I2C1(addr3, Current_Reg, &temp) == 0) {
                if (temp < 65000) {
                    current[2] = temp * 0.0002;
                } else {
                    current[2] = 0;
                }
            }
            if (INA226_Read2Byte_I2C1(addr4, Current_Reg, &temp) == 0) {
                if (temp < 65000) {
                    current[3] = temp * 0.0002;
                } else {
                    current[3] = 0;
                }
            }
            if (INA226_Read2Byte_I2C1(addr5, Current_Reg, &temp) == 0) {
                if (temp < 65000) {
                    current[4] = temp * 0.0002;
                } else {
                    current[4] = 0;
                }
            }
            if (INA226_Read2Byte_I2C2(addr6, Current_Reg, &temp) == 0) {
                if (temp < 65000) {
                    current[5] = temp * 0.0002;
                } else {
                    current[5] = 0;
                }
            }
            if (INA226_Read2Byte_I2C2(addr7, Current_Reg, &temp) == 0) {
                if (temp < 65000) {
                    current[6] = temp * 0.0002;
                } else {
                    current[6] = 0;
                }
            }
            if (INA226_Read2Byte_I2C2(addr8, Current_Reg, &temp) == 0) {
                if (temp < 65000) {
                    current[7] = temp * 0.0002;
                } else {
                    current[7] = 0;
                }
            }
            // 更新串口屏电流数据
            j = 0;
            for (int i = 0; i < 21; i += 3) {
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
            for (int i = 2; i < 23; i += 3) {
                sprintf(Screen_Txt, "功耗：%.4fW", power[j]);
                sprintf(Index, "t%d", i);
                TCJSendTxt(Index, Screen_Txt);
                j++;
            }
        }
        if (RxBuffer1[0]) {
            switch (RxBuffer1[0]) {
                case kaiguan:
                    switch (RxBuffer1[1]) {
                        case 1:
                            kaiguan1
                            kaiguanmode[0] = RxBuffer1[2];
                            set5PixelColor(1, hex2rgb(c&RxBuffer1[2]));
                            break;
                        case 2:
                            kaiguan2
                            kaiguanmode[1] = RxBuffer1[2];
                            set5PixelColor(2,hex2rgb(c&RxBuffer1[2]));
                            break;
                        case 3:
                            kaiguan3
                            kaiguanmode[2] = RxBuffer1[2];
                            set5PixelColor(3,hex2rgb(c&RxBuffer1[2]));
                            break;
                        case 4:
                            kaiguan4
                            kaiguanmode[3] = RxBuffer1[2];
                            set5PixelColor(4,hex2rgb(c&RxBuffer1[2]));
                            break;
                        case 5:
                            kaiguan5
                            kaiguanmode[4] = RxBuffer1[2];
                            set5PixelColor(5,hex2rgb(c&RxBuffer1[2]));
                            break;
                        case 6:
                            kaiguan6
                            kaiguanmode[5] = RxBuffer1[2];
                            set3PixelColor(1,hex2rgb(c&RxBuffer1[2]));
                            break;
                        case 7:
                            kaiguan7
                            kaiguanmode[6] = RxBuffer1[2];
                            set3PixelColor(2,hex2rgb(c&RxBuffer1[2]));
                            break;
                        case 8:
                            kaiguan8
                            kaiguanmode[7] = RxBuffer1[2];
                            set3PixelColor(3,hex2rgb(c&RxBuffer1[2]));
                            break;
                    }
                    break;
                case dianliu:
                    switch (RxBuffer1[1]) {
                        case 1:
                            INA226_Write2Byte_I2C1(addr1, Alert_Reg_limit, dianliu_yuzhi[0] * 8);//上限5A
                            break;
                        case 2:
                            INA226_Write2Byte_I2C1(addr2, Alert_Reg_limit, dianliu_yuzhi[1] * 8);
                            break;
                        case 3:
                            INA226_Write2Byte_I2C1(addr3, Alert_Reg_limit, dianliu_yuzhi[2] * 8);
                            break;
                        case 4:
                            INA226_Write2Byte_I2C1(addr4, Alert_Reg_limit, dianliu_yuzhi[3] * 8);
                            break;
                        case 5:
                            INA226_Write2Byte_I2C1(addr5, Alert_Reg_limit, dianliu_yuzhi[4] * 8);
                            break;
                        case 6:
                            INA226_Write2Byte_I2C2(addr6, Alert_Reg_limit, dianliu_yuzhi[5] * 8);
                            break;
                        case 7:
                            INA226_Write2Byte_I2C2(addr7, Alert_Reg_limit, dianliu_yuzhi[6] * 8);
                            break;
                        case 8:
                            INA226_Write2Byte_I2C2(addr8, Alert_Reg_limit, dianliu_yuzhi[7] * 8);
                            break;
                        default:
                            break;
                    }
                    dianliu_yuzhi[RxBuffer1[1] - 1] = RxBuffer1[2];
                    dianliu_yuzhi[RxBuffer1[1] - 1] = dianliu_yuzhi[RxBuffer1[1] - 1] + (RxBuffer1[3] << 4);
                    break;
                default:
                    break;
            }
            memset(RxBuffer1, 0, sizeof(RxBuffer1));
            RxCnt1 = 0;
        }
        w2812_sync();
    }

}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        RxBuffer1[RxCnt1++] = USART_ReceiveData(USART2);
        USART_SendData(USART1, RxBuffer1[RxCnt1 - 1]);
//        if (RxCnt1 == 6) {
//            RxCnt1 = 0;
//        }
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if (DMA_GetFlagStatus( DMA1_FLAG_TC2)) {
        TIM_Cmd( TIM2, DISABLE);
        DMA_Cmd( DMA1_Channel2, DISABLE);
        DMA_ClearFlag( DMA1_FLAG_TC2);
    }
}
void BASIC_TIM_IRQHandler (void)
{
    if ( TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET ) {
        time++;
        if (time == 2000){
            time = 0;
            chang_an[old_key-1] = 1;
            RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1, DISABLE);
        }
        TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
    }
}
