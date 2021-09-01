#ifndef STUDENT_H
#define STUDENT_H

#define DATA_FORLDER "data"
#define MAX_TEXT 30

#include "subject_tree.h"

char str_media_alta[15];

typedef struct Aluno{
    char nome[MAX_NAME];        // Nome do Aluno
    char ID[11];                // ID Do Aluno em string porque pode ultrapassar o limite de int
    char curso[MAX_NAME];       // Curso do Aluno. ex: EIC
    double MediaFinal;          // Guarda a media final do aluno
    SUBJECTS* disciplina;       // Ponteiro para a estrutura de dados Avore que tem as disciplinas
}STUDENT;

void inicializar_student(STUDENT* Aluno);

void set_student_nome(STUDENT* Aluno, char* _nome);
void set_student_ID(STUDENT* Aluno, char* _ID);
void set_student_curso(STUDENT* Aluno, char* _curso);
void calcular_Media_Final(STUDENT* Aluno, SUBJECTS* DCP);

// operações
bool agendar_teste(SUBJECTS* disciplina, int n_teste, int _dia_mes_ano_hora_min[5], int percentagem);
bool inserir_nota(SUBJECTS* disciplina, int n_teste, float _nota);
bool modificar_nota(SUBJECTS* disciplina, int n_teste, float _nota);
int soma_testes_feito(SUBJECTS* disciplina);    // função soma e rotora o numero totalde testes feito
void dcp_media_alta(SUBJECTS* disciplina, float _media_anterior);     //  função rotorna o titulo da disciplina com media mais alta

void calcular_Media_Final(STUDENT* Aluno, SUBJECTS* DCP);

//
void filtrar_disciplina(SUBJECTS* disciplina, float media_busca);
void ver_agenda(SUBJECTS* disciplina);
void ver_status(SUBJECTS* disciplina);

bool exportar_info_arquivo(STUDENT* Aluno, char* filepath);

bool salvar_dados(STUDENT* Dados);
bool carregar_dados(STUDENT* Dados);

#endif // STUDENT_H
