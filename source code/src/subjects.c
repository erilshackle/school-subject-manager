#include "subjects.h"

// Includes
#include "student.h"
#include "avals.h"


/**< Funções Arvore Binaria */
SUBJECTS* criar_disciplina(char* _titulo, char* _proff ,unsigned int _semestre, bool load){
    // a variavel _proff pode ser o filepath de um arquivo se for para ser carregado.
    int i,k;
    SUBJECTS* novo = (SUBJECTS*)malloc(sizeof(SUBJECTS));

    if(load == true){
        errno = 0;
        FILE* dados = fopen(_proff, "rb");
        if(dados){
            fread( novo, sizeof(SUBJECTS), 1, dados);
            fclose(dados);
        } else{
            return NULL;
        }
        puts(strerror(errno));
    }else{

        // dados inicializados
        strcpy(novo->titulo,_titulo);
        novo->semestre = _semestre;
        strcpy(novo->proff,_proff);
        /* dados pardões */
        novo->media = 0.00000;
        novo->numAvals = 0;
        for(i = 0; i < 5; i++) novo->lastEvent[i] = NONE;
        // Avaliacnoes
        for(i = 0; i < MAX_AVAL; i++){
            novo->aval[i].tipo = NONE;
            novo->aval[i].peso = 0;
            for(k = 0; k < 5; k++)  novo->aval[i].data[k] = 0;  // definir data dos testes, como invalidos
            novo->aval[i].nota = 0.000000;
            novo->aval[i].feito = false;
        }
        // exame
        novo->exame.active = false;
        novo->exame.feito = false;
        novo->exame.nota = 0.000000;
        for(i = 0; i < 5; i++) novo->exame.data[i] = 0;
    }
    // filepath
    sprintf(novo->filepath, "%s/dcp_%03d.db", SBJ_FILES,_NUM_DISCIPLINAS_ATUAL);
    novo->dir = novo->esq = NULL;
    return novo;
}

bool inserir_disciplina(SUBJECTS* *DCP, char* _titulo, char* _proff, unsigned int _semestre, bool load){
    if (*DCP == NULL){
        *DCP = criar_disciplina( _titulo, _proff, _semestre, load);
        if( *DCP != NULL ) _NUM_DISCIPLINAS_ATUAL++;        //  Ao adicionar disciplina o numero de disciplinas aumenta
        return true;
    }
    else if( strcmp(_titulo,(*DCP)->titulo) < 0 ) return inserir_disciplina( &(*DCP)->esq ,_titulo,_proff,_semestre, load);
    else if( strcmp(_titulo,(*DCP)->titulo) > 0 ) return inserir_disciplina( &(*DCP)->dir ,_titulo,_proff,_semestre, load);
    else {// se chegar aqui é porque a disciplina ja existe (strcmp == 0)
        puts("ERRO: Disciplina Ja existe!");
    }
    return false;
}

bool eliminar_disciplina(SUBJECTS* *DCP, char _titulo[]){
    if(*DCP==NULL){
        puts("ERRO: Disciplina não encontrado!");
        return false;
    }
    else if (strcmp(_titulo, (*DCP)->titulo)==0){
        // caso1: sem filhos
        if( (*DCP)->esq == NULL && (*DCP)->dir == NULL ){
			remove((*DCP)->filepath);
			free(*DCP);
			*DCP = NULL;
			_NUM_DISCIPLINAS_ATUAL--;
        }
        //caso 2: com 1 filho à direita
		else if( (*DCP)->esq == NULL){
			SUBJECTS* aux = *DCP;
			*DCP = (*DCP)->dir;
			remove(aux->filepath);
			free(aux); aux = NULL;
			_NUM_DISCIPLINAS_ATUAL--;
		}
		//caso 2: com 1 filho à esquerda
		else if ( (*DCP)->dir == NULL){
			SUBJECTS* aux = *DCP;
			*DCP = (*DCP)->esq;
			remove(aux->filepath);
			free(aux); aux = NULL;
			_NUM_DISCIPLINAS_ATUAL--;
        }
        // caso 3: com 2 filhos
        else{
            SUBJECTS* aux = (*DCP)->dir;    // menor à direita
			while(aux->esq!=NULL){
				aux = aux->esq;
			}
			// copiar os dados
			copiar_disciplinas(aux, *DCP);
			return eliminar_disciplina( &(*DCP)->dir, aux->titulo );
        }
        return true;
    }
    else if( strcmp(_titulo,(*DCP)->titulo) < 0 )  return eliminar_disciplina( &(*DCP)->esq ,_titulo);
    else if( strcmp(_titulo,(*DCP)->titulo) > 0 )  return eliminar_disciplina( &(*DCP)->dir ,_titulo);
    else return false;
    return true;
}

bool eliminar_todas_disciplinas(SUBJECTS* *DCP){
    if(*DCP == NULL)return false;
    eliminar_todas_disciplinas( &(*DCP)->esq);
    eliminar_todas_disciplinas( &(*DCP)->dir);
    free(*DCP);
    *DCP = NULL;
    _NUM_DISCIPLINAS_ATUAL--;
    return true;
}

bool arquivar_disciplina(SUBJECTS* *DCP, char* title){
//    char filename[DCP_TITLE+5];
//    sprintf(filename,"archive_%s.sbj", (*DCP)->titulo);
//    FILE* arq = fopen( OLD_SBJ_FILE(filename), "wb" );
////    FILE* olds = fopen( OLD_SBJ_FILE("archived.txt"), "a");
//    if(arq){
//        fwrite( (*DCP), sizeof(SUBJECTS), 1, arq );
//        fprintf(arq, "%s - %s - %i\n", (*DCP)->titulo, (*DCP)->proff, (*DCP)->semestre);
//        eliminar_disciplina(&(*DCP),(*DCP)->titulo);
//        return true;
//        fclose(arq);
//    }
    return false;
}

SUBJECTS* buscar_disciplina(SUBJECTS* Disciplina,char* _titulo){
    if(Disciplina == NULL){
        puts("< Não Encontrado >");
        return NULL;
    }else if(strcmp(_titulo,Disciplina->titulo) < 0) return buscar_disciplina(Disciplina->esq, _titulo);
     else if(strcmp(_titulo,Disciplina->titulo) > 0) return buscar_disciplina(Disciplina->dir, _titulo);
    else return Disciplina;
}

int Nivelprofundidade(SUBJECTS* DCP, char* _title){
    int depth = 0;
    convert_Upercase(_title);
    SUBJECTS* disciplina = DCP;
    while( disciplina != NULL && strcmp(disciplina->titulo, _title)!=0 ){
        depth++;
        if( strcmp(_title, disciplina->titulo) < 0 )
            disciplina = disciplina->esq;
        else if( strcmp(_title, disciplina->titulo) > 0 )
            disciplina = disciplina->dir;
    }
    if(disciplina == NULL) return -1;
    return depth;
}

void listar_disciplinas_root(SUBJECTS* Disciplina, SUBJECTS* _ROOT){
    if(Disciplina == NULL) return;
    printf(" %-12s | %2d | %-2i | %-22s | %3d%% agendado\n",
           Disciplina->titulo, Nivelprofundidade(_ROOT, Disciplina->titulo),
           Disciplina->semestre, Disciplina->proff, getAvalStatus(Disciplina)
    );
    listar_disciplinas_root(Disciplina->esq, _ROOT);
    listar_disciplinas_root(Disciplina->dir, _ROOT);
}

void listar_disciplinas_asc(SUBJECTS* Disciplina, SUBJECTS* _ROOT){
    if(Disciplina == NULL) return;
    listar_disciplinas_asc(Disciplina->esq, _ROOT);
    printf(" %-12s | %2d | %-2i | %-22s | %3d%% agendado\n",
           Disciplina->titulo, Nivelprofundidade(_ROOT, Disciplina->titulo),
           Disciplina->semestre, Disciplina->proff, getAvalStatus(Disciplina)
    );
    listar_disciplinas_asc(Disciplina->dir, _ROOT);
}

void listar_disciplinas_desc(SUBJECTS* Disciplina, SUBJECTS* _ROOT){
    if(Disciplina == NULL) return;
    listar_disciplinas_desc(Disciplina->dir, _ROOT);
    printf(" %-12s | %2d | %-2i | %-22s | %3d%% agendado\n",
           Disciplina->titulo, Nivelprofundidade(_ROOT, Disciplina->titulo),
           Disciplina->semestre, Disciplina->proff, getAvalStatus(Disciplina)
    );
    listar_disciplinas_desc(Disciplina->esq, _ROOT);
}

/**< Operational */
bool sbj_listing(SUBJECTS *sbj){
    if(sbj == NULL){
        return false;
    }
    sbj_listing(sbj->esq);
    printf("- %s -", sbj->titulo);
    sbj_listing(sbj->dir);
    return true;
}

void calc_subject_media(SUBJECTS* Disciplina){  // invocada pela função uniao de (set_mark_to && un_mark_to) _avaliamento_;
    int i;
    float Media = 0.00000;
    if(Disciplina->exame.active && Disciplina->exame.feito ){
        Media = Disciplina->exame.nota * 1;
    } else {
        for(i = 0; i < Disciplina->numAvals; i++){
            if(Disciplina->aval[i].feito == true){
                Media += ( Disciplina->aval[i].nota * (Disciplina->aval[i].peso / 100.000) );
            }
        }
    }
    Disciplina->media = Media;
    return;
}


void set_LasttEvent(SUBJECTS* DCP, int data[5]){

}

void exportar_disciplinas(SUBJECTS* DCP, FILE* *file){
    if(DCP==NULL)return;
    exportar_disciplinas(DCP->esq, &(*file));
    fprintf(*file,"%huS;%s;%s;%.0f;%s\n",DCP->semestre,DCP->titulo,DCP->proff,DCP->media, writeStatus(DCP));
    exportar_disciplinas(DCP->dir, &(*file));
}
void exportar_disciplinas_info(SUBJECTS* DCP, FILE* *file){
    if(DCP==NULL)return;
    exportar_disciplinas_info(DCP->esq, &(*file));
    /// vars
    int T = count_aval_type(DCP, TESTE), X = DCP->exame.active,
        P = count_aval_type(DCP, PROJETO), O = count_aval_type(DCP, OEA);

    fprintf(*file,"(%huS)  %-15s | %-21s | %-3d |%-5s|%-5s|%-5s| %3d%% Agendado |\n",
            DCP->semestre, DCP->titulo, DCP->proff, T,
            P > 0? " SIM " : " NAO ",
            O > 0? " SIM " : " NAO ",
            X == true? returnMiniData(DCP->exame.data) : " NAO ",
            getAvalStatus(DCP));
    exportar_disciplinas_info(DCP->dir, &(*file));
}

/**< Auxiliares */

void copiar_disciplinas(SUBJECTS* to, SUBJECTS* from){  // Utilizado na função eliminar como auxiliar
    int a,d;
    strcpy(to->titulo,from->titulo);
    strcpy(to->proff, from->proff);
    to->media = from->media;
    to->numAvals = from->numAvals;
    for(a = 0; a < MAX_AVAL; a++){
        to->aval[a].tipo = from->aval[a].tipo;
        to->aval[a].peso = from->aval[a].peso;
        to->aval[a].nota = from->aval[a].nota;
        to->aval[a].feito = from->aval[a].feito;
        for(d = 0; d < 5; d++)
            to->aval[a].data[d] = from->aval[a].data[d];
    }
    to->exame.active = from->exame.active;
    to->exame.feito = from->exame.feito;
    to->exame.nota = from->exame.nota;
    for(d = 0; d < 5; d++)
            to->exame.data[d] = from->exame.data[d];
    /* end */
    return;
}

void convert_Upercase(char* str){      // converter string para MAIUSTUCLAS
    int i; fflush(stdin);
	for(i=0;i < strlen(str); i++)
			if( str[i] > 96 && str[i] < 123)
				str[i] = str[i]-32;
}

void make_captalized(char* str){
    int i;fflush(stdin);
    if( str[0] > 96 && str[0] < 123) str[0] = str[0]-32;

    for(i=1; i < strlen(str); i++)
        if( str[i] == ' '){
            i++;
            if( str[i] > 96 && str[i] < 123)    str[i] = str[i]-32;
        }
    fflush(stdin);
}

/**< Save & Load Data */

bool save_disciplinas(SUBJECTS* DCP){
    if(DCP == NULL) return false;
    FILE* db = fopen(DCP->filepath, "wb");
    if(db == NULL) return false;
    // salvar
    FILE* lista;   // salvar na lista
    if( (lista = fopen(LIST_FILE, "a+")) )
    {
        fprintf(lista, "%s %s\n", DCP->titulo,DCP->filepath);
        fclose(lista);
        fwrite(DCP,sizeof(SUBJECTS), 1,db);

        fclose(db);
    }
    // salvar
    save_disciplinas(DCP->esq);
    save_disciplinas(DCP->dir);
    return true;
}

bool load_disciplinas(SUBJECTS* *DCP){
    errno = 0;
    char dcpname[DCP_TITLE], filepath[16];
    FILE* lista = fopen(LIST_FILE, "r");
    puts(".");
    if(lista!=NULL){
        while( fscanf(lista, "%s%s", dcpname,filepath)!=EOF){
            if( strcmp(dcpname,"@")==0 && strcmp(filepath,"ENDLIST")==0 ) break;
            printf("%s: ", dcpname);
            inserir_disciplina(&(*DCP), dcpname, filepath, 1, true);
        }
        fclose(lista);
    }else{
        perror("list.dat");
        return false;
    }
    return true;
}

/**< free memory allocated */

void free_disciplinas(SUBJECTS* root){
    if(root == NULL) return;
    free_disciplinas(root->esq);
    free_disciplinas(root->dir);
    free(root);
    return;
}
