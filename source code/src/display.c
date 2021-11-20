#include "display.h"
#include <stdio.h>
#include <locale.h>

/**< DISPLAY */
void textcolor (COLORS iColor) {        // copiado do conio2.c
     HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
     GetConsoleScreenBufferInfo(hl, &bufferInfo);
     bufferInfo.wAttributes &= 0x00F0;
     SetConsoleTextAttribute (hl, bufferInfo.wAttributes |= iColor);
}
void textbackground (COLORS iColor) {   // copiado do conio2.c
     HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
     GetConsoleScreenBufferInfo(hl, &bufferInfo);
     bufferInfo.wAttributes &= 0x000F;
     SetConsoleTextAttribute (hl, bufferInfo.wAttributes |= (iColor << 4));
}

void MarkIt(char _string[], COLORS f_color, COLORS b_color){
    // mudar a cor
    HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen, previous;
    GetConsoleScreenBufferInfo(hl, &screen);
    previous.wAttributes = screen.wAttributes;    // pegar informacao da cor anterior
    screen.wAttributes &= 0x00F0;
    if( b_color == -1)
        SetConsoleTextAttribute(hl,screen.wAttributes |= (f_color) );
    else
        SetConsoleTextAttribute(hl,f_color + b_color*0x10);
    // escrever a string
    printf("%s",_string);
    // colocar a cor nterior
    SetConsoleTextAttribute (hl, previous.wAttributes);
}

/**< END_DISPLAY */

const int menus_themes = 5;
//struct config_ini CONFIG;

void ConsoleDisplay(HANDLE* hStdout, SHORT COLS, SHORT LINES){
    char modescreen[33];
    sprintf(modescreen,"mode con cols=%hd lines=%hd ", COLS, LINES);
    system(modescreen);
    SMALL_RECT windowSize = {0, 0, COLS+1, LINES+1};
    COORD bufferSize = {COLS, LINES};
    SetConsoleScreenBufferSize(hStdout, bufferSize);
    SetConsoleTitle("SCHOOL SUBJECTS MANAGER");
    setlocale(LC_ALL, "Portuguese");
    SetConsoleWindowInfo(hStdout, 1, &windowSize);
    configInit();
    CLEAR_SCREEN;
    return;
}

void createTheme(char _theme[5][2]){
    int i;
    for(i = 0; i < menus_themes; i++){
        CONFIG.color[i][6] = _theme[i][0];
        CONFIG.color[i][7] = _theme[i][1];
    }
    system(CONFIG.color[3]);
}

void SetColor(UINT uFore,UINT uBack){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),uFore+uBack*0x10);
}

void softwareUpdate(const char* _LINK){
    printf("verificando.");
    char* link2Drive = (char*)calloc(100,sizeof(char));
    putchar('.');
    if(link2Drive != NULL){
        sprintf(link2Drive, "explorer %s", _LINK);
        putchar('.');
        system(link2Drive);
        printf("OK!");
        free(link2Drive);
    } else {
        printf("ERROR!");
    }
    return;
}

void configInit(void){
    int i;
    for(i = 0; i< menus_themes; i++){
        strcpy(CONFIG.color[i], "COLOR F0");
    }
    strcpy(CONFIG.numeric, "default");
    return;
}

void configLoad(void){
    char prop[15], vals[20];
    int i, fb;  /* fb - foreground background*/

    FILE* ini = fopen(CONFIG_INI, "r");
    if(ini){
        while( fscanf(ini, "%s %s", prop, vals) != EOF ){

            if( strcmp(prop, "theme:")==0 ){
                for(i = 0, fb = 0; i < menus_themes; i++){
                    CONFIG.color[i][6] = vals[fb++];
                    CONFIG.color[i][7] = vals[fb++];
                }
            }else if(strcmp(prop,"language:")==0){
                    if(strcmp(vals,"default")==0)        setlocale(LC_ALL, "");
                    else setlocale(LC_ALL, vals);
            }else if(strcmp(prop,"numeric:")==0){
                    strcpy(CONFIG.numeric,vals);
                    if(strcmp(CONFIG.numeric,"default")==0)     setlocale(LC_NUMERIC, "C");
                    else if(strcmp(CONFIG.numeric,"period")==0) setlocale(LC_NUMERIC, "en_US");
                    else if(strcmp(CONFIG.numeric,"comma")==0)  setlocale(LC_NUMERIC, "Portuguese");
                    else
                        setlocale(LC_NUMERIC, CONFIG.numeric);
            }
        }
        fclose(ini);
        printf("%s loaded.\n\n", CONFIG_INI);
    }else{
        printf("%s initiated.\n\n", CONFIG_INI);
    }
    return;
}

void configSave(void){
    int i;
    FILE* ini = fopen(CONFIG_INI, "w");
    if(ini){
        fputs("; subject manager settings.\n",ini);
        fprintf(ini,"theme: ");
            for(i =0; i < menus_themes; i++){
                fprintf(ini, "%c%c", CONFIG.color[i][6],CONFIG.color[i][7]);
            }
        fprintf(ini,"\nnumeric: %s", CONFIG.numeric);
        fputs("\n# to reset, delete this file.",ini);
        fclose(ini);
    }
    setlocale(LC_NUMERIC,CONFIG.numeric);
    return;
}

int erase_all_data(void){
    system("cls & color 0c");
    puts("Ao clicar em <SIM>:");
    puts("* Todos os seus dados desde programa serão apagados;");
    puts("** O programa será reiniciado.");
    puts("<!> Esta ação não há volta!!");
    FILE* rad = fopen("ssm-reset.vbs", "w");
    if(rad){
        system("attrib +h sm-reset.vbs");
        fputs(" Dim result, objFS, objFile \n", rad);
        fputs(" result=Msgbox(\"Tens a certeza que deseja prosseguir?\",vbYesNo+vbExclamation+vbApplicationModal, \"Apagar Todos os Dados\") \n", rad);
        fputs(" If result = vbYes Then \n",rad);
        fputs(" \tSet objFS = CreateObject(\"Scripting.FileSystemObject\") \n", rad);
        fputs(" \tSet objFile = objFS.DeleteFolder(\"data\", True) \n", rad);
        fputs(" End If \n", rad);
        fclose(rad);
        Sleep(20);
        system("cscript //nologo sm-reset.vbs");
    }
    FILE* result;
    remove("ssm-reset.vbs");
    if( (result = fopen(STUDENT_FILE, "r")) ){
        fclose(result);
        return 0;
    }
    else
        return 1;
}
