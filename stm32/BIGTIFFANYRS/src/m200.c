#include <libmfwtimer.h>
#include "m200.h"
#include "hal.h"
#include "nbfi.h"
#include "nbfi_config.h"
#include <time.h>
//uint32_t    M200_add = 0xedc14;

struct wtimer_desc __xdata m200_regular_desc;
struct wtimer_desc __xdata m200_active_desc;
struct wtimer_desc __xdata m200_fastdl_desc;
struct wtimer_desc __xdata m200_sinchro_desc;
struct wtimer_desc __xdata m200_radiotest_desc;
struct wtimer_desc __xdata m200_write_tarif_desc;
struct wtimer_desc __xdata m200_everysec_desc;

void SendRegular(struct wtimer_desc __xdata *desc);
void ResetActive(struct wtimer_desc __xdata *desc);
void ResetFastDl(struct wtimer_desc __xdata *desc);
void TimeSinchro(struct wtimer_desc __xdata *desc);
void RadioTest(struct wtimer_desc __xdata *desc);
void EverySec(struct wtimer_desc __xdata *desc);

bool sw_free = 1;
struct tm time_st;
uint32_t __xdata LastTime;
uint32_t __xdata LastHourTime;
uint32_t __xdata LastDayTime;
uint32_t __xdata LastWeekTime;
uint32_t __xdata LastMonthTime;

uint32_t __xdata dltime;

uint8_t __xdata tmpbuf[64];
uint8_t __xdata lasthourbuf[30];
uint8_t __xdata lastdaybuf[30];
uint8_t __xdata lastweekbuf[30];
uint8_t __xdata lastmonthbuf[30];
uint8_t __xdata certaindaybuf[30];
uint8_t __xdata certaindaybuf_aux[30];
uint8_t __xdata m200_comm_number = 0xFF;

uint8_t serial_is_changed = 0;

uint8_t regular_state = 0;
uint8_t sinchro_state = 0;
uint8_t write_tarif_state = 0, write_tarif_parity = 0;

TARIF_ZONE * tarif_mode;

uint32_t regular_counter = 1;
uint32_t regular_counter_aux = 1;
uint8_t write_tarif_try_num;

uint8_t display_mode = 0;

uint8_t watch_data[32];

TARIF_ZONE onetarif[] =
{
    {T1, 0, 0},
    {END, 0, 0}
};

enum
{
    CMD_METER_LINK_TEST = 0x00,
    CMD_METER_GET_ACCESS = 0x01,
    CMD_METER_WRITE_PARAM = 0x03,
    CMD_METER_READ_TIME = 0x04,
    CMD_METER_READ_ACCUM = 0x05,
    CMD_METER_ULTRA_WRITE_PARAM = 0x07,
    CMD_METER_READ_OTHER = 0x08,
};

__reentrant uint16_t bcd2dec_r(uint16_t bcd)
{
    return bcd ? (bcd2dec_r(bcd>>4)*10) + (bcd & 0x0f) : 0;
}

uint8_t dec2bcd(uint8_t n)
{
    return (n / 10 * 16 +  n % 10);
}

void M200_Init()
{
    M200_ReadSettings();
    ScheduleTask(&m200_regular_desc, &SendRegular, RELATIVE, SECONDS(10));
    ScheduleTask(&m200_sinchro_desc, &TimeSinchro, RELATIVE, SECONDS(5));
}

void cpy_timestamp(void)
{
    time_st.tm_sec = bcd2dec_r(m200_rxbuf.payload[1]);
    time_st.tm_min = bcd2dec_r(m200_rxbuf.payload[2]);
    time_st.tm_hour = bcd2dec_r(m200_rxbuf.payload[3]);
    time_st.tm_wday = bcd2dec_r(m200_rxbuf.payload[4]);
    time_st.tm_mday = bcd2dec_r(m200_rxbuf.payload[5]);
    time_st.tm_mon = bcd2dec_r(m200_rxbuf.payload[6]) - 1;
    time_st.tm_year = bcd2dec_r(m200_rxbuf.payload[7]) + 100;
    time_st.tm_isdst = bcd2dec_r(m200_rxbuf.payload[8]);
    LastTime = mktime(&time_st);
}

void M200_Receive_Success()
{
    if(serial_is_changed)
        serial_is_changed = 2;

    if(m200_comm_number != 0xFF)
    {
        if (m200_comm_number == 0xFE)
        {
            M200_Send_Event(0x09, dltime, 0);
            m200_comm_number = 0xFF;
        }
        else
        {
            tmpbuf[0] = 210;
            for(uint8_t i = 0; i != m200_rxbuf.len - 3; i++)
                tmpbuf[i + 1] = m200_rxbuf.payload[1 + i];
            NBFi_Send(tmpbuf, m200_rxbuf.len - 2);
            m200_comm_number = 0xFF;
        }
        return;
    }

    switch(write_tarif_state)
    {
    case 1:
        write_tarif_state = 101;
        write_tarif_try_num = 0;
        ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, MILLISECONDS(5));
        break;
    case 120:
        if (m200_rxbuf.payload[1] == 0)
            write_tarif_state = 121;
        break;
    case 101:
        write_tarif_state = 2;
        write_tarif_try_num = 0;
        ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, MILLISECONDS(5));
        break;
    default:
        if (write_tarif_state >= 2 && write_tarif_state < 98)
        {
            if (m200_rxbuf.payload[1] == 0)
            {
                if (write_tarif_parity)
                {
                    write_tarif_parity = 0;
                    write_tarif_state++;
                }
                else
                    write_tarif_parity = 1;

                write_tarif_try_num = 0;
            }
            ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, MILLISECONDS(5));
        }
        break;
    }

    switch (regular_state)
    {
    case 20:
    {
        if (m200_rxbuf.payload[1] == 0)
            regular_state = 21;
        break;
    }
    case 1:
    {
        if (m200_rxbuf.len == 11)
        {
            cpy_timestamp();
            regular_state = 2;
        }
        break;
    }
    case 3:
        regular_state = 0;
        M200_Report_Regular(0);
        ScheduleTask(&m200_regular_desc, &SendRegular, RELATIVE, SECONDS(10));
        break;
    }

    switch (sinchro_state)
    {
    case 20:
    {
        if (m200_rxbuf.payload[1] == 0)
            sinchro_state = 21;
        break;
    }
    case 1:
    {
        if (m200_rxbuf.len == 11)
        {
            cpy_timestamp();
            sinchro_state = 2;
        }
        break;
    }
    case 3:
        sinchro_state = 0;
        M200_Report_Regular(0);
        ScheduleTask(&m200_sinchro_desc, &TimeSinchro, RELATIVE, (3600-(LastTime%(3600)))*SECONDS(1));
        break;
    }

}

void M200_Receive_Timeout()
{
    if(m200_comm_number != 0xFF)
    {
        if(m200_comm_number == 0x02)
            m200_comm_number = 245;
        M200_Send_Event(200, 0, m200_comm_number*256);
        m200_comm_number = 0xFF;
    }
}

void SendRegular(struct wtimer_desc __xdata *desc)
{
    static uint8_t try_num;
    switch(regular_state)
    {
    case 0: //start
        if(!(regular_counter%m200_settings.regular_period))
        {
            switch(m200_settings.read_mode)
            {
                case ATENDOFHOUR:
                    if(lasthourbuf[0] == 0x31)
                        NBFi_Send(lasthourbuf, 29);
                    else
                        M200_Send_Event(200, 0, 0x31*256);
                    lasthourbuf[0] = 0;
                    break;
                case ATENDOFDAY:
                    if(lastdaybuf[0] == 0x31)
                        NBFi_Send(lastdaybuf, 29);
                    else
                        M200_Send_Event(200, 0, 0x31*256);
                    lastdaybuf[0] = 0;
                    break;
                case ATENDOFWEEK:
                    if(lastweekbuf[0] == 0x31)
                        NBFi_Send(lastweekbuf, 29);
                    else
                        M200_Send_Event(200, 0, 0x31*256);
                    //lastweekbuf[0] = 0;
                    break;
                case ATENDOFMONTH:
                    if(lastmonthbuf[0] == 0x31)
                        NBFi_Send(lastmonthbuf, 29);
                    else
                        M200_Send_Event(200, 0, 0x31*256);
                    //lastmonthbuf[0] = 0;
                    break;
                default:
                    if((m200_settings.read_mode&0x80) == ATCERTAINDAY)
                    {
                        if(certaindaybuf[0] == 0x31)
                            NBFi_Send(certaindaybuf, 29);
                        else
                            M200_Send_Event(200, 0, 0x31*256);
                        certaindaybuf[0] = 0;
                    }

                    break;
            }
        }
        if(!(regular_counter_aux++%m200_settings.regular_period_aux))
        {
            switch(m200_settings.read_mode_aux)
            {
                case ATENDOFDAY:
                    if(lastdaybuf[0] == 0x31)
                        NBFi_Send(lastdaybuf, 29);
                    else
                        M200_Send_Event(200, 0, 0x31*256);
                    //lastdaybuf[0] = 0;
                    break;
                case ATENDOFWEEK:
                    if(lastweekbuf[0] == 0x31)
                        NBFi_Send(lastweekbuf, 29);
                    else
                        M200_Send_Event(200, 0, 0x31*256);
                    //lastweekbuf[0] = 0;
                    break;
                case ATENDOFMONTH:
                    if(lastmonthbuf[0] == 0x31)
                        NBFi_Send(lastmonthbuf, 29);
                    else
                        M200_Send_Event(200, 0, 0x31*256);
                    //lastmonthbuf[0] = 0;
                    break;
                default:
                    if((m200_settings.read_mode_aux&0x80) == ATCERTAINDAY)
                    {
                        if(certaindaybuf_aux[0] == 0x31)
                            NBFi_Send(certaindaybuf_aux, 29);
                        else
                            M200_Send_Event(200, 0, 0x31*256);
                        certaindaybuf_aux[0] = 0;
                    }
                    break;
            }
        }

        if((regular_counter++%m200_settings.regular_period) || (m200_settings.read_mode != ATONCE))
            break;
    case 5:
        if(sinchro_state||((write_tarif_state > 0)&&(write_tarif_state < 99)))
        {
            ScheduleTask(&m200_regular_desc, &SendRegular, RELATIVE, SECONDS(1));
            return;
        }
        regular_state = 20;
        try_num = 0;
    case 20:
        if(++try_num <= 3)
        {
            uint8_t data[10];
            data[0] = 2;
            for(uint8_t i = 0; i < 6; i++)
                data[i + 1] = 0x02;
            M200_Send(CMD_METER_GET_ACCESS, data, 7);
        }
        else
        {
            M200_Send_Event(200, 0, 0x31*256);
            regular_state = 0;
            break;
        }
        ScheduleTask(&m200_regular_desc, &SendRegular, RELATIVE, SECONDS(1));
        return;
    case 21:
        try_num = 0;
        regular_state = 1;
    case 1:
        if(++try_num <= 3)
        {
            uint8_t data[10];
            data[0] = 0;
            M200_Send(CMD_METER_READ_TIME, data, 1);
        }
        else
        {
            M200_Send_Event(200, 0, 0x31*256);
            regular_state = 0;
            break;
        }
        ScheduleTask(&m200_regular_desc, &SendRegular, RELATIVE, SECONDS(1));
        return;
    case 2:
        try_num = 0;
        regular_state = 3;
    case 3:
        if(++try_num < 3)
        {
            uint8_t data[10];
            data[0] = 0;
            data[1] = 6;
            M200_Send(CMD_METER_READ_ACCUM, data, 2);
        }
        else
        {
            M200_Send_Event(200, 0, 0x31*256);
            regular_state = 0;
            break;
        }
        ScheduleTask(&m200_regular_desc, &SendRegular, RELATIVE, SECONDS(1));
        return;
    case 4:
        break;
    }

    if(NBFi_Is_Mode_Normal())
    {
        static uint8_t tries = 0;
        if(serial_is_changed == 2)
        {
            serial_is_changed = 0;
            tmpbuf[0] = 0xEE;
            tmpbuf[1] = 0x01;
            uint32_t serial = m200_settings.rs_add;
            uint32_t k = 100000000;

            for(uint8_t i = 2; i != 7; i++ )
            {
                tmpbuf[i] =  dec2bcd(serial/k);
                serial %= k;
                k /= 100;
            }
            NBFi_Send(tmpbuf, 7);
        }
        else if(serial_is_changed == 1)
        {
            if(++tries > 3)
            {
                M200_Send_Event(200, 0, 256*0x20);
                serial_is_changed = 0;
            }
            else
            {
                uint8_t buf[10];
                buf[0] = 0;
                M200_Send(CMD_METER_READ_OTHER, buf, 1);
            }
        }
        else
            tries = 0;

    }
    ScheduleTask(&m200_regular_desc, &SendRegular, RELATIVE, SECONDS(10));
}

void TimeSinchro(struct wtimer_desc __xdata *desc)
{
    static uint8_t try_num;

    switch(sinchro_state)
    {
    case 0:
        if(regular_state||((write_tarif_state > 0)&&(write_tarif_state < 99)))
        {
            ScheduleTask(&m200_sinchro_desc, &TimeSinchro, RELATIVE, SECONDS(10));
            return;
        }
        sinchro_state = 20;
        try_num = 0;
    case 20:
        if(++try_num <= 3)
        {
            uint8_t data[10];
            data[0] = 2;
            for(uint8_t i = 0; i < 6; i++)
                data[i + 1] = 0x02;
            M200_Send(CMD_METER_GET_ACCESS, data, 7);
        }
        else
        {
            sinchro_state = 0;
            break;
        }
        ScheduleTask(&m200_sinchro_desc, &TimeSinchro, RELATIVE, SECONDS(1));
        return;
    case 21:
        try_num = 0;
        sinchro_state = 1;
    case 1:
        if(++try_num <= 3)
        {
            uint8_t data[10];
            data[0] = 0;
            M200_Send(CMD_METER_READ_TIME, data, 1);
        }
        else
        {
            sinchro_state = 0;
            break;
        }
        ScheduleTask(&m200_sinchro_desc, &TimeSinchro, RELATIVE, SECONDS(1));
        return;
    case 2:
        try_num = 0;
        sinchro_state = 3;
    case 3:
        if(++try_num < 3)
        {
            uint8_t data[10];
            data[0] = 0;
            data[1] = 6;
            M200_Send(CMD_METER_READ_ACCUM, data, 2);
        }
        else
        {
            sinchro_state = 0;
            break;
        }
        ScheduleTask(&m200_sinchro_desc, &TimeSinchro, RELATIVE, SECONDS(1));
        return;
    }
    ScheduleTask(&m200_sinchro_desc, &TimeSinchro, RELATIVE, SECONDS(3600));
}

long int  _timezone;
long int  _dstbias;

bool M200_isCanSleep()
{
    return rs_free&sw_free;
}

uint32_t m230_to_uint32(uint8_t __xdata *buf)
{
    uint32_t val = 0;
    val |= (uint32_t)buf[0] << 16;
    val |= (uint32_t)buf[1] << 24;
    val |= (uint32_t)buf[2] << 0;
    val |= (uint32_t)buf[3] << 8;
    return val;
}

void M200_Report_Regular(uint8_t __xdata *buf)
{
    uint8_t index = 1;
    uint8_t __xdata *tbuf;
    if(buf)
        tbuf = buf;
    else
        tbuf = tmpbuf;
    tbuf[0] = 0x31;
    PARAM_IDENTIFIER param;
    param.timestamp_flag = 1;
    param.obis_A = 0;
    param.obis_B = 0;
    param.obis_C = 1;
    param.obis_D = 1;
    for(uint8_t i = 0; i != 4; i++)
    {
        param.obis_E = i + 1;
        param.timestamp_flag = (i == 0);
        *((uint16_t*)(&tbuf[index])) = *((uint16_t*)(&param));
        index += 2;
        if(!i)
        {
            tbuf[index++] = (LastTime >> 24) & 0xFF;
            tbuf[index++] = (LastTime >> 16) & 0xFF;
            tbuf[index++] = (LastTime >> 8) & 0xFF;
            tbuf[index++] = (LastTime >> 0) & 0xFF;
        }

        uint32_t val = m230_to_uint32(&m200_rxbuf.payload[1 + i * 16]);
        tbuf[index++] = (val >> 24) & 0xFF;
        tbuf[index++] = (val >> 16) & 0xFF;
        tbuf[index++] = (val >> 8) & 0xFF;
        tbuf[index++] = (val >> 0) & 0xFF;
    }
    if(buf == 0)
        NBFi_Send(tmpbuf, index);
}

void M200_Stamp()
{
    if(LastHourTime/3600 != LastTime/3600)
    {
        M200_Report_Regular(lasthourbuf);
        LastHourTime = LastTime;
        if(time_st.tm_hour == 0)
        {
            M200_Report_Regular(lastdaybuf);
            LastDayTime = LastTime;

            if(m200_settings.read_mode>=0x80)
            {
                if(time_st.tm_mday == (((uint8_t)m200_settings.read_mode)&0x7f))
                {
                    M200_Report_Regular(certaindaybuf);
                    if((m200_settings.regular_period - (regular_counter%m200_settings.regular_period)) >  (360*20) )
                    {
                        regular_counter = m200_settings.regular_period - (regular_counter%(360*20));
                    }
                }
            }

            if(m200_settings.read_mode_aux >= 0x80)
            {
                if(time_st.tm_mday == (((uint8_t)(m200_settings.read_mode_aux))&0x7f))
                {
                    M200_Report_Regular(certaindaybuf_aux);
                    if((m200_settings.regular_period_aux - (regular_counter_aux%m200_settings.regular_period_aux)) >  (360*20) )
                    {
                        regular_counter_aux = m200_settings.regular_period_aux - (regular_counter_aux%(360*20));
                    }
                }
            }

            if(time_st.tm_wday == 1)
            {
                M200_Report_Regular(lastweekbuf);
                LastWeekTime = LastTime;

                if((m200_settings.regular_period >= 60480)&&(m200_settings.regular_period <= 241920))
                {
                    if((m200_settings.regular_period - (regular_counter%m200_settings.regular_period)) >  (360*20) )
                    {
                        regular_counter = m200_settings.regular_period - (regular_counter%(360*20));
                    }
                }
                if((m200_settings.regular_period_aux >= 60480)&&(m200_settings.regular_period_aux <= 241920))
                {
                    if((m200_settings.regular_period_aux - (regular_counter_aux%m200_settings.regular_period_aux)) >  (360*20) )
                    {
                        regular_counter_aux = m200_settings.regular_period_aux - (regular_counter_aux%(360*20));
                    }
                }

            }else lastweekbuf[0] = 0;

            if(time_st.tm_mday == 1)
            {
                M200_Report_Regular(lastmonthbuf);
                LastMonthTime = LastTime;
                if(m200_settings.regular_period >= 267840)
                {
                    if((m200_settings.regular_period - (regular_counter%m200_settings.regular_period)) >  (360*24*5) )
                    {
                        regular_counter = m200_settings.regular_period - (regular_counter%(360*24*5));
                    }
                }
                if(m200_settings.regular_period_aux >= 267840)
                {
                    if((m200_settings.regular_period_aux - (regular_counter_aux%m200_settings.regular_period_aux)) >  (360*24*5) )
                    {
                        regular_counter_aux = m200_settings.regular_period_aux - (regular_counter_aux%(360*24*5));
                    }
                }
            }else if(time_st.tm_mday > 5) lastmonthbuf[0] = 0;

        }//else lastdaybuf[0] = 0;
    }
}

void bigendian_cpy(uint8_t __xdata* from, uint8_t __xdata* to, uint8_t len);

void M200_RX_Callback(uint8_t __generic* data, uint16_t length)
{
    uint8_t i;

    if(CheckTask(&m200_fastdl_desc))
    {
        ScheduleTask(&m200_fastdl_desc, &ResetFastDl, RELATIVE, SECONDS(60*2));
    }

    switch(data[0])
    {
    case 200:   //set add
        if(length == 5)
        {
            bigendian_cpy((uint8_t __xdata*)&data[1],(uint8_t __xdata*)&m200_settings.rs_add, 4);
            serial_is_changed = 1;

            M200_WriteSettings();
        }
        else if(length == 1)
        {
            tmpbuf[0] = 200;
            bigendian_cpy( (uint8_t __xdata*)&m200_settings.rs_add, (uint8_t __xdata*)(&tmpbuf[1]), 4);
            NBFi_Send(tmpbuf, 5);
        }
        break;
    case 201:   //set add
        if(length == 11)
        {
            bigendian_cpy( (uint8_t __xdata*)&data[1], (uint8_t __xdata*)&m200_settings.regular_period, 4);
            m200_settings.read_mode = data[5];
            bigendian_cpy( (uint8_t __xdata*)&data[6], (uint8_t __xdata*)&m200_settings.regular_period_aux, 4);
            m200_settings.read_mode_aux = data[10];
            M200_WriteSettings();
        }
        else if(length == 1)
        {
            tmpbuf[0] = 201;
            bigendian_cpy((uint8_t __xdata*)&m200_settings.regular_period, (uint8_t __xdata*)&tmpbuf[1], 4);
            tmpbuf[5] = m200_settings.read_mode;
            bigendian_cpy((uint8_t __xdata*)&m200_settings.regular_period_aux, (uint8_t __xdata*)&tmpbuf[6], 4);
            tmpbuf[10] = m200_settings.read_mode_aux;
            NBFi_Send(tmpbuf, 11);
        }
        break;
    case 202:
        if(length == 2)
        {
            if((data[1] >= 1) && (data[1] <= 3))
            {
                switch(data[1])
                {
                    case 1:
                        tarif_mode = onetarif;
                        m200_settings.t_mode = 1;
                        break;
                    case 2:
                        tarif_mode = m200_settings.twotarifs;
                        m200_settings.t_mode = 2;
                        break;
                    case 3:
                        tarif_mode = m200_settings.threetarifs;
                        m200_settings.t_mode = 3;
                        break;
                    return;
                }
                M200_Write_Tarif_Main(0);
                //m200_comm_number = 0x0A;
                //M200_Send(0x0A, &data[1], 1);
            }
        }
        else if(length == 1)
        {
            tmpbuf[0] = 202;
            tmpbuf[1] = m200_settings.t_mode;
            NBFi_Send(tmpbuf, 2);
        }
        break;
    case 203:
        if(length == 1)
        {
            tmpbuf[0] = 203;
            tmpbuf[1] = write_tarif_state;
            NBFi_Send(tmpbuf, 2);
        }
        break;
    case 204:
        if((length >= 4)&& (length <= 1 + 3*4)&& !((length - 1)%3))
        {
            for(i = 0; i != (length - 1)/3; i++)
            {
                m200_settings.twotarifs[i].tarif = data[1 + i*3];
                m200_settings.twotarifs[i].hour = data[2 + i*3];
                m200_settings.twotarifs[i].minute = data[3 + i*3];
            }
            m200_settings.twotarifs[i].tarif = END;
            M200_WriteSettings();
        }
        else
        {
           tmpbuf[0] = 204;

           for(i = 0; i != 4; i++)
           {
                if(m200_settings.twotarifs[i].tarif == END) break;
                tmpbuf[i*3 + 1] = m200_settings.twotarifs[i].tarif;
                tmpbuf[i*3 + 2] = m200_settings.twotarifs[i].hour;
                tmpbuf[i*3 + 3] = m200_settings.twotarifs[i].minute;
           }
           NBFi_Send(tmpbuf, 1 + i*3);
        }
        break;
    case 205:
        if((length >= 4)&&(length <= 1 + 3*9)&& !((length - 1)%3))
        {
            for(i = 0; i != (length - 1)/3; i++)
            {
                m200_settings.threetarifs[i].tarif = data[1 + i*3];
                m200_settings.threetarifs[i].hour = data[2 + i*3];
                m200_settings.threetarifs[i].minute = data[3 + i*3];
            }
            m200_settings.threetarifs[i].tarif = END;
            M200_WriteSettings();
        }
        else
        {
           tmpbuf[0] = 205;
           for(i = 0; i != 9; i++)
           {
                if(m200_settings.threetarifs[i].tarif == END) break;
                tmpbuf[i*3 + 1] = m200_settings.threetarifs[i].tarif;
                tmpbuf[i*3 + 2] = m200_settings.threetarifs[i].hour;
                tmpbuf[i*3 + 3] = m200_settings.threetarifs[i].minute;
           }
           NBFi_Send(tmpbuf, 1 + i*3);
        }
        break;
    case 210:
        if(length >= 2)
        {
            m200_comm_number = data[1];
            M200_Send(data[1], &data[2], length - 2);
        }
        break;
    case 211:
         M200_Send_RAW(&data[1], length - 1);
        break;
    case 239:   //on_off fast_dl
        if(length != 2) break;
        switch(data[1])
        {
        case 0:     //set FAST_DL
            NBFi_Config_Set_FastDl(1);
            ScheduleTask(&m200_fastdl_desc, &ResetFastDl, RELATIVE, SECONDS(60*5));
            break;
        case 1:     //reset FAST_DL
            wtimer0_remove(&m200_fastdl_desc);
            ResetFastDl(0);
            break;
        case 5:     //radio test
            ResetFastDl(0);
            wtimer0_remove(&m200_fastdl_desc);
            nbfi.mode = CRX;
            nbfi.heartbeat_interval = 5;
            ScheduleTask(&m200_radiotest_desc, &RadioTest, RELATIVE, SECONDS(60*3));
            NBFi_SendHeartBeats(0);
            break;
        }
        break;
    case 245:       //set time
        if(length >= 5)
        {
            struct tm *time_str;
            bigendian_cpy((uint8_t __xdata*)&data[1], (uint8_t __xdata*)&dltime, 4);
            time_str = (struct tm*)localtime(&dltime);
            tmpbuf[0] = 0x0C;
            tmpbuf[1] = dec2bcd(time_str->tm_sec);
            tmpbuf[2] = dec2bcd(time_str->tm_min);
            tmpbuf[3] = dec2bcd(time_str->tm_hour);
            tmpbuf[4] = dec2bcd(time_str->tm_wday);
            tmpbuf[5] = dec2bcd(time_str->tm_mday);
            tmpbuf[6] = dec2bcd(time_str->tm_mon + 1);
            tmpbuf[7] = dec2bcd(time_str->tm_year - 100);
            tmpbuf[8] = dec2bcd(time_str->tm_isdst);
            m200_comm_number = 0xFE;
            M200_Send(CMD_METER_WRITE_PARAM, tmpbuf, 9);
        }
        else if(length == 1)
        {
            tmpbuf[0] = 245;
            bigendian_cpy((uint8_t __xdata*)&dltime, (uint8_t __xdata*)&tmpbuf[1], 4);
            NBFi_Send(tmpbuf, 5);
        }
        break;
    }
}

uint16_t __xdata h_interval;
uint16_t __xdata SW_IN;

void EverySec(struct wtimer_desc __xdata *desc)
{
    static uint8_t cmd;

    if(nbfi.heartbeat_interval > 60)
        h_interval = nbfi.heartbeat_interval;
    if(!SW_IN)
    {
        sw_free = 0;
        PA_EN = 0;
        delay_ms(20);
        PA_EN = 1;
        delay_ms(20);
        PA_EN = 0;
        cmd++;
        NBFi_Force_process();
    }
    else
    {
        switch(cmd)
        {
        case 1:
            if(!CheckTask(&m200_fastdl_desc))
            {
                if(regular_state == 0)
                {
                    regular_state = 5;
                    SendRegular(0);
                }
                nbfi.heartbeat_interval = 2;
                ScheduleTask(&m200_active_desc, &ResetActive, RELATIVE, SECONDS(60*20));
            }
            break;
        case 2:
            if(CheckTask(&m200_fastdl_desc))
            {
                wtimer0_remove(&m200_fastdl_desc);
                ResetFastDl(0);
            }
            break;
        case 3:
            if(!CheckTask(&m200_fastdl_desc))
            {
                NBFi_Config_Set_FastDl(1);
                ScheduleTask(&m200_fastdl_desc, &ResetFastDl, RELATIVE, SECONDS(60*2));
            }
            break;
        default:
            break;
        }
        cmd = 0;
        sw_free = 1;
    }
    ScheduleTask(&m200_everysec_desc, &EverySec, RELATIVE, SECONDS(1));

}

void M200_Loop()
{
    if(!CheckTask(&m200_everysec_desc))
        EverySec(0);
}

void ResetActive(struct wtimer_desc __xdata *desc)
{
    nbfi.heartbeat_interval = h_interval;
}

void ResetFastDl(struct wtimer_desc __xdata *desc)
{
    NBFi_Config_Set_FastDl(0);
    nbfi.heartbeat_interval = 2;
    ScheduleTask(&m200_active_desc, &ResetActive, RELATIVE, SECONDS(60*20));
}

void RadioTest(struct wtimer_desc __xdata *desc)
{
    NBFi_Config_Set_FastDl(1);
    ScheduleTask(&m200_fastdl_desc, &ResetFastDl, RELATIVE, SECONDS(60*2));
}

void M200_Send_Event(uint8_t event, uint32_t time, uint16_t data)
{
    tmpbuf[0] = 0x33;
    tmpbuf[1] = (time >> 24) & 0xFF;
    tmpbuf[2] = (time >> 16) & 0xFF;
    tmpbuf[3] = (time >> 8) & 0xFF;
    tmpbuf[4] = (time >> 0) & 0xFF;
    tmpbuf[5] = event;
    tmpbuf[6] = (data >> 8) & 0xFF;
    tmpbuf[7] = (data >> 0) & 0xFF;
    NBFi_Send(tmpbuf, 8);
}

void M200_Write_Tarif_Packet(TARIF_ZONE *zone, uint16_t address)
{
//    uint8_t mas[32];
    uint8_t *mas = watch_data;
    mas[0] = 0x02;
    mas[1] = address >> 8;
    mas[2] = address & 0xFF;
    mas[3] = 0x10;

    if (zone)
    {
        //  find last time of tarif and set him to 00:00
        TARIF_ZONE *zone_tmp = zone;
        while(zone_tmp->tarif != END)
            zone_tmp++;
        zone_tmp--;
        mas[4] = 0x00;
        mas[5] = (((uint8_t)zone_tmp->tarif + 1) << 5);

        for(uint8_t i = 1; i != 8; i++)
        {
            if(zone->tarif != END)
            {
                mas[4 + i * 2] = zone->minute;
                mas[4 + i * 2 + 1] = (((uint8_t)zone->tarif + 1) << 5) + zone->hour;
                zone++;
            }
            else
            {
                mas[4 + i * 2] = 0;
                mas[4 + i * 2 + 1] = (1 << 5) + 24;
            }
        }
    }
    else
    {
        for(uint8_t i = 0; i != 8; i++)
        {
            mas[4 + i * 2] = 0;
            mas[4 + i * 2 + 1] = (1 << 5) + 24;
        }
    }

    M200_Send(CMD_METER_ULTRA_WRITE_PARAM, mas, 20);
}

void M200_Write_Tarif_Main(struct wtimer_desc __xdata *desc)
{
    uint8_t data[10];
    switch(write_tarif_state)
    {
    case 0:
        if(regular_state|sinchro_state)
        {
            ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, SECONDS(10));
            return;
        }
        write_tarif_state = 120;
        write_tarif_try_num = 0;
    case 120:
        if(++write_tarif_try_num <= 3)
        {
            data[0] = 2;
            for(uint8_t i = 0; i < 6; i++)
                data[i + 1] = 0x02;
            M200_Send(CMD_METER_GET_ACCESS, data, 7);
        }
        else
        {
            write_tarif_state = 0;
            m200_settings.t_mode = 0;
            M200_WriteSettings();
            M200_Send_Event(200, 0, 0x0A*256);
            break;
        }
        ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, SECONDS(1));
        return;
    case 121:
        write_tarif_try_num = 0;
        write_tarif_state = 1;
    case 1:
        if(++write_tarif_try_num <= 3)
        {
            data[0] = 0x2A;
            data[1] = 0x00;
            M200_Send(CMD_METER_WRITE_PARAM, data, 2);
        }
        else
        {
            write_tarif_state = 0;
            m200_settings.t_mode = 0;
            M200_WriteSettings();
            M200_Send_Event(200, 0, 0x0A*256);
            return;
        }
        ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, SECONDS(1));
        return;
    case 101:
        switch(m200_settings.t_mode)
        {
        case 1:
            display_mode = 0x02;
            break;
        case 2:
            display_mode = 0x06;
            break;
        case 3:
            display_mode = 0x0E;
            break;
        }
        if(++write_tarif_try_num <= 3)
        {
            data[0] = 0x01;
            for(uint8_t i = 0; i < 4; i++)
            {
                data[i * 2 + 1] = 0x1F;
                data[i * 2 + 2] = 0x00;
            }
            M200_Send(CMD_METER_WRITE_PARAM, data, 9);
        }
        else
        {
            write_tarif_state = 0;
            m200_settings.t_mode = 0;
            M200_WriteSettings();
            M200_Send_Event(200, 0, 0x09*256);
            return;
        }
        ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, SECONDS(1));
        return;
    case 98:
        write_tarif_state = 99;
        M200_WriteSettings();
        M200_Send_Event(0x11, 0, 0);
        return;
    case 99:
        write_tarif_parity = 0;
        write_tarif_state = 0;
        ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, MILLISECONDS(5));
        return;
    default:
        if(write_tarif_state >=2 && write_tarif_state < 98)
        {
            if(++write_tarif_try_num <= 3)
            {
                uint8_t pos = write_tarif_state - 2;
                uint16_t address = 0x1000 + pos * 34 + write_tarif_parity * 17;
                M200_Write_Tarif_Packet(write_tarif_parity ? 0 : tarif_mode, address);
            }
            else
            {
                write_tarif_parity = 0;
                write_tarif_state = 0;
                m200_settings.t_mode = 0;
                M200_WriteSettings();
                M200_Send_Event(200, 0, 0x11*256);
                return;
            }
            ScheduleTask(&m200_write_tarif_desc, &M200_Write_Tarif_Main, RELATIVE, SECONDS(1));
            return;
        }
    }
}
