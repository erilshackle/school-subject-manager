#include "testes.h"
#include "subjects.h"

#include <string.h>
#include <time.h>

char* getTime(int date[5], bool Left){
    if(date == NULL) return "(null)";

    char* TheTime = (char*)malloc(26 * sizeof(char));
    struct tm theDate = {.tm_mday=date[0],          //  0 - dia
                         .tm_mon=date[1]-1 ,        //  1 - mes
                         .tm_year=date[2]-1900,     //  2 - ano
                         .tm_hour=date[3],          //  3 - hora
                         .tm_min=date[4]            //  4 - minutos
    };

    if( date[0] > 31 || date[0] < 1  || date[1] > 12 || date[1] < 1 ){
            free(TheTime);
            return "?/?/?";

    }else if(Left == true){
        int secs = difftime(mktime(&theDate),time(NULL));   // retornar o tempo estantes em segundos
        int dd = (int) secs/(3600*24),      // dia
            hh = (int) (secs/3600)%24,      // hora
            mn = (int) ((secs/60)%60)+1;    // minuto

        if(dd >= 31){
            strftime(TheTime,sizeof(TheTime), "%a %d às %H:%M", &theDate);
        }
        else if(dd >= 14){
            sprintf(TheTime,"daqui a %d dias", dd );
        }
        else if(dd > 7 && dd < 14){
            sprintf(TheTime,"1 semana & %d %s", dd-7, ( dd-7 == 0? "dia":"dias"));
        }
        else if(dd == 7){
            free(TheTime);
            return "daqui a 1 semana";
        }
        else if(dd > 0){
            sprintf(TheTime,"%d %s & %02d horas", dd, ( dd == 0? "dia":"dias"), hh);
        }
        else if(dd == 0 && hh > 0){
            sprintf(TheTime,"daqui a %d %s", hh, (hh==1?"hora":"horas") );
        }
        else if(dd==0 && hh == 0 && mn < 0){
            sprintf(TheTime,"em %d minutos", mn );
        }
        else if(dd==0 && hh == 0 && mn == 0){
            return "agora!";
        }
        else if(dd==0 && hh == 0 && mn >= 0){
            sprintf(TheTime,"passado %d minutos", abs(mn) );
        }
        else if(dd==0 && hh < 0){
            sprintf(TheTime,"%d %s atras", abs(hh), (hh==-1?"hora":"horas") );
        }
        else if(dd == -1 && hh <= 23){
            sprintf(TheTime,"ontem às %02i:%02i", date[3],date[4]);
        }
        else if(dd> -7){
            sprintf(TheTime,"%d dias atras", abs(dd));
        }
        else if(dd> -14){
            strftime(TheTime,sizeof(TheTime), "semana passada (%a %d)", &theDate);
        }
        else if(dd > -30){
            sprintf(TheTime,"ultimo dia %d", date[0]);
        }
        else if(dd > -181){
        strftime(TheTime,sizeof(TheTime), "paddado dia %a/%d", &theDate);
        }
        else{
            sprintf(TheTime,"desde %d/%02d/%04d", date[0],date[1],date[3]);
        }
    } else {
        strftime(TheTime,sizeof(TheTime), "%a %d %b  - %H:%M", &theDate);
    }
    return TheTime;
}
