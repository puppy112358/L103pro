//
// Created by 86180 on 2024/7/20.
//

#ifndef L103PRO_USER_KAIGUAN_H_
#define L103PRO_USER_KAIGUAN_H_

#define  kaiguan1 GPIO_WriteBit(GPIOB, GPIO_Pin_12, RxBuffer1[2]);
#define  kaiguan2 GPIO_WriteBit(GPIOB, GPIO_Pin_13, RxBuffer1[2]);
#define  kaiguan3 GPIO_WriteBit(GPIOB, GPIO_Pin_14, RxBuffer1[2]);
#define  kaiguan4 GPIO_WriteBit(GPIOB, GPIO_Pin_15, RxBuffer1[2]);
#define  kaiguan5 GPIO_WriteBit(GPIOA, GPIO_Pin_8, RxBuffer1[2]);
#define  kaiguan6 GPIO_WriteBit(GPIOA, GPIO_Pin_11, RxBuffer1[2]);
#define  kaiguan7 GPIO_WriteBit(GPIOA, GPIO_Pin_12, RxBuffer1[2]);
#define  kaiguan8 GPIO_WriteBit(GPIOA, GPIO_Pin_15, RxBuffer1[2]);

#define jianpan1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)
#define jianpan2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)
#define jianpan3 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define jianpan4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define jianpan5 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)
#define jianpan6 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)
#define jianpan7 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define jianpan8 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)

enum working_mode{
    closing = 0,
    working = 1,
    baojing
};
void kaiguan_Init();
void BASIC_TIM_Config(void);
void BASIC_TIM_NVIC_Config(void);
int ReadKey();


#endif //L103PRO_USER_KAIGUAN_H_
