#ifndef DEFINES_H
#define DEFINES_H


/*==================================================
                    LCD DEFINES
==================================================*/

/* LCD command definitions */

#define MODE_8BIT_2LINE      0x38
#define DSP_ON_CUR_OFF       0x0C
#define CLEAR_LCD            0x01
#define SHIFT_CUR_RIGHT      0x06


/* LCD data pins
   P0.8 - P0.15 */

#define LCD_DATA             8


/* LCD control pins */

#define RS                   19
#define RW                   17
#define EN                   18


/*==================================================
                   KEYPAD DEFINES
==================================================*/

/* Row pins
   P1.16 - P1.19 */

#define ROW0                 16
#define ROW1                 17
#define ROW2                 18
#define ROW3                 19


/* Column pins
   P1.20 - P1.23 */

#define COL0                 20
#define COL1                 21
#define COL2                 22
#define COL3                 23


/*==================================================
                     ADC DEFINES
==================================================*/

#define FOSC                 12000000
#define CCLK                 (5 * FOSC)
#define PCLK                 (CCLK / 4)

#define ADCLK                3000000

#define DIVIDER              ((PCLK / ADCLK) - 1)

#define CLKDIV_VALUE         (DIVIDER << 8)

#define PDN_BIT              (1 << 21)

#define START_CONV           (1 << 24)


/* ADC result */

#define RESULT               6
#define DONE_BIT             31


/* ADC channels */

#define CH0                  (1 << 0)
#define CH1                  (1 << 1)
#define CH2                  (1 << 2)
#define CH3                  (1 << 3)


/*==================================================
                EXTERNAL INTERRUPTS
==================================================*/

/*
   LPC2148:

   EINT0 -> P0.16
   EINT1 -> P0.14

   VIC:
   EINT0 = Channel 14
   EINT1 = Channel 15
*/

#define EINT0_CH             14
#define EINT1_CH             17


/*==================================================
                       RTC
==================================================*/

#define RTC_PCLK             (CCLK / 4)

#define PREINT1              ((int)(RTC_PCLK / 32768) - 1)

#define PREFRAC1             (RTC_PCLK - \
                             ((PREINT1 + 1) * 32768))


/* RTC Control Register */

#define RTC_EN               (1 << 0)
#define RTC_RESET            (1 << 1)
#define RTC_CLKSRC           (1 << 4)


/*==================================================
                       LEDs
==================================================*/

/*
   LEDs are connected to Port 1
*/

#define LED_GREEN            (1 << 28)
#define LED_YELLOW           (1 << 27)
#define LED_RED              (1 << 26)


/*==================================================
                  7-SEGMENT
==================================================*/

/*
   Segment data:
   P0.0 - P0.7

   Digit select:
   P0.20 and P0.21
*/

#define DSEL1                20
#define DSEL2                21


/*==================================================
                     BUZZER
==================================================*/

/*
   Buzzer -> P1.30
*/

#define BUZZER               (1 << 30)


#endif