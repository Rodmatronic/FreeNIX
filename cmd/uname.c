#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"

int
main(argc, argv)
char * argv[];
{
    struct utsname uts;
    int print_s = 0, print_n = 0, print_r = 0, print_v = 0, print_m = 0;
    int print_all = 0;
    int any_option = 0;

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] != '-') {
            fprintf(stderr, "uname: invalid argument: %s\n", arg);
            exit(1);
        }
        for (int j = 1; arg[j] != '\0'; j++) {
            char c = arg[j];
            switch (c) {
                case 'a':
                    print_all = 1;
                    any_option = 1;
                    break;
                case 's':
                    print_s = 1;
                    any_option = 1;
                    break;
                case 'n':
                    print_n = 1;
                    any_option = 1;
                    break;
                case 'r':
                    print_r = 1;
                    any_option = 1;
                    break;
                case 'v':
                    print_v = 1;
                    any_option = 1;
                    break;
                case 'm':
                    print_m = 1;
                    any_option = 1;
                    break;
                default:
                    fprintf(stderr, "uname: invalid option -- '%c'\n", c);
                    exit(1);
            }
        }
    }

    if (argc == 1) {
        print_s = 1;
    } else if (!any_option) {
        print_s = 1;
    }

    uname(&uts);

    if (print_all) {
        print_s = print_n = print_r = print_v = print_m = 1;
    }

    int printed = 0;
    if (print_s) {
        printf("%s", uts.sysname);
        printed = 1;
    }
    if (print_n) {
        if (printed) printf(" ");
        printf("%s", uts.nodename);
        printed = 1;
    }
    if (print_r) {
        if (printed) printf(" ");
        printf("%s", uts.release);
        printed = 1;
    }
    if (print_v) {
        if (printed) printf(" ");
        printf("%s", uts.version);
        printed = 1;
    }
    if (print_m) {
        if (printed) printf(" ");
        printf("%s", uts.machine);
    }
    putchar('\n');

    exit(0);
}
