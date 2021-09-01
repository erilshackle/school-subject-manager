#include "student.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void inicializar_student(STUDENT* Aluno){   // inicializa dados do estudadnte
    system("cls");
    // Aviso de entrada
    puts("Esta deve ser a sua primeira vez a usar este programa.");
    puts("Por favor preencha algumas informações.\n---");

    puts("\nEntre com seu nome:");    fflush(stdin);
        gets(Aluno->nome);
    puts("Nome do seu curso:");     fflush(stdin);
        gets(Aluno->curso);
    puts("Numero de ID:");
        scanf("%s", Aluno->ID);
    Aluno->disciplina = NULL;
}

void set_student_nome(STUDENT* Aluno, char* _nome){
    strcpy(Aluno->nome,_nome);
}

void set_student_ID(STUDENT* Aluno, char* _ID){
    strcpy(Aluno->ID,_ID);
}

void set_student_curso(STUDENT* Aluno, char* _curso){
    strcpy(Aluno->curso,_curso);
}

void calcular_Media_Final(STUDENT* Aluno, SUBJECTS* DCP){
    if(DCP == NULL) return;
    calcular_Media_Final(NULL, DCP->esq);   // envivei NULL para parrametro para não fazer a media aritmetica
    calcular_Media_Final(NULL, DCP->dir);   // assim a media artimetica será feita apos todas as somas
    static float media = 0.000000;
    media +=  DCP->media;

    // _NUM_DCISCILINAS_ATUAL: variavel global criada para guardar o numero atual de disciplinas do aluno
    if(Aluno != NULL){
        Aluno->MediaFinal = media  / (float) _NUM_DISCIPLINAS_ATUAL;
        media = 0.000000;
    }
}

// funções de operações

void ver_agenda(SUBJECTS* disciplina){
    if(disciplina == NULL) return;

    ver_agenda(disciplina->esq);
    printf("Disciplina: %s\n", disciplina->titulo);
    int i;
    for(i = 0; i < disciplina->numTestes; i++){
            if (disciplina->teste[i].feito == false){
                printf(" > (%i*Teste) >> %s\n", i+1, showDateOf(disciplina->teste[i].data));
            }
    }
    puts("==================================");
    ver_agenda(disciplina->dir);
}

void ver_status(SUBJECTS* disciplina){
    if(disciplina == NULL)return;
    int i;
    ver_status(disciplina->esq);
    printf("%-15s { ", disciplina->titulo);
    for(i = 0; i< disciplina->numTestes; i++){
        if(disciplina->teste[i].feito == true)
            printf(" %.2f*%.1f +", disciplina->teste[i].nota, (float) disciplina->teste[i].peso/100);
    }
    printf("\b } media = %.2f\n", disciplina->media);
    ver_status(disciplina->dir);
}

void filtrar_disciplina(SUBJECTS* disciplina, float media_busca){
    if (disciplina == NULL)return;
    filtrar_disciplina(disciplina->esq, media_busca);

    if( disciplina->media >= media_busca)
        printf(" %-15s | media => %.1f\n", disciplina->titulo, disciplina->media);

    filtrar_disciplina(disciplina->dir, media_busca);
}


bool agendar_teste(SUBJECTS* disciplina, int n_teste, int _dia_mes_ano_hora_min[5], int percentagem){
    int i, status = 0;
    for(i=0; i < disciplina->numTestes; i++){
        status += disciplina->teste[i].peso;
    }
    if( status >= 100){
        printf("Não pode agendar mais teste para esta disciplina");
    }else if( percentagem <= 0 || percentagem > 100){
        printf("Valor do peso do teste inválido: %d  (1~100)", percentagem);
    } else {
        // marcar data
        for(i = 0; i < 5; i++) disciplina->teste[n_teste].data[i] = _dia_mes_ano_hora_min[i];
        // marcar peso
        disciplina->teste[n_teste].peso = percentagem;
        // validar o teste marcado
        disciplina->numTestes++;
        return true;
    }
    return false;
}

int soma_testes_feito(SUBJECTS* disciplina){
    int i, feitos = 0;
    if (disciplina == NULL) return 0;
    for(i=0;i<disciplina->numTestes; i++)
        if (disciplina->teste[i].feito==true)
            feitos++;
    feitos += soma_testes_feito(disciplina->esq);
    feitos += soma_testes_feito(disciplina->dir);
    return feitos;
}

bool inserir_nota(SUBJECTS* disciplina, int n_teste, float _nota){
    if(n_teste < 0 || n_teste > disciplina->numTestes){
        return false;
    }else if( _nota < 0.00 || _nota > 20.00){
        return false;
    }else{
        disciplina->teste[n_teste].nota = _nota;
        disciplina->teste[n_teste].feito = true;
        calcular_Media(disciplina);

        return true;
    }
    return false;
}

bool modificar_nota(SUBJECTS* disciplina, int n_teste, float _nota){
    if(n_teste < 0 || n_teste > disciplina->numTestes){
        return false;
    }else if( _nota < 0.00 || _nota > 20.00){
        return false;
    }else{
        disciplina->teste[n_teste].nota = _nota;
        disciplina->teste[n_teste].feito = true;
        calcular_Media(disciplina);

        return true;
    }
    return false;
}

void dcp_media_alta(SUBJECTS* disciplina, float _media_aux){
    if(disciplina == NULL)return false;
    if (disciplina->media > _media_aux){
        strcpy(str_media_alta, disciplina->titulo);
    }
    dcp_media_alta(disciplina->esq, _media_aux);
    dcp_media_alta(disciplina->dir, _media_aux);
    return true;
}

// Guardar Dados do estudante

bool exportar_info_arquivo(STUDENT* Aluno, char* filepath){
    calcular_Media_Final(Aluno,Aluno->disciplina);
    FILE* arquivo = fopen(filepath, "w");
    if (arquivo == NULL){
        return false;
    }else{
        fprintf(arquivo,"NOME: %s\n", Aluno->nome);
        fprintf(arquivo,"ID: %s\n", Aluno->ID);
        fprintf(arquivo,"CURSO: %s\n", Aluno->curso);
        fprintf(arquivo,"TOTAL DE DISCIPLINAS: %d\n", _NUM_DISCIPLINAS_ATUAL);
        fprintf(arquivo,"CALCULO DE MEDIA FINAL: %.2lf\n", Aluno->MediaFinal);
        fprintf(arquivo,"--------------- - DISCIPLINAS - ---------------\n");
        fclose(arquivo);
    }
    // agora exportar informações sobre disciplinas
    exportar_info_disciplina(Aluno->disciplina, filepath);
    // abrir novamente para adiconar informações extras

    FILE* arquivoX = fopen(filepath, "a");
    if (arquivoX == NULL){
        return false;
    }else{
        fprintf(arquivoX,"--------------- ----- fim ----- ---------------\n");
        time_t tempo = time(NULL);
        fprintf(arquivoX,"%s\n\n", ctime(&tempo));   // imprimir a data
        fclose(arquivoX);
    }
    return true;
}

bool salvar_dados(STUDENT* Dados){
    system("if not exist data (mkdir data)");  //   cria a pasta data caso nao existir
    calcular_Media_Final(Dados,Dados->disciplina);
    FILE* user = fopen("data/user.db", "wb");
    if( user == false) return false;
    fwrite(Dados, sizeof(STUDENT), 1, user);
    fclose(user);
    remove("data/list.txt");
    return salvar_disciplinas(Dados->disciplina);
}
bool carregar_dados(STUDENT* Dados){
    // caregar dados de estudante
    FILE* data = fopen("data/user.db", "rb");
    if (data==NULL) return false;
    fread(Dados, sizeof(STUDENT), 1, data);
    fclose(data);
    // carregar as disciplinas e retornar true or false
    Dados->disciplina = NULL;
    carregar_disciplinas( &(Dados->disciplina)); //
    return true;
}


