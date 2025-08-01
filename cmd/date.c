#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"
#include "../include/fs.h"

unsigned long time_t;

int
main(int argc, char *argv[])
{
    unsigned long epoch = time(time_t);
    struct tm tm;

    epoch_to_tm(epoch, &tm);
    printf("%s %s %d %02d:%02d:%02d %d\n", getday(tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900), month[tm.tm_mon], tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_year + 1900);

    exit(0);
}
