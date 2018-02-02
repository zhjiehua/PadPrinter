#ifndef __IO_H_
#define __IO_H_

//�����
#define OUT_UPDOWN_PORT     GPIOB     //ӡͷ��������
#define OUT_UPDOWN_PIN      GPIO_PIN_2  
#define OUT_FRONTBACK_PORT  GPIOB     //ӡͷǰ������
#define OUT_FRONTBACK_PIN   GPIO_PIN_3
#define OUT_SHIFT_PORT      GPIOB     //������λ����
#define OUT_SHIFT_PIN       GPIO_PIN_1
#define OUT_POS_PORT        GPIOE     //������λ����
#define OUT_POS_PIN         GPIO_PIN_5

//�����
#define IN_ABSORB_O_PORT    GPIOC     //����ԭʼλ
#define IN_ABSORB_O_PIN     GPIO_PIN_3
#define IN_ABSORB_L_PORT    GPIOC     //��������λ
#define IN_ABSORB_L_PIN     GPIO_PIN_2
#define IN_PRINT_O_PORT     GPIOC     //ӡ��ԭʼλ
#define IN_PRINT_O_PIN      GPIO_PIN_4
#define IN_PRINT_L_PORT     GPIOC     //ӡ������λ
#define IN_PRINT_L_PIN      GPIO_PIN_5

#define IN_SHIFT_O_PORT     GPIOD     //��λԭʼλ
#define IN_SHIFT_O_PIN      GPIO_PIN_3
#define IN_POS_PORT         GPIOD     //��λ������
#define IN_POS_PIN          GPIO_PIN_2
#define IN_SHIFT_L1_PORT    GPIOD     //��λ����λ1
#define IN_SHIFT_L1_PIN     GPIO_PIN_0
#define IN_SHIFT_L2_PORT    GPIOC     //��λ����λ2
#define IN_SHIFT_L2_PIN     GPIO_PIN_7
//#define IN_SHIFT_O_PORT     GPIOC     //��λԭʼλ
//#define IN_SHIFT_O_PIN      GPIO_PIN_7
//#define IN_POS_PORT         GPIOD     //��λ������
//#define IN_POS_PIN          GPIO_PIN_0
//#define IN_SHIFT_L1_PORT    GPIOD     //��λ����λ1
//#define IN_SHIFT_L1_PIN     GPIO_PIN_2
//#define IN_SHIFT_L2_PORT    GPIOD     //��λ����λ2
//#define IN_SHIFT_L2_PIN     GPIO_PIN_3

#define IN_SENSOR_LEVEL_PORT    GPIOD     //��λ����λ2
#define IN_SENSOR_LEVEL_PIN     GPIO_PIN_7

//������������������㹲�ã����Բ����޸�
#define IN_FRONT_O_PORT     IN_ABSORB_O_PORT    //ǰ��ԭʼλ
#define IN_FRONT_O_PIN      IN_ABSORB_O_PIN
#define IN_FRONT_L_PORT     IN_PRINT_O_PORT     //ǰ����λ
#define IN_FRONT_L_PIN      IN_PRINT_O_PIN

void IO_Init(void);

#endif
