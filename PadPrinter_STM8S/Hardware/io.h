#ifndef __IO_H_
#define __IO_H_

//输出点
#define OUT_UPDOWN_PORT     GPIOB     //印头上下气缸
#define OUT_UPDOWN_PIN      GPIO_PIN_2  
#define OUT_FRONTBACK_PORT  GPIOB     //印头前后气缸
#define OUT_FRONTBACK_PIN   GPIO_PIN_3
#define OUT_SHIFT_PORT      GPIOB     //辅助移位气缸
#define OUT_SHIFT_PIN       GPIO_PIN_1
#define OUT_POS_PORT        GPIOE     //辅助定位气缸
#define OUT_POS_PIN         GPIO_PIN_5

//输入点
#define IN_ABSORB_O_PORT    GPIOC     //吸油原始位
#define IN_ABSORB_O_PIN     GPIO_PIN_3
#define IN_ABSORB_L_PORT    GPIOC     //吸油下限位
#define IN_ABSORB_L_PIN     GPIO_PIN_2
#define IN_PRINT_O_PORT     GPIOC     //印油原始位
#define IN_PRINT_O_PIN      GPIO_PIN_4
#define IN_PRINT_L_PORT     GPIOC     //印油下限位
#define IN_PRINT_L_PIN      GPIO_PIN_5

#define IN_SHIFT_O_PORT     GPIOD     //移位原始位
#define IN_SHIFT_O_PIN      GPIO_PIN_3
#define IN_POS_PORT         GPIOD     //定位传感器
#define IN_POS_PIN          GPIO_PIN_2
#define IN_SHIFT_L1_PORT    GPIOD     //移位极限位1
#define IN_SHIFT_L1_PIN     GPIO_PIN_0
#define IN_SHIFT_L2_PORT    GPIOC     //移位极限位2
#define IN_SHIFT_L2_PIN     GPIO_PIN_7
//#define IN_SHIFT_O_PORT     GPIOC     //移位原始位
//#define IN_SHIFT_O_PIN      GPIO_PIN_7
//#define IN_POS_PORT         GPIOD     //定位传感器
//#define IN_POS_PIN          GPIO_PIN_0
//#define IN_SHIFT_L1_PORT    GPIOD     //移位极限位1
//#define IN_SHIFT_L1_PIN     GPIO_PIN_2
//#define IN_SHIFT_L2_PORT    GPIOD     //移位极限位2
//#define IN_SHIFT_L2_PIN     GPIO_PIN_3

#define IN_SENSOR_LEVEL_PORT    GPIOD     //移位极限位2
#define IN_SENSOR_LEVEL_PIN     GPIO_PIN_7

//该输入点与上面的输入点共用，所以不用修改
#define IN_FRONT_O_PORT     IN_ABSORB_O_PORT    //前后原始位
#define IN_FRONT_O_PIN      IN_ABSORB_O_PIN
#define IN_FRONT_L_PORT     IN_PRINT_O_PORT     //前后极限位
#define IN_FRONT_L_PIN      IN_PRINT_O_PIN

void IO_Init(void);

#endif
