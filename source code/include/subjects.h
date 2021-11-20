#ifndef SUBJECTS_H
#define SUBJECTS_H
#define DCP_TITLE 15
#define MAX_AVAL 5
#define MAX_NAME 22

#define LIST_FILE               "ssm_data/list.dat"
#define SBJ_FILES               "ssm_data/sbj/"
#define SBJ_START               system("cd ssm_data &&  rmdir /s /q sbj/ 2>nul    &   mkdir sbj/");
#define OLD_SBJ_START           system("cd ssm_data &&  rmdir /s /q old/    &   mkdir old/");
#define OLD_SBJ_FILES           "ssm_data/old/"

/**< includes */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>


unsigned int _NUM_DISCIPLINAS_ATUAL;

typedef enum AVAL_TYPE {TESTE, /*TRABALHO,*/ PROJETO, OEA, EXAME,  ALL, NONE = -1} AVAL_TYPE;

typedef struct Subject_BinTree{
    char titulo[DCP_TITLE];         // nome da disciplina (Identificador)
    char proff[MAX_NAME];                 //nome do professor da disciplina
    unsigned int semestre;    // semestre 0-127
    int lastEvent[5];
    int numAvals;                  // numero de avaliações que a disciplina tem
    float media;                    // a media das notas da disciplina
    struct EXAME{
        bool active;
        bool feito;
        float nota;
        int data[5];
    }exame;                         // Em caso de Exame
    struct AVALIACAO{
        AVAL_TYPE tipo;     // tipo de avaliação Baseado no enum AVAL_TYPE
        int data[5];                // dia,mes,ano,hora,minuto
        float nota;                 //  nota de um teste
        int peso;                   // percentagem para este teste max 100
        bool feito;                 // verdareito caso teste foi feito, falso caso não
    }aval[MAX_AVAL];
    char filepath[25];              // diretorio do arquivo para salvar e carregar  15(data/dcp_000.db)
    struct Subject_BinTree* esq;    // Nó Esquerda
    struct Subject_BinTree* dir;    // Nó Direta
}SUBJECTS;
// binTree
SUBJECTS* criar_disciplina(char* _titulo, char* _proff, unsigned int _semestre, bool load);
bool inserir_disciplina(SUBJECTS* *DCP, char* _titulo, char* _proff, unsigned int _semestre, bool load);
bool eliminar_disciplina(SUBJECTS* *DCP, char* _titulo);
bool eliminar_todas_disciplinas(SUBJECTS* *DCP);
bool arquivar_disciplina(SUBJECTS* *DCP, char *title);
SUBJECTS* buscar_disciplina(SUBJECTS* Disciplina, char* _titulo);
int Nivelprofundidade(SUBJECTS* DCP, char* _title);
void listar_disciplinas_root(SUBJECTS* Disciplina, SUBJECTS* _ROOT);    //  preordem <root first>
void listar_disciplinas_asc(SUBJECTS* Disciplina, SUBJECTS* _ROOT);     //  inordem <esq><dir>
void listar_disciplinas_desc(SUBJECTS* Disciplina, SUBJECTS* _ROOT);    //  inordem <dir><esq>
// operations
bool sbj_listing(SUBJECTS *sbj);
void calc_subject_media(SUBJECTS* Disciplina);
void set_LasttEvent(SUBJECTS* DCP, int data[5]);
void exportar_disciplinas(SUBJECTS* DCP, FILE* *file);
void exportar_disciplinas_info(SUBJECTS* DCP, FILE* *file);
// aux
void copiar_disciplinas(SUBJECTS* to, SUBJECTS* from);
void convert_Upercase(char* str);
void make_captalized(char* str);
// data
bool save_disciplinas(SUBJECTS* DCP);
bool load_disciplinas(SUBJECTS* *DCP);
// memory
void free_disciplinas(SUBJECTS* root);
#endif // SUBJECTS_H
