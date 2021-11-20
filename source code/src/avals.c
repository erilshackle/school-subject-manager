#include "avals.h"
#include "subjects.h"

const char* strMonth[] = {"Jan","Fev","Mar","Abr","Mai","Jun","Jul","Ago","Set","Out","Nov","Dez"};
const char* strDWeek[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


void list_avals(SUBJECTS* DCP, bool indexed_1base){
    int i, exame = DCP->exame.active; char mark[6];

    if( (DCP->numAvals + exame) == 0){
        puts("Não há Avalição marcada para esta disciplina.");
        return;
    }

    for( i = 0; i < DCP->numAvals; i++){
        if(DCP->aval[i].feito) gcvt(DCP->aval[i].nota,4,mark); else strcpy(mark,"---");
        printf(" %c <%c %-8s > %-20s\t Peso = %02i%%\t Nota = %s\n", indexed_1base? 0x31+i:'\b',indexed_1base? '<':'\r'
            , getAvalType(DCP->aval[i].tipo),   getTimeTo(DCP->aval[i].data),   DCP->aval[i].peso,  mark
        );
    }
    if(exame){
        if(DCP->exame.feito) gcvt(DCP->exame.nota,4,mark); else strcpy(mark,"---");
        printf(" %c <%c %-8s > %-20s\t Peso = %02i%%\t Nota = %s\n", indexed_1base? 0x31+i:'\b',indexed_1base? '<':'\r'
            , getAvalType(EXAME), getTimeTo(DCP->exame.data), 100, mark
        );
    }
    return;
}

int count_aval_type(SUBJECTS* dcp, AVAL_TYPE _type){
    int i, nums = 0;
    for(i = 0; i < dcp->numAvals; i++)  if(dcp->aval[i].tipo == _type)  nums++;
    return nums;
}

int getAvalStatus(SUBJECTS* dcp){
    int i, status = 0;
    if(dcp->exame.active == true)       return 100;
    for(i = 0; i < dcp->numAvals; i++)  status = status + dcp->aval[i].peso;
    return status;
}

int getThisAvalPeso(SUBJECTS* dcp, AVAL_TYPE type){
    int i, peso = 0;
    /// caso Exame
    if( type == EXAME)
        if(dcp->exame.active == true)
            return EXAME;   // EXAME possui um valor de 100 declarado no enum em subjects.h

    for(i = 0; i < dcp->numAvals; i++){
        if( dcp->aval[i].tipo == type )
            peso += dcp->aval[i].peso;
    }
    return peso;
}

char* getAvalType(AVAL_TYPE _tipo){
    switch(_tipo){
        case TESTE: return "Teste";
        case PROJETO: return "Projeto";
        case OEA: return "O.E.A";
        case EXAME: return "Exame";
         default: return "Avaliacao";
    }
}

char* getTimeTo(int date[5]){
    if(date == NULL) return "(null)";
    if( date[2] == NONE && date[1] == NONE && date[2] == NONE )return "sem eventos";
    char* TheTime = (char*)malloc(21*sizeof(char));
    struct tm theDate = {.tm_mday=date[0],          //  0 - dia
                         .tm_mon =date[1]-1 ,       //  1 - mes
                         .tm_year=date[2]-1900,     //  2 - ano
                         .tm_hour=date[3],          //  3 - hora
                         .tm_min =date[4]           //  4 - minutos
    };
    int secs = difftime(mktime(&theDate),time(NULL));   // retornar o tempo estantes em segundos
    bool thisYear = secs/(86400*366) == 0? true : false;
    if ( thisYear ){
        sprintf(TheTime, "%s, %d %s as %02d:%02d",strDWeek[theDate.tm_wday],
                                                    theDate.tm_mday,
                                                    strMonth[theDate.tm_mon],
                                                    theDate.tm_hour,
                                                    theDate.tm_min);
    } else {
        sprintf(TheTime, "%i %s %04i",   theDate.tm_mday,
                                                        strMonth[theDate.tm_mon],
                                                        theDate.tm_year+1900);
    }
    return TheTime;
}

char* getTimeLeft(int date[5]){
    if(date == NULL) return "(null)";
    /* Day/Month/Year/Hour/Minutes */
    char* timeleft = (char*)malloc(26 * sizeof(char));
    struct tm theDate = {.tm_mday=date[0],
            .tm_mon  = date[1]-1 ,
            .tm_year = date[2]-1900,
            .tm_hour = date[3],
            .tm_min  = date[4]
    };
    int secs = difftime(mktime(&theDate),time(NULL));   // retornar o tempo estantes em segundos
    int dd   = secs/(3600*24),
        hh   = (secs/3600)%24,
        mn   = (int) ((secs/60)%60)+1,
        ano  = secs/(86400*366);

    if( date[0] > 31 || date[0] < 1  || date[1] > 12 || date[1] < 1 ){
        sprintf(timeleft,"?/?/?");
    }
    else if(ano > 0){
        return getTimeTo(date);
    }
    else if(dd > 31){
        sprintf(timeleft,"%d %s as %02d:%02d ", date[0], strMonth[date[1]-1], hh,mn);
    }else if(dd >= 14){
        sprintf(timeleft,"daqui a %d dias", dd );
    }
    else if(dd > 7 && dd < 14){
        sprintf(timeleft,"em 1 semana & %d %s", dd-7, ( dd-7 == 0? "dia":"dias"));
    }
    else if(dd == 7){
        free(timeleft);
        return "daqui a 1 semana";
    }
    else if(dd > 0){
        sprintf(timeleft,"em %d %s & %02d horas", dd, ( dd == 1? "dia":"dias"), hh);
    }
    else if(dd == 0 && hh > 0){
        sprintf(timeleft,"daqui a %d %s", hh, (hh==1?"hora":"horas") );
    }
    else if(dd==0 && hh == 0 && mn < 0){
        sprintf(timeleft,"em %d minutos", mn );
    }
    else if(dd==0 && hh == 0 && mn == 0){
        return "A decorrer no momento!";
    }
    else if(dd==0 && hh == 0 && mn >= 0){
        sprintf(timeleft,"passado %d minutos", abs(mn) );
    }
    else if(dd==0 && hh < 0){
        sprintf(timeleft,"%d %s atras", abs(hh), (hh==-1?"hora":"horas") );
    }
    else if(dd == -1 && hh <= 23){
        sprintf(timeleft,"ontem as %02i:%02i", date[3],date[4]);
    }
    else if(dd> -7){
        sprintf(timeleft,"%d dias atras", abs(dd));
    }
    else if(dd> -14){
        strftime(timeleft,sizeof(timeleft), "semana passada (%a %d)", &theDate);
    }
    else if(dd > -30){
        sprintf(timeleft,"ultimo dia %d", date[0]);
    }
    else if(dd > -181){
        strftime(timeleft,sizeof(timeleft), "passado dia %a/%d", &theDate);
    }
    else{
        sprintf(timeleft,"desde %d %s %04d", date[0],strMonth[date[1]-1],date[2]);
    }
    return timeleft;
}

/// aux

char* returnMiniData(int *data){
    char* minidata = (char*)malloc(6 * sizeof(char));
    struct tm theDate = {.tm_mday = data[0], .tm_mon = data[1]-1 , .tm_year = data[2]-1900};
    if ( (difftime(mktime(&theDate),time(NULL))/(86400*366)) != 0 )
        sprintf(minidata,"%2d/%-2d",*(data+1),*(data+2)-2000);
    else
        sprintf(minidata,"%2d/%-2d",*(data),*(data+1));
    return minidata;
}

int indexOfAval(SUBJECTS* disciplina, AVAL_TYPE type){
    int i;
    for(i = 0; i < disciplina->numAvals; i++)
        if( disciplina->aval[i].tipo == type)
            return i;
    return -1;
}

void copyAvalTo(SUBJECTS* DCP, int to_idx, int from_idx){
    int i;
    for(i = 0; i < 5; i++) DCP->aval[to_idx].data[i] = DCP->aval[from_idx].data[i];
    DCP->aval[to_idx].feito = DCP->aval[from_idx].feito;
    DCP->aval[to_idx].tipo = DCP->aval[from_idx].tipo;
    DCP->aval[to_idx].nota = DCP->aval[from_idx].nota;
    DCP->aval[to_idx].peso = DCP->aval[from_idx].peso;
    return;
}

bool check_data(int data[5], bool report){
    if(data[0] > 31 || data[0] < 1){
        if(report)  printf("Erro! verifique o DIA da data: %d\n", data[0]);
    }else if(data[1] > 12 || data[1] < 1){
        if(report)  printf("Erro! verifique o MES da data: %02d\n", data[1]);
    }else if(data[2] < 2000 && data[2] >= 1000){
        if(report)  printf("Erro! verifique o ANO da data: %04d\n", data[2]);
    }else{
        if(data[2] < 1000) data[2] += 2000;
        if(report)  printf("OK! %d %s %04d\n", data[0],strMonth[data[1]-1],data[2]);
        return true;
    }
    return false;
}

/// AVALIACOES

bool set_mark_to(SUBJECTS* DCP, int aval_idx, float *mark_debug){
    int i;
    float input_mark = 0;
    char curr_mark[6];
    bool isExame =  ( (DCP->exame.active) && (aval_idx == DCP->numAvals) ) ? true : false;
    AVAL_TYPE tipo_aval = isExame ? EXAME : DCP->aval[aval_idx].tipo;

    if(isExame)
        if(DCP->exame.feito) gcvt(DCP->exame.nota,4,curr_mark); else strcpy(curr_mark,"---");
    else
        if(DCP->aval[aval_idx].feito) gcvt(DCP->aval[aval_idx].nota,4,curr_mark); else strcpy(curr_mark,"---");

    printf("> Nota do %s (%s) << ", getAvalType(tipo_aval), curr_mark);

    scanf("%f", &input_mark);

    if(input_mark < 0.0000 || input_mark > NOTA_MAXIMA){
        if (mark_debug != NULL) *mark_debug = input_mark;
        return false;
    }

    if(isExame){
        DCP->exame.nota = input_mark;
        for(i = 0; i < 5; i++)  DCP->lastEvent[i] = DCP->exame.data[i];
        DCP->exame.feito = true;
    }else{
        DCP->aval[aval_idx].nota = input_mark;
        for(i = 0; i < 5; i++)  DCP->lastEvent[i] = DCP->aval[aval_idx].data[i];
        DCP->aval[aval_idx].feito = true;
    }

    if (mark_debug != NULL) *mark_debug = input_mark;
    return true;
}

bool un_mark_to(SUBJECTS* DCP, int aval_idx){
    aval_idx *= aval_idx < 0 ? -1 : 1 ;
    bool isExame =  ( (DCP->exame.active) && (aval_idx == DCP->numAvals) ) ? true : false;

    if(isExame){
        if(DCP->exame.feito == true){
            DCP->exame.nota = 0.00000;
            DCP->exame.feito = false;
        } else return false;
    }else{
        if(DCP->aval[aval_idx].feito == true){
            DCP->aval[aval_idx].nota = 0.00000;
            DCP->aval[aval_idx].feito = false;
        } else return false;
    }
    return true;
}

int marcar_avaliacao(SUBJECTS* DCP, AVAL_TYPE tipo_aval, int _data[5], int peso){
    int index = -3, i;
    char* avalicao = getAvalType(tipo_aval);
    if(_data[2] < 1900) _data[2] += 2000; else if(_data[2] <= 2000 )  _data[2] = 2000;

    //! condições
    // numero de avalicoes, error may return -3;
    if( DCP->numAvals < MAX_AVAL )
    {index++;
    // data, error may return -2
    if(check_data(_data,false))
    {   index++;
    // peso, error may return -1
    if( (peso + getAvalStatus(DCP)) <= 100 && peso > 0)
    {   index = DCP->numAvals;
        // veficar avaliacoes tipo OEA e PROJETO, que devem ser unicos
        if(tipo_aval == OEA || tipo_aval == PROJETO)
        if(count_aval_type(DCP,tipo_aval) > 0){
            printf("Erro! Nao pode marcar mais %s para esta disciplina.\n", avalicao);
            return -4;  // cant schedule more aval of this type
        }
         // OK! vamos marcar a avalicao!
        DCP->aval[index].tipo = tipo_aval;                              // tipo de avaliação definida
        for( i = 0; i < 5; i++) DCP->aval[index].data[i] = _data[i];    // data definida
        DCP->aval[index].peso = peso;                                   // peso definido
        DCP->aval[index].feito = false;                                 // marcado como nao feito
        DCP->aval[index].nota = 0.000000;                               // nota 0 por ora

        DCP->numAvals += 1;                                             // aumentado o numero de avalicaoes da disciplina
    }}}
    switch(index){
    case -1: printf("Erro! Peso para o %s excede o limite \t\tlim = %i%%\n", avalicao, 100-getAvalStatus(DCP)); break;
    case -2: printf("Erro! Data de %s invalida \t\t\t %d/%02d/%04d\n", avalicao, _data[0],_data[1],_data[2]);  break;
    case -3: printf("Erro! %s excede o numero de avaliaoes permitidas \t\t max = %d\n", DCP->titulo, MAX_AVAL);  break;
    default: printf(" %i* %s (%i%%) marcado com sucesso para %s\n",
                    count_aval_type(DCP,tipo_aval), avalicao, peso,
                    tipo_aval != OEA ? getTimeTo(DCP->aval[index].data) : "\b\b\b\b\b\b.     ");
    }
    return index;
}

bool marcar_exame(SUBJECTS* DCP, int _data[5]){
    int i;
    //! condições
    if(_data[2] < 1900) _data[2] += 2000; else if(_data[2] <= 2000 )  _data[2] = 2000;
    if( _data[0] > 31 || _data[0] < 1 || _data[1] > 12 || _data[1] < 1 ||   DCP->exame.active == true                    // exame ja foi marcado antes?
    ){  printf("> Falha ao marcar exame para %d/%02d/%04d as %02d:%02d\n", _data[0],_data[1],_data[2],_data[3],_data[4]);
        return false;
    }
    //! ------
    // data
    for(i = 0; i < 5; i++)  DCP->exame.data[i] = _data[i];
    // defaults
    DCP->exame.feito = false;
    DCP->exame.nota = 0.000;
    DCP->exame.active = true;
    printf("> Exame marcado com sucesso para %s\n", getTimeTo(DCP->exame.data));
    return true;
}

bool editar_avaliacao(SUBJECTS* DCP, int aval_idx){
    int  i, input_data[5], input_peso;
    bool isExame = false;
    if(DCP->exame.active) isExame = aval_idx == (DCP->numAvals) ? true: false;
    AVAL_TYPE tipo_aval = isExame ? EXAME : DCP->aval[aval_idx].tipo;
    int status = 100;
    char avaliacao[10], _f = '/';
    // valores antes de editados
    int dia, mes, ano, hora, min, peso;

    if(isExame){
        input_data[0] = dia = DCP->exame.data[0];   input_data[1] = mes = DCP->exame.data[1];
        input_data[2] = ano = DCP->exame.data[2];   input_data[3] = hora = DCP->exame.data[3];
        input_data[4] = min = DCP->exame.data[4];   input_peso = peso = 100;
        strcpy(avaliacao,"Exame");
        printf("Editar informacoes do %s de %s\n- - -\n", avaliacao, getTimeTo(DCP->exame.data));
    }else{
        input_data[0] = dia = DCP->aval[aval_idx].data[0];   input_data[1] = mes = DCP->aval[aval_idx].data[1];
        input_data[2] = ano = DCP->aval[aval_idx].data[2];   input_data[3] = hora = DCP->aval[aval_idx].data[3];
        input_data[4] = min = DCP->aval[aval_idx].data[4];   input_peso = peso = DCP->aval[aval_idx].peso;
        status = getAvalStatus(DCP) - DCP->aval[aval_idx].peso;
        strcpy(avaliacao,getAvalType(tipo_aval));
        printf("Editar informacoes do %s de %s\n- - -\n", avaliacao, getTimeTo(DCP->aval[tipo_aval].data));
    }
    if(tipo_aval != OEA){
        _data_:
        printf("> DATA { Dia/Mes/Ano } <<  ");
        scanf("%i%c%i%c%i", &input_data[0], &_f, &input_data[1], &_f, &input_data[2]);
//        printf("~~~~~~~~~~~~~~~~~> ");
        if(input_data[0] == 0) input_data[0] = dia;
        if(input_data[1] == 0) input_data[1] = mes;
        if(input_data[2] == 0) input_data[2] = ano;
        if(check_data(input_data, true) == false) goto _data_;
    }
    if(tipo_aval != OEA){
        printf("> HORA { Hora:Minuto } <<  ");
        scanf("%i%c%i", &input_data[3], &_f, &input_data[4]);
            if(input_data[3] == 0) input_data[3] = hora;
            if(input_data[4] == 0) input_data[4] = min;
    }
    if(!isExame){
        _peso_:
        printf("> PESO { peso }        << %%          (peso max = %-3d)", 100-status);for(i = 0; i < 26; i++) putchar('\b');
        scanf("%i", &input_peso);
        if(input_peso==0) input_peso = peso;
        else if(input_peso < 0 || input_peso > 100-status) goto _peso_;
    }

    // OK! Modificar
    if(isExame){
        for(i = 0; i < 5; i++) DCP->exame.data[i] = input_data[i];
    } else {
        for(i = 0; i < 5; i++) DCP->aval[aval_idx].data[i] = input_data[i];
        DCP->aval[aval_idx].peso = input_peso;
    }
    printf("\n> %s foi alterado >> peso: %d%% \t data: %s;\n",
                    avaliacao, input_peso,
                    tipo_aval != OEA ? getTimeTo(DCP->aval[aval_idx].data) : "\b\b\b\b\b\b\b\b\b");
    return true;
}

bool eliminar_avaliacao(SUBJECTS* DCP, int aval_idx){
    int i, last = DCP->numAvals-1;
    // se indx == numavals então é exame que se está a eliminar. isso porque não podemos recer input especifico para exame
    if(aval_idx == DCP->numAvals && DCP->exame.active){

        if(DCP->exame.feito)    printf("> Exame de %s Elimindado com sucesso\n", getTimeTo(DCP->exame.data));
        else    printf("> Exame de %s Desmarcado com sucesso\n", getTimeTo(DCP->exame.data));

        for(i = 0; i < 5; i++) DCP->exame.data[i] = 0;
        DCP->exame.active = false;
        DCP->exame.feito = false;
        DCP->exame.nota = 0.00;

        return true;
    }else{
        printf("> %s de %s foi Eliminado com sucesso\n", getAvalType(DCP->aval[aval_idx].tipo)
                                                       , getTimeTo(DCP->aval[aval_idx].data));
        if(aval_idx != last){
            for(i = 0; i < 5; i++) DCP->aval[aval_idx].data[i] = DCP->aval[last].data[i];
            DCP->aval[aval_idx].feito = DCP->aval[last].feito;
            DCP->aval[aval_idx].tipo = DCP->aval[last].tipo;
            DCP->aval[aval_idx].nota = DCP->aval[last].nota;
            DCP->aval[aval_idx].peso = DCP->aval[last].peso;
        }
        DCP->numAvals -= 1;

        return true;
    }
    return false;
}

/**< MENU AGENDAR TESTE */

void _agendar_avaliacao_(SUBJECTS* Disciplina){
    time_t tempo = time(NULL);	struct tm* today = localtime(&tempo);
    /// VARS
    char op; int status, i; bool schedule = true;
    bool exame  = Disciplina->exame.active,
        projeto = count_aval_type(Disciplina,PROJETO),
        oea     = count_aval_type(Disciplina,OEA);
    int P_index = indexOfAval(Disciplina,PROJETO);
    // variaveis de inputs
    int input_num, input_peso, input_data[5];
    char input[2];
    /// MENU
    do{ CLEAR_SCREEN; status = getAvalStatus(Disciplina);
        schedule = (status != 100 && Disciplina->numAvals != MAX_AVAL); // verifica se pode-se marcar avaliações
        exame = Disciplina->exame.active;
        // update main info
        printf(" GERIR AGENDA DE AVALIAÇÕES PARA %s\n", Disciplina->titulo);
        puts("=====================================================================");
        printf(" Avaliacoes Marcadas: %d / %i   \t",Disciplina->numAvals + exame,MAX_AVAL);
        printf("Taxa de Agendamento:%s%d%%\n", getAvalStatus(Disciplina) == 100? " cheia ":" ", getAvalStatus(Disciplina));
        puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        printf(" TESTE: %i (%d%%)\t", count_aval_type(Disciplina, TESTE), getThisAvalPeso(Disciplina,TESTE));
/*OEA*/ printf("O.E.A: ");  if(oea == true) printf("%d%%\t", getThisAvalPeso(Disciplina, OEA));    else    printf("-- \t");
        printf("PROJETO: %s (%d%%)\t", projeto == true? returnMiniData(Disciplina->aval[P_index].data):"--", getThisAvalPeso(Disciplina, PROJETO));
        printf("EXAME: %s\n", exame == true? returnMiniData(Disciplina->exame.data):"--");
        puts("=====================================================================");
        if(status != 100)
            printf("\n\tVoce ainda pode marcar ate %d avaliacoes / %i%% de peso\n\n", MAX_AVAL - Disciplina->numAvals, 100-status);
        else
            printf("\n\t\t%s já possui 100%% de avaliacoes agendadas\n\n", Disciplina->titulo);
        puts("---------------------------------------------------------------------");
        if(schedule == true){
            printf("  T  <<  Marcar Teste \t\t\t\t\n");
            if(!projeto)
            printf("  P  <<  Marcar Projeto \t\t\t\t\n");
            if(!oea)
            printf("  O  <<  Marcar O.E.A \t\t\t\t\n");
        }
        if(!exame)
        printf("  X  <<  Marcar Exame \t\t\t\t\n");
        else{
            printf("    <<  Nao pode agendar mais avaliações >>\n");
        }
        puts("- - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - -");
        printf("  .  <<  Editar data/hora/peso de uma avaliação\n");
        printf("  -  <<  Eliminar Avaliação\n");
        puts("- - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - -");

        puts  ("  0  << Voltar");
        puts("---------------------------------------------------------------------\n");
        printf("Escolher << "); fflush(stdin);
        op = getchar();

        //! cases
        switch(op){
            /**< ------------------------------------------------ TESTE --------------- */
            case't': case'T':{    if(!schedule) break;
                CLEAR_SCREEN;
                for(i = 0; i < 70; i++) putchar('-');   putchar('\n');
                    printf("\t\t MARCAR TESTE PARA DISCIPLINA DE %s\n\n", Disciplina->titulo);
                for(i = 0; i < 70; i++) putchar('-');//   putchar('\n');
                printf("! voce ainda pode marcar mais %d avalicaoes deste tipo\n", MAX_AVAL - Disciplina->numAvals);
                puts("! Informe a data, hora e o peso desse teste.\n");
                // data
                printf("> DIA do teste (1~31)  <<                (0 - cancelar)");for(i = 0; i < 29; i++) putchar('\b');
                    scanf("%d", &input_data[0]);                 if(input_data[0]==0) break;
                printf("> MES (1~12)           << ");
                    scanf("%d", &input_data[1]);
                printf("> ANO (+2000)          << ");
                    scanf("%d", &input_data[2]);
                // hora
                printf("> HORA (0~23)          << ");
                    scanf("%d", &input_data[3]);
                printf("> MINUTOS (0~60)       << ");
                    scanf("%d", &input_data[4]);
                // peso
                printf("\n> PESO do teste (1~%d%%) << ", 100- status);
                    scanf("%d", &input_peso);                 if(input_peso==0) break;

                puts("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                marcar_avaliacao(Disciplina, TESTE, input_data, input_peso);
                puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                printf("OK...");
                PAUSE_SCREEN;
                break;
            }
            /**< ----------------------------------------------- PROJETO ------------- */
            case'p': case'P':{    if(!schedule || projeto) break;
                CLEAR_SCREEN;
                for(i = 0; i < 70; i++) putchar('-');   putchar('\n');
                    printf("\t\t MARCAR PROJETO DA DISCIPLINA DE %s\n\n", Disciplina->titulo);
                for(i = 0; i < 70; i++) putchar('-');//   putchar('\n');
                puts("! Voce so pode marcar este tipo de avaliacao 1 vez");
                puts("! Informe a data, hora e o peso do Projeto.\n");
                    // data
                printf("> DIA do projeto (1~31)  <<                (0 - cancelar)");for(i = 0; i < 29; i++) putchar('\b');
                    scanf("%d", &input_data[0]);                    if(input_data[0]==0) break;
                printf("> MES (1~12)             << ");
                    scanf("%d", &input_data[1]);
                printf("> ANO (+2000)            << ");
                    scanf("%d", &input_data[2]);
                // hora
                printf("> HORA (0~23)            << ");
                    scanf("%d", &input_data[3]);
                printf("> MINUTOS (0~60)         << ");
                    scanf("%d", &input_data[4]);
                // peso
                printf("\n> PESO do projeto (1~%d%%) << ", 100- status);
                    scanf("%d", &input_peso);

                puts("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                if( marcar_avaliacao(Disciplina, PROJETO, input_data, input_peso) >=0 ){
                    projeto = true;
                    P_index = indexOfAval(Disciplina,PROJETO);
                }
                puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                printf("OK...");
                PAUSE_SCREEN;

                break;
            }
            /**< ----------------------------------------------- O.E.A. ------------- */
            case'o': case'O':{    if(!schedule) break;
                CLEAR_SCREEN;
                for(i = 0; i < 70; i++) putchar('-');   putchar('\n');
                    printf("     MARCAR OUTROS ELEMENTOS DE AVALIAÇÃO PARA DISCIPLINA DE %s\n\n", Disciplina->titulo);
                for(i = 0; i < 70; i++) putchar('-');//   putchar('\n');
                puts("! Voce so pode marcar este tipo de avaliacao 1 vez");
                puts("! Informe o peso para a avaliacao O.E.A.\n");
                    // peso
                printf("> PESO do O.E.A (1~%d%%)  <<                (0 - cancelar)", 100-status ); for(i = 0; i < 29; i++) putchar('\b');
                    scanf("%d", &input_peso);                       if(input_peso == 0) break;
                    // data
                input_data[0] = today->tm_mday;         input_data[1] = today->tm_mon+1;
                input_data[2] = today->tm_year+1900;    input_data[3] = today->tm_hour;
                input_data[4] = today->tm_min;

                puts("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                oea = marcar_avaliacao(Disciplina, OEA, input_data, input_peso);
                puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                printf("OK...");
                PAUSE_SCREEN;

                break;
            }
            /**< ----------------------------------------------- EXAME ------------- */
            case'x': case'X':{    if(exame == true) break;

                CLEAR_SCREEN;
                for(i = 0; i < 70; i++) putchar('-');   putchar('\n');
                    printf("\t\t MARCAR EXAME DA DISCIPLINA DE %s\n\n", Disciplina->titulo);
                for(i = 0; i < 70; i++) putchar('-');//   putchar('\n');
                puts("! Marcando exame voce nao pode marcar nenhuma outra avaliacao.");
                puts("! Informe a data e hora do Exame.\n");
                    // data
                printf("> DIA do exame (1~31)  <<                (0 - cancelar)");for(i = 0; i < 29; i++) putchar('\b');
                    scanf("%d", &input_data[0]);                  if(input_data[0]==0) break;
                printf("> MES (1~12)           << ");
                    scanf("%d", &input_data[1]);
                printf("> ANO (+2000)          << ");
                    scanf("%d", &input_data[2]);
                // hora
                printf("> HORA (0~23)          << ");
                    scanf("%d", &input_data[3]);
                printf("> MINUTOS (0~60)       << ");
                    scanf("%d", &input_data[4]);

                puts("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                exame = marcar_exame(Disciplina,input_data);
                puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                printf("OK...");
                PAUSE_SCREEN;
                break;
            }
            case'.':{
                _EDITAR_:
                CLEAR_SCREEN;
                printf("\t     EDITAR UMA AVALICAO PARA DISCIPLINA DE %s\n", Disciplina->titulo);
                for(i = 0; i < 70; i++) putchar('~');   putchar('\n');
                    list_avals(Disciplina,true);    putchar('\n');
                 for(i = 0; i < 70; i++) putchar('~');//   putchar('\n');

                puts("! Escolha a avaliacao que deseja Modificar.");
                puts("! Entre com numero da avalicao.\n");

                 // user input
                printf("> Editar Avaliacao   <<                (0 - voltar)"); for(i = 0; i < 27; i++) putchar('\b');
                    fflush(stdin);
                    scanf("%s", input);
                    input_num = atoi(input);
                    if(input_num == 0) break;

                // check valid input
                if(input_num < 0 || input_num > Disciplina->numAvals + exame) goto _EDITAR_;

                puts("\n\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                    editar_avaliacao(Disciplina, input_num-1);
                puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");

                printf("OK...");
                PAUSE_SCREEN;

                break;
            }
            case'-':{
                _ELIMINAR_:
                CLEAR_SCREEN;
                printf("\t     ELIMINAR UMA AVALICAO PARA DISCIPLINA DE %s\n", Disciplina->titulo);
                for(i = 0; i < 70; i++) putchar('~');   putchar('\n');
                    list_avals(Disciplina,true);    putchar('\n');
                 for(i = 0; i < 70; i++) putchar('~');//   putchar('\n');

                puts("! Escolha a avaliacao que deseja Eliminar.");
                puts("! Entre com  numero da avalicao.\n");

                 // user input
                printf("> Eliminar Avaliacao   <<                (0 - cancelar)"); for(i = 0; i < 29; i++) putchar('\b');
                    scanf("%d", &input_num);                       if(input_num == 0) break;

                // check valid input
                if(input_num < 0 || input_num > Disciplina->numAvals + exame) goto _ELIMINAR_;

                puts("\n\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
                    eliminar_avaliacao(Disciplina, input_num-1);
                puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");

                projeto = count_aval_type(Disciplina, PROJETO) == 1 ? true : false;
                oea = count_aval_type(Disciplina, OEA) == 1 ? true : false;
                exame = Disciplina->exame.active;

                printf("OK...");
                PAUSE_SCREEN;

                break;
            }
        }
    }while(op != '0');
    free(today);
    return;
}

void _avaliamento_(SUBJECTS* Disciplina){
    int i, input_num;
    bool exame = Disciplina->exame.active, success_set, success_unset;
    float mark_debug;   // bring back the mark inputted on the function set_mark_to
    char input[4];
    do{
        CLEAR_SCREEN;
        success_set = success_unset = false;
        printf("\t     ATRIBUIR NOTA A UMA AVALIACAO DE %s\n", Disciplina->titulo);
        for(i = 0; i < 70; i++) putchar('~');   putchar('\n');
            list_avals(Disciplina,true);        putchar('\n');
        for(i = 0; i < 70; i++) putchar('~');//   putchar('\n');
        puts("! Escolha o numero da avaliacao para atribuir a Nota.");
        puts(": Para marcar a avalicao como NAO FEITO, entre com o simetrico (-n).");
        puts("! Entre com zero (0) para voltar.\n");

        _ATTRIBMARK_:
        // user input
        printf("> N* da Avaliacao   <<                (0 - voltar)"); for(i = 0; i < 27; i++) putchar('\b');
        fflush(stdin);
        scanf("%s", input);
        input_num = atoi(input);
        if(input_num == 0) break;
        // check valid input
        fflush(stdin); putchar('\n');
        if(input_num > 0 && input_num <= (Disciplina->numAvals + exame) )
            success_set = set_mark_to(Disciplina, input_num-1, &mark_debug);

        else if (input_num < 0 && input_num >= (Disciplina->numAvals + exame)*-1)
            success_unset = un_mark_to(Disciplina, abs(input_num+1) );

        else
            goto _ATTRIBMARK_;

        puts("\n\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
            if(success_set){
                    printf("> Nota %.2f atribuida a avaliacao com sucesso\n ", mark_debug);
            } else if(success_unset)
                puts("> OK!! Avalicao marcada como nao feita:\n  Ela aparecera na sua agenta e nao tera a nota atribuida.\n Essa avalicao ficara fora dos calculos por ora");
            else{
                if(input_num > (Disciplina->numAvals + exame))
                    printf("> Nao foi possivel atribuit a nota %.2f para a avaliacao\n", mark_debug);
                else
                    puts("> Erro! Nao pode marcar avaliacao que possui nota atribuida como NAO FEITO");
            }
        puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
        PAUSE_SCREEN;
    }while(input_num != 0);
    /*! calcular a media final da disciplina, a media final so pode sofrer alteracao apos uso desta funcao, entao */
    calc_subject_media(Disciplina);
    return;
}
