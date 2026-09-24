#include <lpc21xx.h>

#include "defines.h"
#include "header.h"


/*==================================================
                  GLOBAL VARIABLES
==================================================*/

extern volatile int password_request;
extern volatile int admin_mode;

extern volatile int pause_flag;
extern volatile int pause_count;

extern volatile int exam_started;
extern volatile int cntdown;

extern volatile int last_minute;

extern int hr;
extern int min;
int dur=0;


/* Log information */

extern int log_available;

extern int prev_exam_start_hour;
extern int prev_exam_start_min;
extern int prev_exam_start_sec;

extern int prev_exam_start_day;
extern int prev_exam_start_month;
extern int prev_exam_start_year;

extern int prev_exam_end_hour;
extern int prev_exam_end_min;


/* Password display flag */

int pf = 0;


/*==================================================
                    LCD FUNCTIONS
==================================================*/

void WriteLCD(u8 dat)
{
    /* RW = 0 -> Write operation */
    IOCLR0 = (1 << RW);

    /* Put data on P0.8-P0.15 */
    IOPIN0 = (IOPIN0 & ~(255 << LCD_DATA)) |
             (dat << LCD_DATA);

    /* Enable pulse */
    IOSET0 = (1 << EN);

    dly_us(1);

    IOCLR0 = (1 << EN);

    dly_ms(2);
}


void CmdLCD(u8 cmd)
{
    /* RS = 0 -> Command register */
    IOCLR0 = (1 << RS);

    WriteLCD(cmd);
}


void InitLCD(void)
{
    /* LCD data pins */
    IODIR0 |= (255 << LCD_DATA);

    /* LCD control pins */
    IODIR0 |= (1 << RS) |
              (1 << RW) |
              (1 << EN);

    dly_ms(15);

    CmdLCD(0x30);

    dly_ms(4);

    dly_us(100);

    CmdLCD(0x30);

    dly_us(100);

    CmdLCD(0x30);

    CmdLCD(MODE_8BIT_2LINE);

    CmdLCD(DSP_ON_CUR_OFF);

    CmdLCD(CLEAR_LCD);

    CmdLCD(SHIFT_CUR_RIGHT);
}


void CharLCD(u8 asciiVal)
{
    /* RS = 1 -> Data register */
    IOSET0 = (1 << RS);

    WriteLCD(asciiVal);
}


void StrLCD(s8 *str)
{
    while(*str)
    {
        CharLCD(*str++);
    }
}


void U32LCD(u32 num)
{
    u32 a[10];
    s32 i = 0;

    if(num == 0)
    {
        CharLCD('0');
    }
    else
    {
        while(num > 0)
        {
            a[i++] = num % 10;
            num = num / 10;
        }

        for(--i; i >= 0; i--)
        {
            CharLCD(a[i] + 48);
        }
    }
}


void S32LCD(s32 num)
{
    if(num < 0)
    {
        CharLCD('-');
        num = -num;
    }

    U32LCD(num);
}


void F32LCD(f32 fn, u32 ndp)
{
    u32 n;
    u32 i;

    if(fn < 0.0)
    {
        CharLCD('-');
        fn = -fn;
    }

    n = fn;

    U32LCD(n);

    CharLCD('.');

    for(i = 0; i < ndp; i++)
    {
        fn = (fn - n) * 10;

        n = fn;

        CharLCD(n + 48);
    }
}


/*==================================================
                  DELAY FUNCTIONS
==================================================*/

void dly_us(u32 dlyUs)
{
    dlyUs *= 12;

    while(dlyUs--);
}


void dly_ms(u32 dlyMs)
{
    dlyMs *= 12000;

    while(dlyMs--);
}


void dly_s(u32 dlyS)
{
    dlyS *= 12000000;

    while(dlyS--);
}


/*==================================================
                PORT PIN CONFIGURATION
==================================================*/

void cfgPortPinFunc(u32 PortN,
                    u32 PinN,
                    u32 FuncN)
{
    if(PortN == 0)
    {
        if(PinN <= 15)
        {
            PINSEL0 =
                (PINSEL0 & ~(3 << (2 * PinN))) |
                (FuncN << (2 * PinN));
        }
        else if(PinN >= 16 && PinN <= 31)
        {
            PINSEL1 =
                (PINSEL1 & ~(3 << (2 * (PinN - 16)))) |
                (FuncN << (2 * (PinN - 16)));
        }
    }

    else if(PortN == 1)
    {
        PINSEL2 =
            (PINSEL2 & ~(3 << (2 * (PinN - 16)))) |
            (FuncN << (2 * (PinN - 16)));
    }
}


/*==================================================
                    KEYPAD
==================================================*/

char kpmlut[4][4] =
{
    {'7','8','9','/'},
    {'4','5','6','*'},
    {'1','2','3','-'},
    {'C','0','=','+'}
};


void Init_kpm(void)
{
    /* Rows P1.16-P1.19 as output */
    IODIR1 |= (15 << ROW0);
}


u32 RowCheck(void)
{
    u32 rno;

    for(rno = 0; rno < 4; rno++)
    {
        IOPIN1 =
            (IOPIN1 & ~(15 << ROW0)) |
            ((~(1 << rno)) << ROW0);

        if(Colscan() == 0)
        {
            break;
        }
    }

    IOCLR1 = (15 << ROW0);

    return rno;
}


u32 ColCheck(void)
{
    u32 colno;

    for(colno = 0; colno < 4; colno++)
    {
        if(((IOPIN1 >> (COL0 + colno)) & 1) == 0)
        {
            break;
        }
    }

    return colno;
}


u32 Colscan(void)
{
    if(((IOPIN1 >> COL0) & 15) < 15)
    {
        return 0;
    }

    return 1;
}


u32 KeyScan(void)
{
    u32 rno;
    u32 cno;
    u32 key;

    while(Colscan());

    rno = RowCheck();

    cno = ColCheck();

    key = kpmlut[rno][cno];

    while(!Colscan());

    return key;
}


/*==================================================
                  NORMAL NUMBER INPUT
==================================================*/

u32 ReadNum(int digits)
{
    u32 num = 0;
    u32 count = 0;

    u8 key;

    while(1)
    {
        key = KeyScan();


        /* ENTER */
        if(key == '=')
        {
            if(count != 0)
            {
                break;
            }
        }


        /* BACKSPACE */
        else if(key == '*')
        {
            if(count > 0)
            {
                num = num / 10;

                count--;

                CmdLCD(0x10);

                CharLCD(' ');

                CmdLCD(0x10);
            }
        }


        /* Numeric key */
        else if((key >= '0') &&
                (key <= '9'))
        {
            if(count < digits)
            {
                num = num * 10 +
                      (key - '0');

                if(pf)
                    CharLCD('*');
                else
                    CharLCD(key);

                count++;
            }
        }
    }

    return num;
}


/*==================================================
                PASSWORD INPUT
==================================================*/

u32 ReadPassword(void)
{
    u32 password = 0;
    u32 count = 0;

    u8 key;

    while(1)
    {
        key = KeyScan();


        /* Number */
        if((key >= '0') &&
           (key <= '9'))
        {
            if(count < 3)
            {
                password =
                    password * 10 +
                    (key - '0');

                CharLCD('*');

                count++;
            }
        }


        /* Backspace */
        else if(key == '*')
        {
            if(count > 0)
            {
                password = password / 10;

                count--;

                CmdLCD(0x10);

                CharLCD(' ');

                CmdLCD(0x10);
            }
        }


        /* Enter */
        else if(key == '=')
        {
            if(count == 3)
            {
                return password;
            }
        }
    }
}


/*==================================================
                     RTC
==================================================*/

int dur;


void RTC_Init(void)
{
    CCR = RTC_RESET;

    PREINT  = PREINT1;
    PREFRAC = PREFRAC1;

    CCR = RTC_EN;
}


void SetRtcTime(void)
{
    s32 h;
    s32 m;
    s32 s;


    CmdLCD(CLEAR_LCD);
    StrLCD("Enter Hours");

    CmdLCD(0xC0);

    h = ReadNum(2);

    CmdLCD(0xC0);

    U32LCD(h);

    dly_ms(100);


    CmdLCD(CLEAR_LCD);
    StrLCD("Enter Min");

    CmdLCD(0xC0);

    m = ReadNum(2);

    CmdLCD(0xC0);

    U32LCD(m);

    dly_ms(100);


    CmdLCD(CLEAR_LCD);
    StrLCD("Enter Sec");

    CmdLCD(0xC0);

    s = ReadNum(2);

    CmdLCD(0xC0);

    U32LCD(s);

    dly_ms(100);


    if(h > 23)
        h = 0;

    if(m > 59)
        m = 0;

    if(s > 59)
        s = 0;


    HOUR = h;
    MIN  = m;
    SEC  = s;
}


void SetRtcDate(void)
{
    s32 day;
    s32 month;
    s32 year;


    CmdLCD(CLEAR_LCD);
    StrLCD("Enter Date:");

    CmdLCD(0xC0);

    day = ReadNum(2);

    CmdLCD(0xC0);
    U32LCD(day);

    dly_ms(100);


    CmdLCD(CLEAR_LCD);
    StrLCD("Enter Month:");

    CmdLCD(0xC0);

    month = ReadNum(2);

    CmdLCD(0xC0);
    U32LCD(month);

    dly_ms(100);


    CmdLCD(CLEAR_LCD);
    StrLCD("Enter Year:");

    CmdLCD(0xC0);

    year = ReadNum(4);

    CmdLCD(0xC0);
    U32LCD(year);

    dly_ms(100);


    DOM   = day;
    MONTH = month;
    YEAR  = year;
}


void GetRtcTime(s32 *h,
                s32 *m,
                s32 *s)
{
    *h = HOUR;
    *m = MIN;
    *s = SEC;
}


void GetRtcdate(s32 *d,
                s32 *m,
                s32 *y)
{
    *d = DOM;
    *m = MONTH;
    *y = YEAR;
}


void DispRtcTime(u32 h,
                 u32 m,
                 u32 s)
{
    CmdLCD(0x80);

    CharLCD((h / 10) + 48);
    CharLCD((h % 10) + 48);

    CharLCD(':');

    CharLCD((m / 10) + 48);
    CharLCD((m % 10) + 48);

    CharLCD(':');

    CharLCD((s / 10) + 48);
    CharLCD((s % 10) + 48);
}


void DispRtcDate(u32 d,
                 u32 m,
                 u32 y)
{
    CmdLCD(0xC0);

    CharLCD((d / 10) + 48);
    CharLCD((d % 10) + 48);

    CharLCD('/');

    CharLCD((m / 10) + 48);
    CharLCD((m % 10) + 48);

    CharLCD('/');

    U32LCD(y);
}


/*==================================================
                 TEMPERATURE
==================================================*/

void DispTemp(void)
{
    f32 tempc;

    tempc = Read_LM35DegC();

    CmdLCD(0x80 + 10);

    F32LCD(tempc, 1);

    CharLCD(223);

    CharLCD('C');
}


/*==================================================
                   DURATION
==================================================*/

void Duration(void)
{
    CmdLCD(CLEAR_LCD);

    StrLCD("Duration(min)");

    CmdLCD(0xC0);

    dur = ReadNum(2);


    if(dur == 0)
    {
        CmdLCD(CLEAR_LCD);

        StrLCD("Invalid Duration");

        dly_ms(1000);

        return;
    }


    CmdLCD(CLEAR_LCD);

    StrLCD("Duration Set");

    CmdLCD(0xC0);

    U32LCD(dur);

    StrLCD(" min");

    dly_ms(1000);
}


/*==================================================
                  RTC EDIT MENU
==================================================*/

void Rtc_edit(void)
{
    u8 op;


    while(1)
    {
        CmdLCD(CLEAR_LCD);

        StrLCD("1.Set Time");

        CmdLCD(0xC0);

        StrLCD("2.Date 3.Exit");


        op = KeyScan();


        switch(op)
        {
            case '1':
                SetRtcTime();
                break;


            case '2':
                SetRtcDate();
                break;


            case '3':
                return;
        }
    }
}


/*==================================================
                   START TIME
==================================================*/

void Start_time(void)
{
    CmdLCD(CLEAR_LCD);

    StrLCD("Start Hour");

    CmdLCD(0xC0);

    hr = ReadNum(2);


    if(hr > 23)
    {
        hr = 0;

        CmdLCD(CLEAR_LCD);

        StrLCD("Invalid Hour");

        dly_ms(1000);

        return;
    }


    CmdLCD(CLEAR_LCD);

    StrLCD("Start Minute");

    CmdLCD(0xC0);

    min = ReadNum(2);


    if(min > 59)
    {
        min = 0;

        CmdLCD(CLEAR_LCD);

        StrLCD("Invalid Minute");

        dly_ms(1000);

        return;
    }


    CmdLCD(CLEAR_LCD);

    StrLCD("Start Time Set");

    CmdLCD(0xC0);

    if(hr < 10)
        CharLCD('0');

    U32LCD(hr);

    CharLCD(':');

    if(min < 10)
        CharLCD('0');

    U32LCD(min);

    dly_ms(1000);
}


/*==================================================
                   PASSWORD
==================================================*/

void verifyPass(void)
{
    u32 password;

    const u32 ADMIN_PASSWORD = 654;


    CmdLCD(CLEAR_LCD);

    StrLCD("Password:");

    CmdLCD(0xC0);


    password = ReadPassword();


    if(password == ADMIN_PASSWORD)
    {
        admin_mode = 1;

        CmdLCD(CLEAR_LCD);

        StrLCD("Access Granted");

        dly_ms(1000);
    }
    else
    {
        admin_mode = 0;

        CmdLCD(CLEAR_LCD);

        StrLCD("Access Denied");

        dly_ms(1000);
    }
}


/*==================================================
                  ADMIN MENU
==================================================*/

void AdminMenu(void)
{
    u8 op;


    while(1)
    {
        CmdLCD(CLEAR_LCD);

        StrLCD("1.RTC 2.Start");

        CmdLCD(0xC0);

        StrLCD("3.Dur 4.Log 5.Exit");


        op = KeyScan();


        switch(op)
        {
            case '1':
                Rtc_edit();
                break;


            case '2':
                Start_time();
                break;


            case '3':
                Duration();
                break;


            case '4':
                View_Loginfo();
                break;


            case '5':
                return;
        }
    }
}


/*==================================================
                TWO DIGIT DISPLAY
==================================================*/

void Display2Digit(u32 value)
{
    if(value < 10)
        CharLCD('0');

    U32LCD(value);
}


/*==================================================
                    LOG INFO
==================================================*/

void View_Loginfo(void)
{
    if(log_available == 0)
    {
        CmdLCD(CLEAR_LCD);

        StrLCD("No Log Found");

        dly_ms(2000);

        return;
    }


    CmdLCD(CLEAR_LCD);

    StrLCD("S-");

    Display2Digit(prev_exam_start_hour);

    CharLCD(':');

    Display2Digit(prev_exam_start_min);

    StrLCD(" E-");

    Display2Digit(prev_exam_end_hour);

    CharLCD(':');

    Display2Digit(prev_exam_end_min);


    CmdLCD(0xC0);

    Display2Digit(prev_exam_start_day);

    CharLCD('/');

    Display2Digit(prev_exam_start_month);

    CharLCD('/');

    U32LCD(prev_exam_start_year);


    dly_ms(5000);

    CmdLCD(CLEAR_LCD);
}


/*==================================================
                     LED
==================================================*/

void Glow_Led(void)
{
    /* Turn OFF all LEDs */

    IOCLR1 =
        LED_GREEN |
        LED_YELLOW |
        LED_RED;


    if(cntdown > 10)
    {
        IOSET1 = LED_GREEN;
    }

    else if(cntdown > 1)
    {
        IOSET1 = LED_YELLOW;
    }

    else if(cntdown == 1)
    {
        IOSET1 = LED_RED;
    }
}


/*==================================================
                      ADC
==================================================*/

void Init_ADC(void)
{
    /* P0.28 -> AD0.1 */

    cfgPortPinFunc(0,28,1);

    ADCR =
        PDN_BIT |
        CLKDIV_VALUE;
}


void Read_ADC(u32 channel,
              u32 *adc_value,
              f32 *voltage)
{
    /* Select channel */

    ADCR &= ~(255 << 0);

    ADCR |= channel;


    /* Start conversion */

    ADCR |= START_CONV;


    /* Wait until conversion complete */

    while(((ADDR >> DONE_BIT) & 1) == 0);


    /* Stop conversion */

    ADCR &= ~START_CONV;


    /* 10-bit result */

    *adc_value =
        (ADDR >> RESULT) & 1023;


    /* Calculate voltage */

    *voltage =
        (3.3 * (*adc_value)) / 1024.0;
}


f32 Read_LM35DegC(void)
{
    u32 adc_value;

    f32 voltage;


    Read_ADC(CH1,
             &adc_value,
             &voltage);


    /* LM35 = 10mV / degree C */

    return voltage * 100;
}


/*==================================================
                  7-SEGMENT
==================================================*/

cs8 seg_lut[10] =
{
    0xC0,
    0xF9,
    0xA4,
    0xB0,
    0x99,
    0x92,
    0x82,
    0xF8,
    0x80,
    0x98
};


void Init_7segs(void)
{
    /* Segment pins P0.0-P0.7 */

    IODIR0 |= 0xFF;


    /* Digit select pins */

    IODIR0 |=
        (3 << DSEL1);
}


void disp_2mux_7segs(int n)
{
    if(n < 0)
        n = 0;

    if(n > 99)
        n = 99;


    /* Tens digit */

    IOPIN0 =
        (IOPIN0 & ~(0xFF << 0)) |
        (seg_lut[n / 10] << 0);

    IOSET0 = (1 << DSEL1);

    dly_ms(1);

    IOCLR0 = (1 << DSEL1);


    /* Units digit */

    IOPIN0 =
        (IOPIN0 & ~(0xFF << 0)) |
        (seg_lut[n % 10] << 0);

    IOSET0 = (1 << DSEL2);

    dly_ms(1);

    IOCLR0 = (1 << DSEL2);
		
}
/*-------------------Interrupts--------------------------------*/
void EINT0_ISR(void) __irq
{
    EXTINT = (1 << 0);

    password_request = 1;

    VICVectAddr = 0;
}


void enable_eint0(void)
{
    /* P0.16 -> EINT0 */
    cfgPortPinFunc(0,16,1);

    /* EINT0 as IRQ */
    VICIntSelect &= ~(1 << EINT0_CH);

    /* Clear pending */
    EXTINT = (1 << 0);

    /* Edge sensitive */
    EXTMODE |= (1 << 0);

    /* Rising edge */
    EXTPOLAR |= (1 << 0);

    /* Vector */
    VICVectAddr0 =
        (unsigned int)EINT0_ISR;

    VICVectCntl0 =
        (1 << 5) | EINT0_CH;

    /* Enable */
    VICIntEnable |= (1 << EINT0_CH);
}
void EINT1_ISR(void) __irq
{
    EXTINT = (1 << 1);

    if(exam_started)
    {
        pause_flag ^= 1;

        if(pause_flag)
        {
            pause_count++;
        }
    }

    VICVectAddr = 0;
}


void enable_eint1(void)
{
    /* P0.14 -> EINT1 */
    cfgPortPinFunc(0,30,2);

    /* EINT1 as IRQ */
    VICIntSelect &= ~(1 << EINT1_CH);

    /* Clear pending interrupt */
    EXTINT = (1 << 1);

    /* Edge sensitive */
    EXTMODE |= (1 << 1);

    /* Falling edge */
    EXTPOLAR &= ~(1 << 1);

    /* Vector */
    VICVectAddr1 = (unsigned int)EINT1_ISR;

    VICVectCntl1 =
        (1 << 5) | EINT1_CH;

    /* Enable */
    VICIntEnable |= (1 << EINT1_CH);
}