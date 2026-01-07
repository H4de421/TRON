#include "Display/MenuDisplay.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "Display/Colors.h"
#include "Utils/String.h"
#include "globals.h"

#define NB_MENU 2
#define NB_MENU_MULTI_OPTION 2

#define MAX(a, b) ((a) > (b) ? a : a)

const MenuOption
    optionList[NB_MENU][MAX(NB_MENU_OPTION, NB_MENU_MULTI_OPTION)] = {
        { { .name = "Solo", .color = BORDER_COLOR },
          { .name = "Multi", .color = BORDER_COLOR },
          { .name = "Quit", .color = RED_COLOR } },
        { { .name = "Launch server", .color = BORDER_COLOR },
          { .name = "join a friend", .color = BORDER_COLOR },
          { .name = "back", .color = RED_COLOR } },
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
    const char *mainTitleFormat =
        "\e[%d;%dH███ ▄████████████▄    ▄████▄   ██▄      ██\e[%d;%dH    ██    "
        "▄▄    ▄▄  ▄█▀    ▀█▄ █ ▀█▄    ██\e[%d;%dH    ██    ██ ▄▄▄█▀  ██      "
        "██ █▄▄▄█ █▀▀▀█\e[%d;%dH    ██    ██ ▀██▄   ▀█▄    ▄█▀ ██    ▀█▄ "
        "█\e[%d;%dH    ██    ██   ▀██▄   ▀████▀   ██      ▀██";

    // each title char is unicode
    // so it take more than 1 bytes
    //
    // each row contain close to 120 bytes
    char mainTitle[mainTitleMaxSize];
    sprintf(mainTitle, mainTitleFormat, y, x, y + 1, x, y + 2, x, y + 3, x,
            y + 4, x);

    addToString(buffer, mainTitle, mainTitleMaxSize);
}

void display_menu_options(int cursor, String *buffer, int menuIdx)
{
    const int init_y = ((G_CANVAS_HEIGHT * 7) / 8)
        - (NB_MENU_OPTION * G_MENU_OPTION_H_SIZE
           + (NB_MENU_OPTION - 1) * G_MENU_OPTION_H_MARGIN);

    for (int i = 0; i < NB_MENU_OPTION; i++)
    {
        char final_name[350];
        char temp_name[60];
        char line_buffer[180] = { 0 };

        const MenuOption option = optionList[menuIdx][i];
        sprintf(temp_name, "\e[3m%d - %s", i, option.name);

        const int option_y =
            init_y + (i * (G_MENU_OPTION_H_SIZE + G_MENU_OPTION_H_MARGIN));
        const int option_x = (G_CANVAS_WIDTH / 2) - (strlen(temp_name) / 2);

        /* option example:
          ╭┐ 1 - Tron
          ╰────────────■
        */

        for (size_t i = 0; i < strlen(temp_name); i++)
            strcat(line_buffer, "─");

        sprintf(final_name, "\e[3m\e[%d:%dH%s╭┐ %s\e[%d:%dH╰──%s■%s", option_y,
                option_x, (i == cursor ? option.color : NO_COLOR), temp_name,
                option_y + 1, option_x, line_buffer, NO_COLOR);

        addToString(buffer, final_name, 350);
    }
}

void clear_input(String *buffer)
{
    const int init_y = ((G_CANVAS_HEIGHT * 7) / 8)
        - (NB_MENU_OPTION * G_MENU_OPTION_H_SIZE
           + (NB_MENU_OPTION - 1) * G_MENU_OPTION_H_MARGIN);
    for (int i = 0; i < NB_MENU_OPTION; i++)
    {
        char final_name[250];
        char blank[75];
        int len = 30;

        memset(blank, ' ', len);
        blank[len] = '\0';

        const int option_y =
            init_y + (i * (G_MENU_OPTION_H_SIZE + G_MENU_OPTION_H_MARGIN));
        const int option_x = (G_CANVAS_WIDTH / 2) - (30 / 2);

        sprintf(final_name, "\e[3m\e[%d:%dH%s\e[%d:%dH%s", option_y, option_x,
                blank, option_y + 1, option_x, blank);

        addToString(buffer, final_name, 250);
    }
}

void display_menu(String *buffer, GAME_STATE state)
{
    if (state == MAIN_MENU)
    {
        int title_y = (G_CANVAS_HEIGHT / 4) - (MAIN_TITLE_HEIGHT / 2);
        int title_x = (G_CANVAS_WIDTH / 2) - (MAIN_TITLE_WIDTH / 2);

        display_main_title(buffer, title_x, title_y);
    }
    display_menu_options(0, buffer, (state == MULTI_MENU));
}
