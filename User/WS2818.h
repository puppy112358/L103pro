//
// Created by 86180 on 2024/7/21.
//

#ifndef L103PRO_USER_WS2818_H_
#define L103PRO_USER_WS2818_H_
#include "debug.h"
#include "ch32l103.h"
#define CODE_1  7
#define CODE_0 3
#define RESET_LEN    (60)
#define  Pixel_NUM 8
#define COLOR5_BUFFER_LEN ((5*(3*8))+RESET_LEN)
#define COLOR4_BUFFER_LEN ((4*(3*8))+RESET_LEN)
#define MAX_STEP (200)
#define LIST_SIZE(list) (sizeof(list)/sizeof(list[0]))
#define hex2rgb(c) (((c)>>16)&0xff),(((c)>>8)&0xff),((c)&0xff)
#define TIM2_CH2CVR_ADDRESS    0x40000038
#define TIM3_CH2CVR_ADDRESS    0x40000438

#define kai_color 0x00FF00//green
#define close_color 0x00
#define baojing_color 0xFF00

void TIM2_PWMOut_Init(u16 arr, u16 psc, u16 ccp);
void TIM3_PWMOut_Init(u16 arr, u16 psc, u16 ccp);
void TIM2_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u16 bufsize);
void TIM3_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u16 bufsize);

void set5PixelColor(uint16_t id, uint8_t r, uint8_t g, uint8_t b);
void set4PixelColor(uint16_t id, uint8_t r, uint8_t g, uint8_t b);
void w2812_sync();
uint32_t interpolateColors(uint32_t color1, uint32_t color2, uint8_t step);
void led_example_0(void);
void led_example_1(void);
void led_example_2(void);
void WS2812Init();



#endif //L103PRO_USER_WS2818_H_
