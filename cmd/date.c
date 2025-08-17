#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"

unsigned long time_t;

int day_of_year(int year, int month, int day) {
    int days = day;
    for (int i = 0; i < month - 1; i++) {
        days += days_in_month[i];
    }
    if (month > 2 && isleapyear(year)) {
        days++;
    }
    return days;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
	goto print;
	exit(0);
    } else if (argc == 2) {
        char *s = argv[1];
        int len = strlen(s);
        if (len != 8 && len != 10) {
            fprintf(stderr, "date: invalid format\n");
            exit(1);
        }

        int MM = (s[0] - '0') * 10 + (s[1] - '0');
        int DD = (s[2] - '0') * 10 + (s[3] - '0');
        int hh = (s[4] - '0') * 10 + (s[5] - '0');
        int mm = (s[6] - '0') * 10 + (s[7] - '0');
        int yy = 0;
        if (len == 10) {
            yy = (s[8] - '0') * 10 + (s[9] - '0');
        }

        if (MM < 1 || MM > 12) {
            fprintf(stderr, "date: invalid month\n");
            exit(1);
        }
        if (DD < 1 || DD > 31) {
            fprintf(stderr, "date: invalid day\n");
            exit(1);
        }
        if (hh < 0 || hh > 23) {
            fprintf(stderr, "date: invalid hour\n");
            exit(1);
        }
        if (mm < 0 || mm > 59) {
            fprintf(stderr, "date: invalid minute\n");
            exit(1);
        }

        struct tm new_tm;
        new_tm.tm_sec = 0;
        new_tm.tm_min = mm;
        new_tm.tm_hour = hh;
        new_tm.tm_mday = DD;
        new_tm.tm_mon = MM - 1;

        if (len == 10) {
            if (yy < 70) {
                new_tm.tm_year = yy + 100; // 2000-2069: 100 -> 2000 (1900+100=2000)
            } else {
                new_tm.tm_year = yy;       // 1970-1999
            }
        } else {
            unsigned long now = time(0);
            struct tm now_tm;
            epoch_to_tm(now, &now_tm);
            new_tm.tm_year = now_tm.tm_year;
        }

        int full_year = 1900 + new_tm.tm_year;
        int max_days = days_in_month[new_tm.tm_mon];
        if (new_tm.tm_mon == 1 && isleapyear(full_year)) { // February in leap year
            max_days++;
        }
        if (DD < 1 || DD > max_days) {
            fprintf(stderr, "date: invalid day for month\n");
            exit(1);
        }

        unsigned long new_epoch = mktime(&new_tm);
        if (stime(new_epoch) < 0) {
            fprintf(stderr, "date: stime failed\n");
            exit(1);
        }
    } else {
        fprintf(stderr, "Usage: date [MMDDhhmm[yy]]\n");
        exit(1);
    }

print:
    unsigned long epoch = time(time_t);
    struct tm tm;
    epoch_to_tm(epoch, &tm);
    printf("%s %s %d %02d:%02d:%02d %d\n", getday(tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900), month[tm.tm_mon], tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_year + 1900);

    exit(0);
}
