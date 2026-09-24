#ifndef HEADER_H
#define HEADER_H


/*==================================================
                    TYPES
==================================================*/

typedef unsigned int  u32;
typedef float         f32;
typedef int           s32;
typedef const char    cs8;
typedef unsigned char u8;
typedef char          s8;


/*==================================================
                    DELAY
==================================================*/

void dly_us(u32);
void dly_ms(u32);
void dly_s(u32);


/*==================================================
                    PCB
==================================================*/

void cfgPortPinFunc(u32, u32, u32);


/*==================================================
                    LCD
==================================================*/

void WriteLCD(u8);
void CmdLCD(u8);
void InitLCD(void);
void CharLCD(u8);
void StrLCD(s8 *);
void U32LCD(u32);
void S32LCD(s32);
void F32LCD(f32, u32);


/*==================================================
               EXTERNAL INTERRUPTS
==================================================*/

void EINT0_ISR(void) __irq;
void EINT1_ISR(void) __irq;

void enable_eint0(void);
void enable_eint1(void);


/*==================================================
                    KEYPAD
==================================================*/

void Init_kpm(void);
u32 RowCheck(void);
u32 ColCheck(void);
u32 Colscan(void);
u32 KeyScan(void);

u32 ReadNum(int);
u32 ReadPassword(void);


/*==================================================
                     RTC
==================================================*/

void RTC_Init(void);

void SetRtcTime(void);
void SetRtcDate(void);

void GetRtcTime(s32 *, s32 *, s32 *);
void GetRtcdate(s32 *, s32 *, s32 *);

void DispRtcTime(u32, u32, u32);
void DispRtcDate(u32, u32, u32);

void Duration(void);
void DispTemp(void);


/*==================================================
                  PROJECT FUNCTIONS
==================================================*/

void Rtc_edit(void);
void Start_time(void);
void verifyPass(void);
void View_Loginfo(void);
void Glow_Led(void);
void AdminMenu(void);
void Display2Digit(u32);


/*==================================================
                     ADC
==================================================*/

void Init_ADC(void);

void Read_ADC(u32,
              u32 *,
              f32 *);

f32 Read_LM35DegC(void);


/*==================================================
                   7-SEGMENT
==================================================*/

void Init_7segs(void);
void disp_2mux_7segs(int);


#endif