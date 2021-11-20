#include "student.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



STUDENT inicializar_student(void){   // inicializa dados do estudadnte
    STUDENT Aluno;
    strcpy(Aluno.nome, getenv("username"));
    strcpy(Aluno.curso, "C.U.R.S.O");
    strcpy(Aluno.ID, "xxxxxx");
    Aluno.idade = 18;
    Aluno.letivo = 1;
    Aluno.media_final = 0.00000000;
    Aluno.disciplina = NULL;
    _NUM_DISCIPLINAS_ATUAL = 0;
    return Aluno;
}

/**< setters *///
void set_student_nome(STUDENT* Aluno){
    int i;
    char _nome[MAX_NAME];   printf("Nome << ");
    for(i =0;i < MAX_NAME-1; i++)putchar('.');
    for(i =0;i < MAX_NAME-1; i++)putchar('\b');
    fflush(stdin);          gets(_nome);
    if(strcmp(_nome,"")==0 || strlen(_nome) >= MAX_NAME) return;
    make_captalized(_nome);
    strcpy(Aluno->nome,_nome);
    return;
}
void set_student_curso(STUDENT* Aluno){
    int i, letivo,limit = sizeof(Aluno->curso)/sizeof(char)-1;
    char _curso[limit+1];
    printf("Curso << ");    fflush(stdin);
    for(i =0;i < limit; i++)putchar('.');
    for(i =0;i < limit; i++)putchar('\b');
    gets(_curso);
    if(strcmp(_curso,"")==0 || strlen(_curso) > limit) return;
    strcpy(Aluno->curso,_curso);
    printf("Ano do curso (int) << #\b");
    set_letivo:
    scanf("%d", &letivo);
    if(letivo == 0) goto set_letivo;
    Aluno->letivo = letivo < 0 ? letivo * (-1) : letivo ;
    return;
}
void set_student_ID(STUDENT* Aluno){
    int i; char _ID[11];
    printf("ID << ");
    for(i =0;i < sizeof(Aluno->ID)/sizeof(char)-1; i++)putchar('.');
    for(i =0;i < sizeof(Aluno->ID)/sizeof(char)-1; i++)putchar('\b');
    fflush(stdin);scanf("%s", _ID);
    strcpy(Aluno->ID,_ID);
    return;
}
bool set_student_idade(STUDENT* Aluno){
    unsigned int ano, mes, dia, i;
    printf("Dia de Aniversario (int) :                (0 - cancelar e voltar)");  for(i = 0; i < 38; i++) putchar('\b');
         scanf("%u", &dia);     if(dia == 0) return true;
    printf("Mes de Aniversario (int) : ");
        scanf("%u", &mes);
    printf("Ano de Nascimento  (int) : ");
        scanf("%u", &ano);

    struct tm birthdate = {.tm_mday = dia, .tm_mon = mes-1 , .tm_year = ano-1900};
    int secs = difftime(time(NULL),mktime(&birthdate));
    Aluno->idade = (int) secs/(86400*366);  // 1 dia * 366 <=> 1 ano
    if(Aluno->idade <= 0) return false;
    else return true;
}

/**< getters *///
int get_student_subjects(STUDENT* Aluno, bool _list){
    if(_list){
        if( sbj_listing(Aluno->disciplina) == false )
            printf("- Nenhuma Disciplina Registrada -");
        puts("\n");
    }
    return _NUM_DISCIPLINAS_ATUAL;
}
void get_student_mediaFinal(STUDENT* Aluno, SUBJECTS* DCP){
    if(DCP == NULL) return;
    get_student_subjects(NULL, DCP->esq);   // envivei NULL para parrametro para nao fazer a media aritmetica
    get_student_subjects(NULL, DCP->dir);   // assim a media artimetica sera feita apos todas as somas
    static double media = 0.000000;
    media +=  DCP->media;

    // _NUM_DCISCILINAS_ATUAL: variavel global criada para guardar o numero atual de disciplinas do aluno
    if(Aluno != NULL){
        Aluno->media_final = media  / (float) _NUM_DISCIPLINAS_ATUAL;
        media = 0.000000;
    }
}

/**< operational *///

void visualizar_disciplinas(SUBJECTS* disciplina){
    if(disciplina == NULL) return;

    visualizar_disciplinas(disciplina->esq);
    printf(" %-12s | %d Avals | %-20s | %4.1f  | %s\n" , disciplina->titulo
                                            , disciplina->numAvals
                                            , getTimeTo(disciplina->lastEvent)
                                            , disciplina->media
                                            , writeStatus(disciplina)
            );
    visualizar_disciplinas(disciplina->dir);
}

void agenda(SUBJECTS* Disciplina){
    if(Disciplina == NULL) return;
    agenda(Disciplina->esq);
    short int i;
    printf("Disciplina { %s }\n",Disciplina->titulo);
    for( i = 0; i < Disciplina->numAvals; i++)
        if(Disciplina->aval[i].feito == false && Disciplina->aval[i].tipo != OEA)
            printf("\t>>\t%-9s: %s\n", getAvalType(Disciplina->aval[i].tipo)
                                     , getTimeLeft(Disciplina->aval[i].data));
    if(Disciplina->exame.active && Disciplina->exame.feito == false)
            printf("\t>>\t%-9s: %s\n", getAvalType(EXAME)
                                     , getTimeLeft(Disciplina->exame.data));
    puts("================================================");

    agenda(Disciplina->dir);
}

void filtrar_disciplina(SUBJECTS* disciplina, float media_busca, bool only_greater_equal){
    if (disciplina == NULL)return;
    filtrar_disciplina(disciplina->esq, media_busca, only_greater_equal);
    if(only_greater_equal){
        if( disciplina->media >= media_busca)
            printf(" %-15s | %-12s | media: %.1f\n", disciplina->titulo,writeStatus(disciplina) ,disciplina->media);
    } else {
        if( disciplina->media < media_busca)
            printf(" %-15s | %-12s | media: %.1f\n", disciplina->titulo,writeStatus(disciplina) ,disciplina->media);
    }
    filtrar_disciplina(disciplina->dir, media_busca, only_greater_equal);
}

char* writeStatus(SUBJECTS* dcp){
    int i;
    char* estado = (char*)malloc(15*sizeof(char));
    int done = 0;
    for(i = 0; i < dcp->numAvals; i++){
            if(dcp->aval[i].feito)
                done += dcp->aval[i].peso;
    }
    if (done == 100)

        sprintf(estado,"%s", dcp->media >= 10.00? "Aprovado":"Reprovado");
    else
        sprintf(estado,"%2i%% Avaliado", done);
    return estado;
}



/**< auxiliares *///


/**< data manager */

bool exportar_subject_info(SUBJECTS *dcp, char *filename){
    errno = 0; bool sucess = false;
    /// vars
    FILE* csv = fopen(filename,"w");
    if(csv){
        fprintf(csv,"Semestre;Disciplina;Docente;Crédito;Avaliação;Estado\n");
        exportar_disciplinas(dcp, &csv);
        fclose(csv);
        sucess = true;
    }
    perror(filename);
    return sucess;
}
bool exportar_student_info(STUDENT *me, char *filename){
    errno = 0; bool success = false;
    /// vars
    FILE* info = fopen(filename,"w");
    if(info){
        // student basic information
        get_student_mediaFinal(me, me->disciplina);
        fprintf(info, "Nome  : %s \nIdade : %u anos\n", me->nome, me->idade);
        fprintf(info, "Curso : %s \nID    : %s\n\n", me->curso, me->ID);
        fprintf(info, "Total Creditos : %d pts \nMedia Final    : %.1lf\n\n", me->letivo, me->media_final);
        // student subjects information
        fputs("---------------------------------------------------------------------------------------\n",info);
        fputs("(SMT) DISCIPLINAS     | PROFESSOR             | TST | PRO | OEA | XAM |  AVALIACAO    |\n",info);
        fputs("---------------------------------------------------------------------------------------\n",info);
        exportar_disciplinas_info(me->disciplina, &info);
        fputs("---------------------------------------------------------------------------------------\n",info);
        fputs("SMT: Semestre;   TST: Testes;  PRO: Projetos; OEA: Outros Elementos; XAM: Exame",info);
        fclose(info);
        success = true;
    }
    perror(filename);
    return success;
}

bool salvar_dados(STUDENT* User){
    SBJ_START;
    //calcular_MediaFinal(Dados,Dados->disciplina);
    FILE* udb = fopen(STUDENT_FILE, "wb");
    if(!udb) return false;
    fwrite(User, sizeof(STUDENT), 1, udb);
    fclose(udb);
    remove(LIST_FILE);
    save_disciplinas(User->disciplina);
    FILE* lst = fopen(LIST_FILE, "a+");
    if(lst){ fprintf(lst,"@ ENDLIST\n\n! A little touch in this file, will mess all your data up."); fclose(lst); }
    return true;
}
bool carregar_dados(STUDENT* User){
    // caregar dados de estudante
    printf("A carregar dados.");
    FILE* data = fopen(STUDENT_FILE, "rb");
    if (data==NULL) return false;
    putchar('.');
    fread(User, sizeof(STUDENT), 1, data);
    fclose(data);
    // carregar as disciplinas
    User->disciplina = NULL;
    load_disciplinas( &(User->disciplina));
    return true;
}
