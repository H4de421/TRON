#include "Display/MenuDisplay.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "Display/Colors.h"
#include "Utils/String.h"
#include "globals.h"

const MenuOption optionList[NB_MENU_OPTION] = {
    { .name="Start" },
    { .name="Quit" }
};  

void display_main_title(String *buffer, int x, int y)
{
    /*
    ███ ▄████████████▄    ▄████▄   ██▄      ██  
        ██    ▄▄    ▄▄  ▄█▀    ▀█▄ █ ▀█▄    ██   
        ██    ██ ▄▄▄█▀  ██      ██ █▄▄▄█ █▀▀▀█        
        ██    ██ ▀██▄   ▀█▄    ▄█▀ ██    ▀█▄ █         
        ██    ██   ▀██▄   ▀████▀   ██      ▀██        
    */
    const char *mainTitleFormat = "\e[%d;%dH███ ▄████████████▄    ▄████▄   ██▄      ██\e[%d;%dH    ██    ▄▄    ▄▄  ▄█▀    ▀█▄ █ ▀█▄    ██\e[%d;%dH    ██    ██ ▄▄▄█▀  ██      ██ █▄▄▄█ █▀▀▀█\e[%d;%dH    ██    ██ ▀██▄   ▀█▄    ▄█▀ ██    ▀█▄ █\e[%d;%dH    ██    ██   ▀██▄   ▀████▀   ██      ▀██";

    // each title char is unicode
    // so it take more than 1 bytes
    //
    // each row contain close to 120 bytes
    char mainTitle[mainTitleMaxSize];
    sprintf(mainTitle, mainTitleFormat, y, x, y+1, x, y+2,x, y+3, x, y+4 ,x);

    addToString(buffer, mainTitle, mainTitleMaxSize);
}

void display_menu_options(int cursor, String *buffer)
{
    const int init_y = ((CANVAS_HEIGHT * 7) / 8) - ( NB_MENU_OPTION * MENU_OPTION_H_SIZE + (NB_MENU_OPTION-1) * MENU_OPTION_H_MARGIN );

    for (int i =0; i<NB_MENU_OPTION; i++)
    {
        char final_name[170];
        char temp_name[130];

        const MenuOption option = optionList[i];
        sprintf(temp_name, "%d - %s", i, option.name);
        

        const int option_y = init_y + (i * (MENU_OPTION_H_SIZE+MENU_OPTION_H_MARGIN));
        const int option_x = (CANVAS_WIDTH / 2) - (strlen(temp_name) / 2);
    
        char color[15];
        sprintf(color,"\x1b[%d;5;247m", ((i==cursor)?48:38));

        sprintf(final_name, "\e[%d:%dH%s%s%s", option_y, option_x, color ,temp_name, NO_COLOR);
        
        addToString(buffer, final_name, 150);
    }
}

void display_menu(String *buffer)
{
    int title_y = (CANVAS_HEIGHT /4 ) - (MAIN_TITLE_HEIGHT / 2); 
    int title_x = (CANVAS_WIDTH / 2 ) - (MAIN_TITLE_WIDTH / 2); 

    display_main_title(buffer, title_x, title_y);

    display_menu_options(0, buffer);
}
