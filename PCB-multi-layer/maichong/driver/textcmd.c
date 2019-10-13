/******************** (C) COPYRIGHT 2018 CETC27-CETIIC-ZYGD ********************
* File Name          : textcmd.c
* Author             : WZJ
* Version            : V1.0
* Date               : 2018/11/29
* Description        :
*******************************************************************************/
#include "textcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ad5245.h"

#define cmd_num (6)

enum cmd_words {HELP, PWM, RESISTANCE, VOLTAGE, GAIN, THRESHOLD} cmd_word;
static char cmd_word_str[cmd_num][12] = {"HELP", "PWM", "RESISTANCE", "VOLTAGE", "GAIN", "THRESHOLD"};
char textcmd_buf[textcmd_length];
//20190510
vu16 textcmd_cnt;
extern float pwm_freq, pwm_duty;
extern float voltage_target;
extern float gain;
extern u8 PAS_down, PAS_up;
vu8 termination_flag;
u16 resistance;
extern void Tim3_pwm_config(unsigned short arr, unsigned short psc);

char *str_upr(char *s1)
{
    char *temp;

    for (temp = s1; *s1 != '\0'; s1++)
    {
        if (*s1 > 96 && *s1 < 123)
        {
            *s1 = *s1 - 32;
        }
    }
    s1 = temp;
    return s1;
}
void text_parse(char *s1)
{
    char *PT;
    u8 index;
    u16 tmp_u16;

    s1 = str_upr(s1);

    for (index = 0; index < cmd_num; index++)
    {
        PT = strstr(s1, cmd_word_str[index]);
        if (PT != 0)break;
    }
    if (index < cmd_num)
    {
        cmd_word = (enum cmd_words)index;
        switch (cmd_word)
        {
            case HELP:
            {
                printf("HELP,VOLTAGE,GAIN,THRESHOLD,PWM,RESISTANCE");
                break;
            }
            case PWM:
            {
                PT += 3;
                if (*PT == '\r')
                {
                    printf("%.1f kHz,%.1f %%\r\n", pwm_freq, pwm_duty);
                }
                else
                {
                    PT++;
                    pwm_freq = atof(PT);
                    PT = strstr(PT, ",");
                    PT++;
                    pwm_duty = atof(PT);
                    if (pwm_freq >= 1 && pwm_duty >= 0)
                    {
                        Tim3_pwm_config(999, 36000 / pwm_freq - 1); //PWMÆµÂÊ 36000/(999+1)=36 kHz
                        TIM_SetCompare2(TIM3, pwm_duty * 10);
                        printf("<T>\r\n");
                    }
                    else
                    {
                        printf("<F>\r\n");
                    }
                }
                break;
            }
            case RESISTANCE:
            {
                PT += 10;
                if ((*PT == '\r' && *(PT + 1) == '\n'))
                {
                    printf("%d ohm\r\n", resistance);
                }
                else
                {
                    PT++;
                    resistance = atoi(PT);
                    tmp_u16 = (resistance - 2 * 50) * 256 / 10000;
                    ad5245_write(tmp_u16);
                    printf("<T>\r\n");
                }
                break;
            }
            case VOLTAGE:
            {
                PT += 7;
                if ((*PT == '\r' && *(PT + 1) == '\n'))
                {
                    printf("%f v\r\n", voltage_target);
                }
                else
                {
                    PT++;
                    voltage_target = atof(PT);
                    printf("<T>\r\n");
                }
                break;
            }
            case GAIN:
            {
                PT += 4;
                if ((*PT == '\r' && *(PT + 1) == '\n'))
                {
                    printf("%f v\r\n", gain);
                }
                else
                {
                    PT++;
                    gain = atof(PT);
                    if(gain>20)
                    {
                        printf("<F>\r\n");
                    }else
                    {
                        resistance = 500*gain;
                        tmp_u16 = (resistance - 2 * 50) * 256 / 10000;
                        ad5245_write(tmp_u16);
                        printf("<T>\r\n");
                    }
                }
                break;
            }
            case THRESHOLD:
            {
                PT += 9;
                if (*PT == '\r')
                {
                    printf("%d ,%d \r\n", PAS_down, PAS_up);
                }
                else
                {
                    PT++;
                    PAS_down = atoi(PT);
                    PT = strstr(PT, ",");
                    PT++;
                    PAS_up = atof(PT);
                    printf("<T>\r\n");
                }
                break;
            }
            default:
            {
                printf("<BADCOMMAND>\r\n") ;
                break;
            }
        }
    }
    else
    {
        printf("<BADCOMMAND>\r\n") ;
    }
}
void text_cmd(void)
{
    u16 i;

    text_parse(textcmd_buf);
    for (i = 0; i < textcmd_length; i++)textcmd_buf[i] = 36;
    termination_flag = 0;
}

/******************** (C) COPYRIGHT 2018 CETC27-CETIIC-ZYGD ********************/
