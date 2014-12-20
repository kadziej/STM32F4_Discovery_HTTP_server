/*
 * rtc.h
 */

#ifndef RTC_H_
#define RTC_H_

#include <inttypes.h>


typedef enum { MON, TUE, WED, THR, FRI, SAT, SUN } WEEK_DAY;
typedef enum { JAN, FEB, MAR, APR, MAY, JUN, JUL, OCT, AUG, SEPT, NOV, DEC } MONTH;

//typedef struct {

	//WEEK_DAY day;
	//MONTH month;

	//uint16_t year;
	//uint8_t hour;
	//uint8_t minute;
	//uint8_t second;

//}  RTCDate;

//void RTCgetTime(RTCDate * time);



void RTC_setTimeFromSNTP(time_t);
void RTC_Config(void);
void RTC_TimeShow(void);

void RTC_GetTime_char(char *time);
void RTC_GetDate_char(char *date);

void RTC_GetDate_yesterday_char(char *date, uint16_t _i);

#endif /* RTC_H_ */
