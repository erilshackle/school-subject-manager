#ifndef SUBJECT_TREE_H
#define SUBJECT_TREE_H

#define MAX_TESTE 5
#define MAX_NAME 30

#include <stdbool.h>

unsigned int _NUM_DISCIPLINAS_ATUAL;

typedef struct Arv_Bin{
    char titulo[15];                // nome da disciplina (Identificador)
    char teacher[MAX_NAME];         //nome do professor da disciplina
    int numTestes;                  // numero de testes que a disciplina tem
    float media;                    // a media das notas da disciplina
    struct teste{
        float nota;                 //  nota de um teste
        int peso;                   // percentagem para este teste max 100
        int data[5];                // dia,mes,ano,hora,minuto
        bool feito;                 // verdareito caso teste foi feito, falso caso não
    }teste[MAX_TESTE];
    char filepath[15];              // diretorio do arquivo para salvar e carregar
    struct Arv_Bin* esq;            // Nó Esquerda
    struct Arv_Bin* dir;            // Nó Direta
}SUBJECTS;

// funções da arvore
SUBJECTS* criar_disciplina(char* _title, char* _teacher, bool load_from_file);               // criar no heap
void inserir_disciplina(SUBJECTS* *DCP, char* _title, char* _teacher, bool load_from_file);  // inserir disciplina
SUBJECTS* buscar_disciplina(SUBJECTS* DCP, char* _title);               // buscar disciplina por nome
bool eliminar_disciplina(SUBJECTS* *DCP, char* _title);                 // eliminar
void listar_disciplinas_pre(SUBJECTS* DCP);                             // imprimir preordem
void listar_disciplinas_in(SUBJECTS* DCP);                              // imprimir inordem
void listar_disciplinas_pos(SUBJECTS* DCP);                             // imprimir posordem
int Nivelprofundidade(SUBJECTS* disciplina, char* _title);              // retornar o nivel de profundidade do nó

void calcular_Media(SUBJECTS* disciplina);                 // função calcula media e armazena no proia disciplina

// função de auxilio
void copiar_disciplinas(SUBJECTS* to_disciplina, SUBJECTS* from_disciplina);    // utilizado em função eliminar
char* showDateOf(int *date);        // converte vetor em data e rotorna string em formato de data
void convert_Upercase(char* text);  // esta função transforma um texto dado em maiuscula

bool exportar_info_disciplina(SUBJECTS* disciplina, char* filepath);

bool salvar_disciplinas(SUBJECTS* dados);
bool carregar_disciplinas(SUBJECTS* *dados);

#endif // SUBJECT_TREE_H
