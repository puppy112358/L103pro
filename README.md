# WS2812
WS2812 接收的频率最好是800 000Hz，因此单片机输出的PWM信号的频率应为此值；

初始化函数为 `WS2812Init()`；

本例程为使用TIM2通道2和TIM3通道2驱动两串灯，可根据需要在初始化阶段修改定时器通道编号以及DMA通道编号即可；

`set3PixelColor(i, 0, 0, 0)`和`set6PixelColor(i, 0, 0, 0)`函数用于直接点灯，i为该io口连接的第i个灯，后面三个数值是该灯想要设置的RGB颜色；

每次执行完一次`set3PixelColor(i, 0, 0, 0)`和`set6PixelColor(i, 0, 0, 0)`函数都要执行 `w2812_sync();`才能实现点灯或者改变颜色效果，同样该函数内部的定时器DMA设置可自行修改；