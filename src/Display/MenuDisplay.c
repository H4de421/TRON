#include "Display/MenuDisplay.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "Display/Colors.h"
#include "Utils/String.h"
#include "globals.h"

const MenuOption optionList[NB_MENU_OPTION] = {
    { .name="Start", .color=BORDER_COLOR },
    { .name="Quit" , .color=RED_COLOR}
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
    const int init_y = ((G_CANVAS_HEIGHT * 7) / 8) - ( NB_MENU_OPTION * G_MENU_OPTION_H_SIZE + (NB_MENU_OPTION-1) * G_MENU_OPTION_H_MARGIN );

    for (int i =0; i<NB_MENU_OPTION; i++)
    {
        char final_name[250];
        char temp_name[130];

        const MenuOption option = optionList[i];
        sprintf(temp_name, "\e[3m%d - %s", i, option.name);
        

        const int option_y = init_y + (i * (G_MENU_OPTION_H_SIZE+G_MENU_OPTION_H_MARGIN));
        const int option_x = (G_CANVAS_WIDTH / 2) - (strlen(temp_name) / 2);

        /*╭┐ 1 - Tron
          ╰────────────■*/

        sprintf(final_name, "\e[3m\e[%d:%dH%s╭┐ %s\e[%d:%dH╰───────────────■%s", option_y, option_x, (i==cursor ? option.color : NO_COLOR),temp_name, option_y+1, option_x,NO_COLOR);
        
        addToString(buffer, final_name, 150);
    }
}

void display_menu(String *buffer)
{
    int title_y = (G_CANVAS_HEIGHT /4 ) - (MAIN_TITLE_HEIGHT / 2); 
    int title_x = (G_CANVAS_WIDTH / 2 ) - (MAIN_TITLE_WIDTH / 2); 

    display_main_title(buffer, title_x, title_y);

    display_menu_options(0, buffer);
}
