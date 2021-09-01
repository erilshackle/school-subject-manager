#include "subject_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

unsigned int _NUM_DISCIPLINAS_ATUAL = 0;

SUBJECTS* criar_disciplina(char* _title, char* _teacher, bool load_from_file){
    // a variavel _teacher pode ser o filepath de um arquivo se for para ser carregado.
    int i,k;
    SUBJECTS* novo = (SUBJECTS*)malloc(sizeof(SUBJECTS));

    if(load_from_file == true){
        FILE* data = fopen( _teacher, "rb");
        if(data){
            fread( novo, sizeof(SUBJECTS), 1, data);
            fclose(data);
        }

    }else{

        // dados inicializados
        strcpy(novo->titulo,_title);
        strcpy(novo->teacher,_teacher);
        // dados pardões
        novo->media = 0.00000;
        novo->numTestes = 0;
        for(i = 0; i < MAX_TESTE; i++){
            novo->teste[i].peso = 0;
            for(k = 0; k < 5; k++)    // definir data dos testes, como invalidos
                novo->teste[i].data[k] = 0;
            novo->teste[i].nota = 0.000000;
            novo->teste[i].feito = false;
        }
        sprintf(novo->filepath, "data/dcp_%d.db",_NUM_DISCIPLINAS_ATUAL);
    }
    novo->dir = novo->esq = NULL;
    return novo;
}

// função inserir disciplina: adiciona uma disciplina na arvore de acordo com o titulo
void inserir_disciplina(SUBJECTS* *DCP, char* _title, char* _teacher, bool load_from_file){
    if (*DCP == NULL){
        *DCP = criar_disciplina( _title, _teacher, load_from_file);

        if( *DCP != NULL ) _NUM_DISCIPLINAS_ATUAL +=1;        //  Ao adicionar disciplina o numero de disciplinas aumenta

        return;
    }
    else if( strcmp(_title,(*DCP)->titulo) < 0 )  inserir_disciplina( &(*DCP)->esq ,_title,_teacher, load_from_file);
    else if( strcmp(_title,(*DCP)->titulo) > 0 )  inserir_disciplina( &(*DCP)->dir ,_title,_teacher, load_from_file);
    else{   // se chegar aqui é porque a disciplina ja existe (strcmp == 0)
        printf("Erro, Disciplina Ja existe!\n");
    return;
    }
}

SUBJECTS* buscar_disciplina(SUBJECTS* DCP, char* _title){ // busca uma disicplina e retorna seu endereço
    convert_Upercase(_title);
    if ( DCP == NULL){
        return NULL;
    }
    else if( strcmp(_title,DCP->titulo) < 0 )  return buscar_disciplina(DCP->esq,_title);
    else if( strcmp(_title,DCP->titulo) > 0 )  return buscar_disciplina(DCP->dir,_title);
    else return DCP;
}



bool eliminar_disciplina(SUBJECTS* *DCP, char* _title){
    if(*DCP == NULL)   return false;
    if(strcmp(_title, (*DCP)->titulo) == 0){
		// Caso 1: sem filhos
		if( (*DCP)->esq == NULL && (*DCP)->dir == NULL ){
			*DCP = NULL;
			free(*DCP);
			_NUM_DISCIPLINAS_ATUAL--;

		}//caso 2: com apenas 1 filho
		else if ( (*DCP)->esq == NULL){
			SUBJECTS* aux = *DCP;
			*DCP = (*DCP)->dir;
			free(aux);
			_NUM_DISCIPLINAS_ATUAL--;

		}
		else if ( (*DCP)->dir == NULL){
			SUBJECTS* aux = *DCP;
			*DCP = (*DCP)->esq;
			free(aux);
			_NUM_DISCIPLINAS_ATUAL--;

		}// Caso 3: com ambos os filhos
		else {
			SUBJECTS* aux = (*DCP)->dir;    // menor à direita
			while(aux->esq!=NULL){
				aux = aux->esq;
			}
			// copiar os dados
			copiar_disciplinas(aux, *DCP);
			eliminar_disciplina( &(*DCP)->dir, aux->titulo );
            return true;
		}
		return true;
	}
	else if( strcmp(_title,(*DCP)->titulo) < 0 )  eliminar_disciplina( &(*DCP)->esq ,_title);
    else if( strcmp(_title,(*DCP)->titulo) > 0 )  eliminar_disciplina( &(*DCP)->dir ,_title);
	return true;
}
void listar_disciplinas_pre(SUBJECTS* dcp){ // impimir lista de disciplina em preordem
    if(dcp == NULL) return;

    printf(" %-12s | %-15s |   %02i   | %.1f\n", dcp->titulo, dcp->teacher, dcp->numTestes, dcp->media);
    listar_disciplinas_pre(dcp->esq);
    listar_disciplinas_pre(dcp->dir);
}
void listar_disciplinas_in(SUBJECTS* dcp){  // impimir lista de disciplina em inordem
    if(dcp == NULL) return;

    listar_disciplinas_in(dcp->esq);
    printf(" %-12s | %-15s |   %02i   | %.1f\n", dcp->titulo, dcp->teacher, dcp->numTestes, dcp->media);
    listar_disciplinas_in(dcp->dir);
}
void listar_disciplinas_pos(SUBJECTS* dcp){ // impimir lista de disciplina em posordem
    if(dcp == NULL) return;

    listar_disciplinas_pos(dcp->esq);
    listar_disciplinas_pos(dcp->dir);
    printf(" %-12s | %-15s |   %02i   | %.1f\n", dcp->titulo, dcp->teacher, dcp->numTestes, dcp->media);
}


int Nivelprofundidade(SUBJECTS* DCP, char* _title){
    int depth = 0;
    convert_Upercase(_title);
    SUBJECTS* disciplina = DCP;
    while( disciplina != NULL && strcmp(disciplina->titulo, _title)!=0 ){
        depth++;
        if(disciplina == NULL) return -1;
        else if( strcmp(_title, disciplina->titulo) < 0 )
            disciplina = disciplina->esq;
        else if( strcmp(_title, disciplina->titulo) > 0 )
            disciplina = disciplina->dir;
    }
    return depth;
}


void calcular_Media(SUBJECTS* disciplina){
    int i;
    float media = 0.000000;
    for(i = 0; i < disciplina->numTestes; i++){
        // media = media + nota* (cast float)peso/100
        media += ( disciplina->teste[i].nota * (float)disciplina->teste[i].peso/100 );
    }
    disciplina->media = media;
}

char* showDateOf(int date[]){ //  esta função converte uma vetor de inteiros de tamanho 5 em uma data.
	struct tm data = {
			.tm_mday= date[0],
			.tm_mon=date[1]-1 ,
			.tm_year=date[2]-1900,
			.tm_hour=date[3],
			.tm_min=date[4]
			};
    time_t day = mktime(&data); // funçøes do time.h que constroi uma data atravez da estrutua tm

    return ctime(&day);
}

void copiar_disciplinas(SUBJECTS* to_disciplina, SUBJECTS* from_disciplina){
    int t,d;
    strcpy(to_disciplina->titulo, from_disciplina->titulo);
    strcpy(to_disciplina->teacher, from_disciplina->teacher);
    // dados pardões
    to_disciplina->media = from_disciplina->media;
    to_disciplina->numTestes = from_disciplina->numTestes;
    for(t = 0; t < MAX_TESTE; t++){
        to_disciplina->teste[t].peso = from_disciplina->teste[t].peso;
        for(d = 0; d < 5; d++)    // definir data dos testes, como invalidos
            to_disciplina->teste[t].data[d] = from_disciplina->teste[t].data[d];
        to_disciplina->teste[t].nota = from_disciplina->teste[t].nota;
        to_disciplina->teste[t].feito = from_disciplina->teste[t].feito;
    }
    return;
}

void convert_Upercase(char* text){  // esta função converte a string enviada no parametro para MAIUSTUCLAS
    int i;
	for(i=0;i < strlen(text); i++)
			if( text[i] > 96 && text[i] < 123)
				text[i] = text[i]-32;
}

// Funcções de Gerenciamento


bool exportar_info_disciplina(SUBJECTS* disciplina, char* filepath){
    if(disciplina == NULL) return false;
    exportar_info_disciplina(disciplina->esq, filepath);
    int i;
    FILE* _arquivo = fopen(filepath, "a+");
    if(_arquivo!=NULL){
        fprintf(_arquivo,"%s :::\n", disciplina->titulo);
        fprintf(_arquivo,">\tProfessor: %s\n", disciplina->teacher);
        fprintf(_arquivo,">\tNum de Testes: %d\n", disciplina->numTestes);
        for(i = 0; i < disciplina->numTestes; i++){
                // imprimir testes sua nota e sua data
        fprintf(_arquivo,">\t>>\t%i˚Teste: %.2f [%d/%d/%d] (%d%%)\n", i+1, disciplina->teste[i].nota,
                                    disciplina->teste[i].data[0],disciplina->teste[i].data[1],
                                    disciplina->teste[i].data[2], disciplina->teste[i].peso);
        }
        fprintf(_arquivo,">\tMédia : %.2f\n", disciplina->media);
        fprintf(_arquivo,"- - - - -\n");
        fclose(_arquivo);
    }
    exportar_info_disciplina(disciplina->dir, filepath);
    return true;
}

bool salvar_disciplinas(SUBJECTS* dados){
    if(dados == NULL) return false;
    // salvar
    FILE* arq = fopen( dados->filepath, "w");
    if(arq == NULL) return false;
    FILE* lista = fopen( "data/list.dat", "a");   // salvar numa lista
    if(lista){
        fprintf(lista, "%s\n%s\n",dados->titulo,dados->filepath);
        fclose(lista);
    }
    fwrite(dados,sizeof(SUBJECTS),1,arq);
    fclose(arq);
    // salvar
    salvar_disciplinas(dados->esq);
    salvar_disciplinas(dados->dir);
    return true;
}

bool carregar_disciplinas(SUBJECTS* *dados){    // função não funciona INTERROMPIDA
    //return false;   // esta função não foi terminada sua programação
    char dcpname[16], filepath[16];
    FILE* lista = fopen("data/list.dat", "r");
    puts("Carregando dados...\n");
    if(lista!=NULL){
        //SUBJECTS* aux = dados;
        while( fscanf(lista, "%s %s", dcpname,filepath)){
            errno = 0;
            perror(filepath);

            inserir_disciplina( dados, dcpname, filepath, true);
        }
        fclose(lista);
    }else{
        perror("list.dat");
    }
    return true;
}

// FUNÇÃO INTERROMPIDA
