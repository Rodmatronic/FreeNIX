#include "../include/types.h"
#include "../include/stat.h"
#include "../include/fcntl.h"
#include "../include/user.h"

char 
*month[] = {
  "Jan",	
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec"
};

const char*
getday(int day, int month, int year)
{
    if (month < 3) {
        month += 12;
        year--;
    }
    int q = day;
    int m = month;
    int K = year % 100;
    int J = year / 100;
    int h = (q + (13*(m+1))/5 + K + (K/4) + (J/4) + 5*J);
    h %= 7;
    if (h < 0) h += 7;
    switch (h) {
        case 0: return "Sat";
        case 1: return "Sun";
        case 2: return "Mon";
        case 3: return "Tue";
        case 4: return "Wed";
        case 5: return "Thu";
        case 6: return "Fri";
        default: return "Err";
    }
}

const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0};
const int days_in_month_leap[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0};

int isleapyear(int);

void
epoch_to_tm(unsigned long epoch, struct tm *tm)
{
    const int seconds_per_minute = 60;
    const int epoch_year = 1970;

    tm->tm_sec = epoch % seconds_per_minute;
    epoch /= seconds_per_minute;
    tm->tm_min = epoch % seconds_per_minute;
    epoch /= seconds_per_minute;
    tm->tm_hour = epoch % 24;
    epoch /= 24;

    // Calculate day of week (0=Sunday)
    tm->tm_wday = (epoch + 4) % 7; // Jan 1, 1970 was Thursday (4)

    int year = epoch_year;
    int days = 0;
    while (1) {
        int leap = isleapyear(year);
        if (days + (leap ? 366 : 365) > epoch) {
            break;
        }
        days += leap ? 366 : 365;
        year++;
    }
    tm->tm_year = year - 1900; // struct tm expects years since 1900
    tm->tm_yday = epoch - days;

    int month = 0;
    int day = tm->tm_yday;
    const int *month_days = isleapyear(year) ? days_in_month_leap : days_in_month;
    while (day >= month_days[month]) {
        day -= month_days[month];
        month++;
    }
    tm->tm_mon = month;
    tm->tm_mday = day + 1; // 1-based day of month
    tm->tm_isdst = -1; // Daylight Saving Time information not available
}

int
isleapyear(int year)
{
    if (year % 4 != 0) return 0;
    if (year % 100 != 0) return 1;
    return (year % 400 == 0);
}
