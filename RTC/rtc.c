/*
 * rtc.c
 *
 */
#include "rtc.h"
#include "stm32f4xx_rtc.h"
#include <time.h>

uint32_t uwAsynchPrediv = 0;
uint32_t uwSynchPrediv = 0;



RTC_TimeTypeDef  RTC_TimeStructure;
RTC_InitTypeDef  RTC_InitStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
RTC_DateTypeDef RTC_DateStructure;


void RTC_setTimeFromSNTP(time_t *t)
{
	*t+=(7200-3600); // +1 or 2h

	struct tm  ts;
	uint8_t buf[80];
	ts = *localtime(t);

	RTC_DateStructure.RTC_Year = ts.tm_year%100;
	switch(ts.tm_mon+1)
	{
		case 1: RTC_DateStructure.RTC_Month = 0x01 ; break;
		case 2: RTC_DateStructure.RTC_Month = 0x02 ; break;
		case 3: RTC_DateStructure.RTC_Month = 0x03 ; break;
		case 4: RTC_DateStructure.RTC_Month = 0x04 ; break;
		case 5: RTC_DateStructure.RTC_Month = 0x05 ; break;
		case 6: RTC_DateStructure.RTC_Month = 0x06 ; break;
		case 7: RTC_DateStructure.RTC_Month = 0x07 ; break;
		case 8: RTC_DateStructure.RTC_Month = 0x08 ; break;
		case 9: RTC_DateStructure.RTC_Month = 0x09 ; break;
		case 10: RTC_DateStructure.RTC_Month = 0x10 ; break;
		case 11: RTC_DateStructure.RTC_Month = 0x11 ; break;
		case 12: RTC_DateStructure.RTC_Month = 0x12 ; break;
		default: RTC_DateStructure.RTC_Month = 0x01 ; break;
	}
	//RTC_DateStructure.RTC_Month = RTC_Month_January;
	RTC_DateStructure.RTC_Date = ts.tm_mday;
	RTC_DateStructure.RTC_WeekDay = ts.tm_wday; //RTC_Weekday_Saturday;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

	// Set the time
	//RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	RTC_TimeStructure.RTC_Hours   = ts.tm_hour;
	RTC_TimeStructure.RTC_Minutes = ts.tm_min;
	RTC_TimeStructure.RTC_Seconds = ts.tm_sec;

	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

	strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	printf("%s\n\r", buf);
}


// Konfiguracja zegara RTC
void RTC_Config(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	/* Enable PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* LSI used as RTC source clock*/
	/* The RTC Clock may varies due to LSI frequency dispersion */
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	// ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	uwSynchPrediv = 0xF0; // Im wiêksza wartoæ tym wolniej liczy
	uwAsynchPrediv = 0x7F; //0x7F 30976

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Configure the RTC data register and RTC prescaler */
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);


	/* Set the date: Friday January 11th 2013 */
	RTC_DateStructure.RTC_Year = 14;
	RTC_DateStructure.RTC_Month = RTC_Month_December;
	RTC_DateStructure.RTC_Date = 1;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

	/* Set the time to 05h 20mn 00s AM */
	//RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	RTC_TimeStructure.RTC_Hours   = 13; //0x06;//0x05;
	RTC_TimeStructure.RTC_Minutes = 00; //0x58;//0x20;
	RTC_TimeStructure.RTC_Seconds = 0x00;

	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

	/* Indicator for the RTC configuration */
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}


void RTC_TimeShow(void)
{
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

  /* Display time Format : hh:mm:ss */
  printf("%d:%d:%d  %02d.%02d.%02d  ",RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds,RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Year);

}



void RTC_GetTime_char(char *time)
{
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	sprintf(time, "%02d:%02d:%02d", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
}

void RTC_GetDate_char(char *date)
{
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	sprintf(date, "%02d-%02d-%02d", RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Year);
}

void RTC_GetDate_yesterday_char(char *date, uint16_t _i)
{
	uint8_t _day,_month,_year;
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

	_day = RTC_DateStructure.RTC_Date;
	_month = RTC_DateStructure.RTC_Month;
	_year = RTC_DateStructure.RTC_Year;

	while(_i>0)
	{
		_i--;
		if(_day==1)
		{
			if(_month==1) {
				_day=31;
				_month=12;
				_year-=1;
			} else {
				_month-=1;
				switch(_month)
				{
					case 1: _day = 31; break;
					case 2: _day = 28; break;
					case 3: _day = 31; break;
					case 4: _day = 30; break;
					case 5: _day = 31; break;
					case 6: _day = 30; break;
					case 7: _day = 31; break;
					case 8: _day = 31; break;
					case 9: _day = 30; break;
					case 10: _day = 31; break;
					case 11: _day = 30; break;
				}
				if(_month==2)
				{
					if(_year%4==0) _day = 29;
				}
			}
		} else {
			_day-=1;
		}
	}

	sprintf(date, "%02d-%02d-%02d", _day, _month,_year);
}
