#include <psp2/kernel/processmgr.h>
#include <psp2/rtc.h>

#include "debugScreen.h"

#define printf psvDebugScreenPrintf

int main(int argc, char *argv[]) {
	psvDebugScreenInit();

	SceDateTime time;
	sceRtcGetCurrentClock(&time, 13);
	printf("Phoenix Island Time:%04d/%02d/%02d %02d:%02d:%02d\n",
		sceRtcGetYear(&time),
		sceRtcGetMonth(&time),
		sceRtcGetDay(&time),
		sceRtcGetHour(&time),
		sceRtcGetMinute(&time),
		sceRtcGetSecond(&time),
		sceRtcGetMicrosecond(&time));

	sceRtcGetCurrentClockLocalTime(&time);
	printf("Your Local Time:%04d/%02d/%02d %02d:%02d:%02d Valid:%i\n",
		sceRtcGetYear(&time),
		sceRtcGetMonth(&time),
		sceRtcGetDay(&time),
		sceRtcGetHour(&time),
		sceRtcGetMinute(&time),
		sceRtcGetSecond(&time),
		sceRtcGetMicrosecond(&time),
		sceRtcCheckValid(&time));

	printf("We are the %ith days of the week (Sun:0)\n",
	       sceRtcGetDayOfWeek(sceRtcGetYear(&time), sceRtcGetMonth(&time), sceRtcGetDay(&time)));
	printf("They are %i days this month and this %s a lap year\n",
		sceRtcGetDaysInMonth(sceRtcGetYear(&time), sceRtcGetMonth(&time)),
		sceRtcIsLeapYear(sceRtcGetYear(&time))?"is":"is not");

	printf("Tick Resolution = %i\n", sceRtcGetTickResolution());

	SceRtcTick current, network, utc, local;
	
	printf("     Current %s:%llu\n", sceRtcGetCurrentTick       (&current      )?"FAIL":"", current.tick);
	printf("     Network %s:%llu\n", sceRtcGetCurrentNetworkTick(&network      )?"FAIL":"", network.tick);
	printf("     Loc2UTC %s:%llu\n", sceRtcConvertLocalTimeToUtc(&current, &utc)?"FAIL":"", utc.tick);
	printf("     UTC2Loc %s:%llu\n", sceRtcConvertUtcToLocalTime(&utc, &local  )?"FAIL":"", local.tick);
	printf("     Loc %c UTC\n","<=>"[sceRtcCompareTick(&current, &utc)+1]);
	printf("     UTC %c Loc\n","<=>"[sceRtcCompareTick(&utc, &current)+1]);
	printf("     UTC %c UTC\n","<=>"[sceRtcCompareTick(&utc, &utc    )+1]);
	
	SceRtcTick add1000, us1000, sec120, min120, hour48, days32, week64, month13, year1000;
	printf("      T+1000 %s:%llu\n", sceRtcTickAddTicks       (&add1000, &current, 1000)?"FAIL":"", add1000.tick);
	printf("     Us+1000 %s:%llu\n", sceRtcTickAddMicroseconds(&us1000,  &current, 1000)?"FAIL":"", us1000.tick);
	printf("      S+120  %s:%llu\n", sceRtcTickAddSeconds     (&sec120,  &current,  120)?"FAIL":"", sec120.tick);
	printf("      M+120  %s:%llu\n", sceRtcTickAddMinutes     (&min120,  &current,  120)?"FAIL":"", min120.tick);
	printf("      H+48   %s:%llu\n", sceRtcTickAddHours       (&hour48,  &current,   48)?"FAIL":"", hour48.tick);
	printf("      D+32   %s:%llu\n", sceRtcTickAddDays        (&days32,  &current,   32)?"FAIL":"", days32.tick);
	printf("      W+64   %s:%llu\n", sceRtcTickAddWeeks       (&week64,  &current,   64)?"FAIL":"", week64.tick);
	printf("      M+13   %s:%llu\n", sceRtcTickAddMonths      (&month13, &current,   13)?"FAIL":"", month13.tick);
	printf("      Y+1000 %s:%llu\n", sceRtcTickAddYears       (&year1000,&current, 1000)?"FAIL":"", year1000.tick);

	char rfc3339_loc[128], rfc3339_12[128], rfc2822_loc[128], rfc2822_12[128];
	printf("RFC3339local %s:%s\n", sceRtcFormatRFC3339LocalTime(rfc3339_loc,&utc      )?"FAIL":"", rfc3339_loc);
	printf("RFC3339 + 2  %s:%s\n", sceRtcFormatRFC3339         (rfc3339_12, &utc, -120)?"FAIL":"", rfc3339_12);
	printf("RFC2822local %s:%s\n", sceRtcFormatRFC2822LocalTime(rfc2822_loc,&utc      )?"FAIL":"", rfc2822_loc);
	printf("RFC2822 - 2  %s:%s\n", sceRtcFormatRFC2822         (rfc2822_12, &utc, +120)?"FAIL":"", rfc2822_12);
	
	const char*rfc_ex1 = "2002-10-02T10:00:00-05:00";
	const char*rfc_ex2 = "2002-10-02T15:00:00Z";
	const char*rfc_ex3 = "2002-10-02T15:00:00.05Z";
	SceRtcTick parse_ex1, parse_ex2, parse_ex3;
	printf("RFC3339_1    %s:%-26s->%llu\n", sceRtcParseRFC3339(&parse_ex1, rfc_ex1)?"FAIL":"", rfc_ex1, parse_ex1.tick);
	printf("RFC3339_2    %s:%-26s->%llu\n", sceRtcParseRFC3339(&parse_ex2, rfc_ex2)?"FAIL":"", rfc_ex2, parse_ex2.tick);
	printf("RFC3339_3    %s:%-26s->%llu\n", sceRtcParseRFC3339(&parse_ex3, rfc_ex3)?"FAIL":"", rfc_ex3, parse_ex3.tick);

	const char*datetime_ex1 = "2002-10-02T10:00:00-05:00";
	const char*datetime_ex2 = "2002-10-02T15:00:00Z";
	const char*datetime_ex3 = "2002-10-02T15:00:00.05Z";
	SceRtcTick parsedDate_ex1, parsedDate_ex2, parsedDate_ex3;
	printf("datetime_1   %s:%-26s->%llu\n",sceRtcParseDateTime(&parsedDate_ex1, datetime_ex1)?"FAIL":"", datetime_ex1, parsedDate_ex1);
	printf("datetime_2   %s:%-26s->%llu\n",sceRtcParseDateTime(&parsedDate_ex2, datetime_ex2)?"FAIL":"", datetime_ex2, parsedDate_ex2);
	printf("datetime_3   %s:%-26s->%llu\n",sceRtcParseDateTime(&parsedDate_ex3, datetime_ex3)?"FAIL":"", datetime_ex3, parsedDate_ex3);

	/* TODO 
	sceRtcSetTime_t(&time, time_t iTime);
	sceRtcSetTime64_t(&time, SceUInt64 ullTime);
	sceRtcGetTime_t(const &time, time_t *piTime);
	sceRtcGetTime64_t(const &time, SceUInt64 *pullTime);
	sceRtcSetDosTime(&time, unsigned int uiDosTime);
	sceRtcGetDosTime(const &time, unsigned int *puiDosTime);
	sceRtcSetWin32FileTime(&time, SceUInt64 ulWin32Time);
	sceRtcGetWin32FileTime(const &time, SceUInt64 *ulWin32Time);
	sceRtcSetTick(&time, const &tick);
	sceRtcGetTick(const &time, &tick);
	*/

	int i=60;
	while(i-->0){
		printf("This sample will close in %2i seconds\r", i);
		sceKernelDelayThread(1000*1000);
	}

	sceKernelExitProcess(0);
	return 0;
}
