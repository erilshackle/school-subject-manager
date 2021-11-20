#ifndef STUDENT_H
#define STUDENT_H
#include "subjects.h"
#include "avals.h"

#define STUDENT_FILE       "ssm_data/user.db"


typedef struct Aluno{
    char nome[MAX_NAME];        // Nome do Aluno
    char ID[11];                // ID Do Aluno em string porque pode ultrapassar o limite de int
    char curso[40];             // Curso do Aluno. ex: Eng. Iformatica e de Computadores
    //UINT short letivo[2];       // ano letivo em forma de anoInicio/anoFim ex: 20/21
    unsigned int idade;
    double media_final;         // Guarda a media final do aluno
    int letivo;               // Ano Letivo do aluno
    SUBJECTS* disciplina;       // Ponteiro para a estrutura de dados Avore que tem as disciplinas
}STUDENT;

STUDENT inicializar_student(void);
// setters
void set_student_nome(STUDENT* Aluno);
void set_student_curso(STUDENT* Aluno);
void set_student_ID(STUDENT* Aluno);
bool set_student_idade(STUDENT* Aluno);
// getters
int get_student_subjects(STUDENT* Aluno, bool _list);
void get_student_mediaFinal(STUDENT* Aluno, SUBJECTS* DCP);
// operational
void visualizar_disciplinas(SUBJECTS* disciplina);
void agenda(SUBJECTS* Disciplina);
void filtrar_disciplina(SUBJECTS* disciplina, float media_busca, bool only_greater_equal);
char* writeStatus(SUBJECTS* dcp);
// data manager
bool exportar_subject_info(SUBJECTS *dcp, char *filename);
bool exportar_student_info(STUDENT *me, char *filename);
bool salvar_dados(STUDENT* User);
bool carregar_dados(STUDENT* User);

#endif // STUDENT_H
