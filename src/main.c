#include <lpc21xx.h>

#include "header.h"
#include "defines.h"


/*==================================================
                GLOBAL FLAGS
==================================================*/

/*
   EINT0:
   Requests password authentication
*/


volatile int password_request = 0;


/*
   1 = administrator authenticated
   0 = normal user
*/

volatile int admin_mode = 0;


/*
   Examination status

   1 = exam running
   0 = exam not running
*/

volatile int exam_started = 0;


/*
   Countdown pause status

   0 = running
   1 = paused
*/

volatile int pause_flag = 0;


/*
   Number of pause operations
*/

volatile int pause_count = 0;


/*
   Remaining examination time
   Unit = minutes
*/

volatile int cntdown = 0;


/*
   Used to detect a new RTC minute
*/

volatile int last_minute = -1;


/*==================================================
              EXAMINATION SETTINGS
==================================================*/

/*
   Configured examination start time
*/

int hr=0;
int min=0;


/*
   Examination duration in minutes
*/

extern int dur;


/*==================================================
                    LOG DATA
==================================================*/

int prev_exam_start_hour = -1;
int prev_exam_start_min  = -1;
int prev_exam_start_sec  = -1;

int prev_exam_start_day   = -1;
int prev_exam_start_month = -1;
int prev_exam_start_year  = -1;

int prev_exam_end_hour = -1;
int prev_exam_end_min  = -1;


/*
   0 = no log
   1 = log available
*/

int log_available = 0;


/*==================================================
                       MAIN
==================================================*/

int main(void)
{
    s32 h;
    s32 m;
    s32 s;

    s32 d;
    s32 mon;
    s32 y;


    /*================================================
                    INITIALIZATION
    =================================================*/

    InitLCD();

    RTC_Init();

    Init_kpm();

    Init_ADC();

    Init_7segs();


    /*================================================
                       LEDs
    =================================================*/

    IODIR1 |=
        LED_GREEN |
        LED_YELLOW |
        LED_RED;

    IOCLR1 =
        LED_GREEN |
        LED_YELLOW |
        LED_RED;


    /*================================================
                      BUZZER
    =================================================*/

    IODIR1 |= BUZZER;

    IOCLR1 = BUZZER;


    /*================================================
                  EXTERNAL INTERRUPTS
    =================================================*/

    /*
       Switch-1 -> EINT0
       Used for administrator/password request
    */

    enable_eint0();


    /*
       Switch-2 -> EINT1
       Used for pause/resume
    */

    enable_eint1();


    /*================================================
                   NORMAL MODE
    =================================================*/

    CmdLCD(CLEAR_LCD);


    /*================================================
                    MAIN LOOP
    =================================================*/

    while(1)
    {

        /*--------------------------------------------
             1. CHECK EINT0 PASSWORD REQUEST
        --------------------------------------------*/

        if(password_request)
        {
            /*
               Clear request first
            */

            password_request = 0;


            /*
               Ask for administrator password
            */

            verifyPass();


            /*
               Password correct
            */

            if(admin_mode)
            {
                /*
                   Enter administrator menu
                */

                AdminMenu();


                /*
                   When administrator exits,
                   return to normal mode
                */

                admin_mode = 0;

                CmdLCD(CLEAR_LCD);
            }
        }


        /*--------------------------------------------
                 2. READ CURRENT RTC
        --------------------------------------------*/

        GetRtcTime(&h,
                   &m,
                   &s);


        GetRtcdate(&d,
                   &mon,
                   &y);


        /*--------------------------------------------
              3. NORMAL MONITORING DISPLAY
        --------------------------------------------*/

       // if(!exam_started)
       // {
            /*
               First line:
               RTC time + temperature
            */

            DispRtcTime(h,m,s);

            DispTemp();


            /*
               Second line:
               Date
            */

            DispRtcDate(d,mon,y);
      //  }


        /*--------------------------------------------
              4. CHECK EXAMINATION START
        --------------------------------------------*/

        if((exam_started == 0) &&
           (dur > 0) &&
           (h == hr) &&
           (m == min) &&
           (s == 0))
        {
            /*
               Start examination
            */

            exam_started = 1;


            /*
               Load configured duration
            */

            cntdown = dur;


            /*
               Save current minute
            */

            last_minute = m;


            /*
               Save examination start time
            */

            prev_exam_start_hour = h;

            prev_exam_start_min = m;

            prev_exam_start_sec = s;


            /*
               Save examination start date
            */

            prev_exam_start_day = d;

            prev_exam_start_month = mon;

            prev_exam_start_year = y;


            /*
               Reset pause information
            */

            pause_flag = 0;

            pause_count = 0;


            /*
               New exam means previous log
               is no longer the current log
            */

            log_available = 0;
        }


        /*--------------------------------------------
                 5. EXAMINATION RUNNING
        --------------------------------------------*/

        if(exam_started)
        {

            /*----------------------------------------
                  Display countdown
            ----------------------------------------*/

            disp_2mux_7segs(cntdown);


            /*----------------------------------------
                     LED indication
            ----------------------------------------*/

            Glow_Led();


            /*----------------------------------------
                   COUNTDOWN OPERATION
            ----------------------------------------*/

            if(!pause_flag)
            {
                /*
                   RTC minute changed
                */

                if((m != last_minute) &&
                   (cntdown > 0))
                {
                    last_minute = m;

                    cntdown--;
                }
            }

            else
            {
                /*
                   While paused, synchronize the
                   reference minute.

                   This prevents an immediate
                   decrement when resumed.
                */

                last_minute = m;
            }


            /*----------------------------------------
                   EXAMINATION COMPLETED
            ----------------------------------------*/

            if(cntdown == 0)
            {
                /*
                   Save examination end time
                */

                prev_exam_end_hour = h;

                prev_exam_end_min = m;


                /*
                   Log now available
                */

                log_available = 1;


                /*
                   Examination stopped
                */

                exam_started = 0;

                pause_flag = 0;


                /*
                   Turn OFF LEDs
                */

                IOCLR1 =
                    LED_GREEN |
                    LED_YELLOW |
                    LED_RED;


                /*
                   Turn ON buzzer
                */

                IOSET1 = BUZZER;


                /*
                   Display completion message
                */

                CmdLCD(CLEAR_LCD);

                StrLCD("Exam Completed");


                dly_ms(2000);


                /*
                   Turn OFF buzzer
                */

                IOCLR1 = BUZZER;


                CmdLCD(CLEAR_LCD);
            }
        }


        /*--------------------------------------------
                  6. EXAM NOT RUNNING
        --------------------------------------------*/

        else
        {
            /*
               Turn OFF all LEDs
            */

            IOCLR1 =
                LED_GREEN |
                LED_YELLOW |
                LED_RED;
        }
    }
}