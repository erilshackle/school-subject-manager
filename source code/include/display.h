#ifndef DISPLAY_H
#define DISPLAY_H

#include <windows.h>
#include <student.h>

#ifdef WIN32
    #define CLEAR_SCREEN    system("cls")
    #define PAUSE_SCREEN    system("pause >nul")
#else
    #define CLEAR_SCREEN    system("clear")
    #define PAUSE_SCREEN    scanf("%*c")
#endif // WIN32

#define PRESS_ENTER (op=getchar())==0x0A
#define WAIT_PRESSKEY   system("pause")
#define WAIT(seconds)   Sleep(seconds*1000)

#define CONFIG_RUN configInit()
#define CONFIG_INI "ssm_data/config.ini"
#define CONFIG_DEL remove(CONFIG_DIR)
#define CONFIG_OPEN system("start notepad.exe ssm_data/config.ini")

/**< DISPLAY */

typedef enum CORES_DOS {Black,Blue, Green, Aqua, Red, Purple, Yellow, White,
            Gray,LBlue,LGreen,LAqua,LRed,LPurple,LYellow,BWhite, BackgroundColor = -1 }COLORS;
//! CONIO.H
void textcolor (COLORS iColor);
void textbackground (COLORS iColor);
void resetTextcolor(COLORS menu_foreground);
void MarkIt(char _string[], COLORS f_color, COLORS b_color);
/**< END_DISPLAY */

struct config_ini{
    char color[5][9];
    char numeric[15];
}CONFIG;

void ConsoleDisplay(HANDLE* hStdout, SHORT COLS, SHORT LINES);
void createTheme(char _theme[5][2]);
void SetColor(UINT uFore,UINT uBack);

void softwareUpdate(const char* _LINK);

void configInit(void);
void configLoad(void);
void configSave(void);
void configFree(void);

int erase_all_data(void);

#endif // DISPLAY_H
