#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#include "menuio.h"
#include "dwindow.h"

int main(int argc, char **argv){
    setlocale(LC_CTYPE, "");
    set_display_size();
    FIRST_PRINT;
    welcome();
}
