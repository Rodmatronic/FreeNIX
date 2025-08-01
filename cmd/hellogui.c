#include "../include/errno.h"
#include "../include/user.h"
#include "../include/stat.h"
#include "../include/fcntl.h"
#include "../include/font8x16.h"
#include "../include/graphics.h"

int guitestbutton;
int helloworldbutton;

int
main()
{
    initgraphics("Hello World!", WHITE);
    graphical_puts(20, 45, "Hello World!", BLACK);
    flush_background();

    while (1) {
	mouser();
        usleep(5000);
    }

    return 0;
}
