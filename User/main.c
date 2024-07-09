/*
 *@Note
 *I2C interface routine to operate EEPROM peripheral:
 *I2C1_SCL(PB10)\I2C1_SDA(PB11).
 *This example uses EEPROM for AT24Cxx series.
 *Steps:
 *READ EEPROM:Start + 0xA0 + 8bit Data Address + Start + 0xA1 + Read Data + Stop.
 *WRITE EERPOM:Start + 0xA0 + 8bit Data Address + Write Data + Stop.
 */

#include <string.h>
#include "debug.h"
#include "ina226.h"
#include "uart.h"
#include "TJCScreen.h"
/* EERPOM DATA ADDRESS Length Definition */
#define Address_8bit  0
#define Address_16bit  1

/* Global Variable */
u16 RxBuffer1[6] = {0};                                             /* USART2 Using */
volatile u8 RxCnt1 = 0;
int dianya_yuzhi[8] = {0};
int dianliu_yuzhi[8] = {0};
float Bus_V[8] = {0};
float current[8] = {0};
float power[8] = {0};

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


/* EERPOM DATA ADDRESS Length Selection */
#define Address_Lenth   Address_8bit
//#define Address_Lenth   Address_16bit

#define  kaiguan1 GPIO_WriteBit(GPIOB, GPIO_Pin_12, RxBuffer1[2]);
#define  kaiguan2 GPIO_WriteBit(GPIOB, GPIO_Pin_13, RxBuffer1[2]);
#define  kaiguan3 GPIO_WriteBit(GPIOB, GPIO_Pin_14, RxBuffer1[2]);
#define  kaiguan4 GPIO_WriteBit(GPIOB, GPIO_Pin_15, RxBuffer1[2]);
#define  kaiguan5 GPIO_WriteBit(GPIOA, GPIO_Pin_8, RxBuffer1[2]);
#define  kaiguan6 GPIO_WriteBit(GPIOA, GPIO_Pin_11, RxBuffer1[2]);
#define  kaiguan7 GPIO_WriteBit(GPIOA, GPIO_Pin_12, RxBuffer1[2]);
#define  kaiguan8 GPIO_WriteBit(GPIOA, GPIO_Pin_15, RxBuffer1[2]);

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
    USARTx_CFG();
    TJCScreenInit(USART2);
//  串口屏文本
    char Screen_Txt[20];
    char Index[3];
    uint8_t led = 0;
    u16 temp = 0;
    printf("SystemClk:%lu\r\n", SystemCoreClock);
    printf("ChipID:%08lx\r\n", DBGMCU_GetCHIPID());

    INA226_Init();
    int j = 0;
    for (int i = 1; i < 50; i += 7) {
        sprintf(Screen_Txt, "电压：%.4fV",0);
        sprintf(Index, "t%d", i);
        TCJSendTxt(Index, Screen_Txt);
        j++;
    }
    j=0;
    for (int i = 0; i < 51; i += 7) {
        sprintf(Screen_Txt, "电流：%.4fA", 0);
        sprintf(Index, "t%d", i);
        TCJSendTxt(Index, Screen_Txt);
        j++;
    }
    j=0;
    for (int i = 2; i < 52; i += 7) {
        sprintf(Screen_Txt, "功耗：%.4fW", 0);
        sprintf(Index, "t%d", i);
        TCJSendTxt(Index, Screen_Txt);
        j++;
    }

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);




    while (1) {
        if (led == 1) {
            led = 0;
        } else {
            led = 1;
        }
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, led);

        //接收上限值
        //接收端口是否打开
        //点灯
//        INA226_Read2Byte_I2C1(addr1,Shunt_V_Reg);
        Delay_Ms(1);
        INA226_Read2Byte_I2C1(addr1, Bus_V_Reg, &temp);
        Bus_V[0] = temp * (double) 0.00125;
        INA226_Read2Byte_I2C1(addr2, Bus_V_Reg, &temp);
        Bus_V[1] = temp * (double) 0.00125;
        INA226_Read2Byte_I2C1(addr3, Bus_V_Reg, &temp);
        Bus_V[2] = temp * (double) 0.00125;
        INA226_Read2Byte_I2C1(addr4, Bus_V_Reg, &temp);
        Bus_V[3] = temp * (double) 0.00125;
        INA226_Read2Byte_I2C1(addr5, Bus_V_Reg, &temp);
        Bus_V[4] = temp * (double) 0.00125;
        INA226_Read2Byte_I2C2(addr6, Bus_V_Reg, &temp);
        Bus_V[5] = temp * (double) 0.00125;
        INA226_Read2Byte_I2C2(addr7, Bus_V_Reg, &temp);
        Bus_V[6] = temp * (double) 0.00125;
        INA226_Read2Byte_I2C2(addr8, Bus_V_Reg, &temp);
        Bus_V[7] = temp * (double) 0.00125;
        // 刷新电压数据
        int j = 0;
        for (int i = 1; i < 50; i += 7) {
            sprintf(Screen_Txt, "电压：%.4fV", Bus_V[j]);
            sprintf(Index, "t%d", i);
            TCJSendTxt(Index, Screen_Txt);
            j++;
        }

        if (Bus_V[0] > 1) {
            INA226_Read2Byte_I2C1(addr1, Current_Reg, &temp);
            if (temp < 65000)
                current[0] = temp * 0.0002;
        } else {
            current[0] = 0;
        }
        if (Bus_V[1] > 1) {
            INA226_Read2Byte_I2C1(addr2, Current_Reg, &temp);
            if (temp < 65000)
                current[1] = temp * 0.0002;
        } else {
            current[1] = 0;
        }
        if (Bus_V[2] > 1) {
            INA226_Read2Byte_I2C1(addr3, Current_Reg, &temp);
            if (temp < 65000)
                current[2] = temp * 0.0002;
        } else {
            current[2] = 0;
        }


        if (Bus_V[3] > 1) {
            INA226_Read2Byte_I2C1(addr4, Current_Reg, &temp);
            if (temp < 65000)
                current[3] = temp * 0.0002;
        } else {
            current[3] = 0;
        }

        if (Bus_V[4] > 1) {
            INA226_Read2Byte_I2C1(addr5, Current_Reg, &temp);
            if (temp < 65000)
                current[4] = temp * 0.0002;
        } else {
            current[4] = 0;
        }

        if (Bus_V[5] > 1) {
            INA226_Read2Byte_I2C1(addr6, Current_Reg, &temp);
            if (temp < 65000)
                current[5] = temp * 0.0002;
        } else {
            current[5] = 0;
        }

        if (Bus_V[6] > 1) {
            INA226_Read2Byte_I2C1(addr7, Current_Reg, &temp);
            if (temp < 65000)
                current[6] = temp * 0.0002;
        } else {
            current[6] = 0;
        }


        if (Bus_V[7] > 1) {
            INA226_Read2Byte_I2C1(addr8, Current_Reg, &temp);
            if (temp < 65000)
                current[7] = temp * 0.0002;
        } else {
            current[7] = 0;
        }

        // 刷新电流数据
        j = 0;
        for (int i = 0; i < 51; i += 7) {
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
        // 刷新功率
        j = 0;
        for (int i = 2; i < 52; i += 7) {
            sprintf(Screen_Txt, "功耗：%.4fW", power[j]);
            sprintf(Index, "t%d", i);
            TCJSendTxt(Index, Screen_Txt);
            j++;
        }
        if (RxBuffer1[0]) {
            switch (RxBuffer1[0]) {
                case kaiguan:
                    switch (RxBuffer1[1]) {
                        case 1:
                            //1号
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
                            kaiguan6
                            break;
                        case 7:
                            kaiguan7;
                            break;
                        case 8:
                            kaiguan8;
                            break;
                    }
                    break;
                case dianliu:
                    switch (RxBuffer1[1]) {
                        case 1:
                            INA226_Write2Byte_I2C1(addr1, Alert_Reg_limit, dianliu_yuzhi[0] * 50);//上限5A
                            break;
                        case 2:
                            INA226_Write2Byte_I2C1(addr2, Alert_Reg_limit, dianliu_yuzhi[1] * 50);
                            break;
                        case 3:
                            INA226_Write2Byte_I2C1(addr3, Alert_Reg_limit, dianliu_yuzhi[2] * 50);
                            break;
                        case 4:
                            INA226_Write2Byte_I2C1(addr4, Alert_Reg_limit, dianliu_yuzhi[3] * 50);
                            break;
                        case 5:
                            INA226_Write2Byte_I2C1(addr5, Alert_Reg_limit, dianliu_yuzhi[4] * 50);
                            break;
                        case 6:
                            INA226_Write2Byte_I2C2(addr6, Alert_Reg_limit, dianliu_yuzhi[5] * 50);
                            break;
                        case 7:
                            INA226_Write2Byte_I2C2(addr6, Alert_Reg_limit, dianliu_yuzhi[6] * 50);
                            break;
                        case 8:
                            INA226_Write2Byte_I2C2(addr6, Alert_Reg_limit, dianliu_yuzhi[7] * 50);
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

    }

}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        RxBuffer1[RxCnt1++] = USART_ReceiveData(USART2);
//        USART_SendData(USART1, RxBuffer1[RxCnt1 - 1]);
//        if (RxCnt1 == 6) {
//            RxCnt1 = 0;
//        }
    }
}


