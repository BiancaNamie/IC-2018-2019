#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NR_GER 30000 //numero de geracoes
#define NR_JOBS 20 //numero de trabalhos
#define NR_MACH 5 //numero de maquinas
#define NP 500  //numero populacao

typedef struct individuo{
  //  int *job;
    int job[NR_JOBS];
    int makespan;
}ind;


void pop_inicial(ind *pop);
float calculaReward(ind pai1,ind pai2,ind pop_f[2]);
int leArquivo(int tempo[NR_MACH][NR_JOBS],char str[]);
int max(int valor_1, int valor_2);
void makespan(ind *solucao, int tempo[NR_MACH][NR_JOBS]);
void copiaIndividuo(ind *i_destino, ind *i_fonte);
void ordenar(ind *pop);
void deletar(ind *pop,int x);

float crossoverCX(ind *pop,ind *pop_f,int p1,int p2, int tempo[NR_MACH][NR_JOBS]);
float crossover_OX(ind *pop, ind *pop_f, int pai1 , int pai2, int tempo[NR_MACH][NR_JOBS]);
float cruzamentoUniforme(ind *pop, ind *pop_f, int pai1, int pai2,int tempo[NR_MACH][NR_JOBS]);
float cruzamento2p(ind *pop, ind *pop_f, int a1, int a2, int tempo[NR_MACH][NR_JOBS]);
float calculaReward(ind pai1,ind pai2,ind pop_f[2]);
void mutacao(ind *mut_1);

void pop_inicial(ind *pop){
    int i,j,x,aux;
    for(i=0;i<NP;i++){
        for (j=0;j<NR_JOBS;j++){
            pop[i].job[j]=j+1;
        }
    }
    for(i=0;i<NP;i++){
        for (j=0;j<NR_JOBS;j++){
            x= rand()%NR_JOBS;
            aux = pop[i].job[j];
            pop[i].job[j]=pop[i].job[x];
            pop[i].job[x]=aux;
        }
    }
}

int leArquivo(int tempo[NR_MACH][NR_JOBS],char str[]){
    FILE *arquivo = fopen(str, "r"); // cria ou abre o arquivo
    if(arquivo == NULL){ // testa se o arquivo foi aberto com sucesso
        printf("\n\nImpossivel abrir o arquivo!\n\n");
        return 0;
    }
    int job,maq,s,u,b;

  //  while(arquivo != EOF) {
        while(getc(arquivo) != ':' ) {
            /* Adicionar caracter à string. */
        }
        fscanf(arquivo,"%d %d %d %d %d\n", &job, &maq, &s, &u, &b);

        while( getc(arquivo) != ':' ) {
            /* Adicionar caracter à string. */
        }
        printf("\n");

        for(maq=0; maq<NR_MACH; maq++){
            for(job=0; job<NR_JOBS; job++){
                int temp;
                fscanf(arquivo,"%d", &temp);
                tempo[maq][job] = temp;
            }
        }
 //   }
    fclose(arquivo);
    return 0;
}

int max(int valor_1, int valor_2) {
	if(valor_1 > valor_2){
		return valor_1;
	}
	else{
		return valor_2;
	}
}

void makespan(ind *solucao, int tempo[NR_MACH][NR_JOBS]) {
	int termina[NR_MACH][NR_JOBS];
	int makespan = 0;

	// inicializa matriz termina  (j = jobs ; m = maquinas)
	for (int m=0; m < NR_MACH; ++m) {
		for (int j=0; j < NR_JOBS; ++j) {
			termina[m][j] = 0;
		}
	}

	// calcula o tempo de cada maquina
	for (int m=0; m < NR_MACH; ++m) {
		for (int j=0; j < NR_JOBS; ++j) {
			int valor_1 = m > 0 ? termina[m-1][j] : 0;
			int valor_2 = j > 0 ? termina[m][j-1] : 0;

			termina[m][j] = max(valor_1, valor_2) + tempo[m][solucao->job[j] - 1];
		}
	}

	// calcula makespan
	for (int m=0; m < NR_MACH ; ++m) {
		for (int j=0; j < NR_JOBS ; ++j) {
			if(termina[m][j] > makespan) {
				makespan = termina[m][j];
			}
		}
	}
	solucao->makespan = makespan;
}


void copiaIndividuo(ind *i_destino, ind *i_fonte){
    int j;
    for(j = 0 ;j < NR_JOBS; j++){
        i_destino->job[j] = i_fonte->job[j];
    }
    i_destino->makespan = i_fonte->makespan;
}

void ordenar(ind *pop){///Bubble sort
    int x, y;
    ind aux;
    for (x = 0; x < NP; x++)  {
        for (y = x + 1; y < NP; y++){ // sempre 1 elemento à frente
            // se o (x > (x+1)) então o x passa pra frente (ordem crescente)
            if (pop[x].makespan > pop[y].makespan) {
                copiaIndividuo(&aux, &pop[x]);    // aux    = pop[x];
                copiaIndividuo(&pop[x], &pop[y]); // pop[x] = pop[y];
                copiaIndividuo(&pop[y], &aux); // pop[y] = aux;
            }
        }
    }
}
/*
int inserirOrdenado(ind *pop, ind *x){
    int i = 0, j;

    while (pop[i].makespan > x.makespan && i<NP){
        i++;
    }
    for(j = NP; j > i; j--){
        copiaIndividuo(&pop[j], &pop[j-1]);
    }
    copiaIndividuo(&pop[i], &x);
}*/

void deletar(ind *pop,int x){
    int y;
    pop[x].makespan = 0;
    for (y=0;y<NR_JOBS;y++){
        pop[x].job[y] = 0;
    }
}

/*------------------------ CRUZAMENTOS -------------------------*/

float crossoverCX(ind *pop,ind *pop_f,int p1,int p2, int tempo[NR_MACH][NR_JOBS]){
   /*verificar se os dois pais forem iguais, se forem, realizar a mutacao;
     ne nao forem, copiar os que sao iguais, e entao comecar a fazer o ciclic crossover
     o ciclic crossover forma ciclos alternando o que o filho recebe do pai,
     existem dois pais e dois filhos de cada vez;
     quem sao os pais(o pai 1 é pego atraves da sequencia do vetor, o pai 2 é randomico?)
    */

    int i,j,aux;

    for (i=0;i<NR_JOBS;i++){////////passar todos os numeros iguais na mesma posicao para o filho 1 e filho 2
        if (pop[p1].job[i] == pop[p2].job[i]){
            pop_f[0].job[i] = pop[p1].job[i];
            pop_f[1].job[i] = pop[p2].job[i];
        }
    }
    i=0;
    if (i<NR_JOBS){
       do{////////////////////////////////faz os ciclos invertendo pai1 e pai2
            while(pop_f[0].job[i] == 0){
                pop_f[0].job[i] = pop[p1].job[i];
                pop_f[1].job[i] = pop[p2].job[i];
                aux = pop[p2].job[i];
                for(j = 0;j < NR_JOBS;j++){///procura onde no vetor pai é o proximo elemento do ciclo
                    if (pop[p1].job[j]==aux){
                        i = j;
                    }
                }
            }

            while(pop_f[0].job[i] != 0 && i<NR_JOBS){/////proximo elemento ainda nao preenchido
                i++;
            }

            if (pop_f[0].job[i]==0 && i<NR_JOBS){
                while(pop_f[0].job[i] == 0){
                    pop_f[0].job[i] = pop[p2].job[i];
                    pop_f[1].job[i] = pop[p1].job[i];
                    aux = pop[p2].job[i];
                    for(j = 0;j < NR_JOBS;j++){
                        if (pop[p1].job[j] == aux){
                            i = j;
                        }
                    }
                }

                while(pop_f[0].job[i] !=0 && i<NR_JOBS){
                    i++;
                }
            }
        }while(pop_f[0].job[i]==0 && i<NR_JOBS);
    }

    mutacao(&pop_f[0]);
    mutacao(&pop_f[1]);

    makespan(&pop_f[0], tempo);
    makespan(&pop_f[1], tempo);

    if (pop_f[0].makespan < pop[NP-1].makespan){
        copiaIndividuo(&pop[NP-1],&pop_f[0]);
        ordenar(pop);
    }
    if (pop_f[1].makespan < pop[NP-1].makespan){
        copiaIndividuo(&pop[NP-1],&pop_f[1]);
        ordenar(pop);
    }

    return calculaReward(pop[p1], pop[p2], pop_f);
}

float crossover_OX(ind *pop, ind *pop_f, int pai1 , int pai2, int tempo[NR_MACH][NR_JOBS])
{
    int ponto1, ponto2;
    int i, j, k, diferenca, aux, flag;
    ind filho, auxiliar;

    while (1)//define quais vão ser os pontos pra "cortar" o individuo //1=true
        {
            ponto1 = (rand() % NR_JOBS);
            ponto2 = (rand() % NR_JOBS);
            if (ponto1 > ponto2)
            {
                aux = ponto1;
                ponto1 = ponto2;
                ponto2 = aux;
            }

            diferenca = ponto2 - ponto1;
            //mudei para que o cruzamento seja menor que metade dos jobs
            if (ponto1 != ponto2 && diferenca <= 0.5 * NR_JOBS) // eu obrigo a ser pelo menos metade do individuo de crossover
            {
                break;
            }
        }
        int pai1real = pai1; //pra eu colocar (se forem bons), todos os filhos

        for (flag = 0; flag <= 1; flag++)// aqui eu faço com que tenha dois filhos
        {
            if (flag == 1)  //troco eles pra economizar código
            {
                aux = pai2;
                pai2 = pai1;
                pai1 = aux;
            }

            for (i = 0; i < NR_JOBS; i++)
            {
                auxiliar.job[i] = pop[pai2].job[i];
            }
            for (i = 0; i < NR_JOBS; i++)
            {
                for (j = ponto1; j <= ponto2; j++)
                {
                    if (auxiliar.job[i] == pop[pai1].job[j])
                    {
                        auxiliar.job[i] = 0;
                    }
                }
            }
            j = 0; //caminha na auxiliar
            i = 0; //caminha no filho
            while (i < ponto1)
            {
                while (auxiliar.job[j] == 0)
                {
                    j++;
                }
                filho.job[i] = auxiliar.job[j];
                i++;
                j++;
            }
            k = ponto1;
            while (i <= ponto2)
            {
                filho.job[i] = pop[pai1].job[k];
                i++;
                k++;
            }
            while (j < NR_JOBS)
            {
                while (auxiliar.job[j] == 0)
                {
                    j++;
                }
                if(i>=NR_JOBS || j>=NR_JOBS)
                {
                    break;
                }
                filho.job[i] = auxiliar.job[j];
                i++;
                j++;
            }

            if (flag == 0){
                copiaIndividuo(&pop_f[0], &filho);
            }else if (flag == 1){
                copiaIndividuo(&pop_f[1], &filho);
            }

        }

    mutacao(&pop_f[0]);
    mutacao(&pop_f[1]);

    makespan(&pop_f[0], tempo);
    makespan(&pop_f[1], tempo);

    if (pop_f[0].makespan < pop[NP-1].makespan){
        copiaIndividuo(&pop[NP-1],&pop_f[0]);
        ordenar(pop);
    }
    if (pop_f[1].makespan < pop[NP-1].makespan){
        copiaIndividuo(&pop[NP-1],&pop_f[1]);
        ordenar(pop);
    }

    return calculaReward(pop[pai1], pop[pai2], pop_f);
}


///void cruzamentoUniforme(ind *pop, int pai1, int pai2, ind *filho1, ind *filho2)
float cruzamentoUniforme(ind *pop, ind *pop_f, int pai1, int pai2,int tempo[NR_MACH][NR_JOBS]){
    int mask[NR_JOBS], i, j, k = 0, auxFaltaFilho1[NR_JOBS], auxFaltaFilho2[NR_JOBS], aux;
    ind filho1, filho2;

    /*inicia os filhos com -1*/
    for( i =0; i<NR_JOBS;i++){
        filho1.job[i] = -1;
        filho2.job[i] = -1;
    }

    //mascara
    for(i = 0; i < NR_JOBS; i++){
        mask[i] = rand() % 2;
    }
    //FILHO 1
    //coloca os 1 no filho
    for(i = 0; i < NR_JOBS; i++){
        if(mask[i] == 1)
            filho1.job[i] = pop[pai1].job[i];
    }
    //acha quais estao faltando no filho 1
    for(i = 0; i < NR_JOBS; i++){
        aux = 0;
        for(j = 0; j < NR_JOBS; j++){
            if(pop[pai2].job[i] == filho1.job[j]){
                aux = 1;
            }
        }
        if(aux == 0){
            auxFaltaFilho1[k] = pop[pai2].job[i];
            k++;
        }
    }
    //coloca os que estao faltando no filho1 na ordem que aparece no pai2
    k = 0;
    for(i = 0; i < NR_JOBS; i++){
        if(filho1.job[i] == -1){
            filho1.job[i] = auxFaltaFilho1[k];
            k++;
        }
    }
    //FILHO 2
    //coloca os 1 no filho2
    k = 0;
    for(i = 0; i < NR_JOBS; i++){
        if(mask[i] == 1)
            filho2.job[i] = pop[pai2].job[i];
    }

    //acha quais estao faltando no filho2
    for(i = 0; i < NR_JOBS; i++){
        aux = 0;
        for(j = 0; j < NR_JOBS; j++){
            if(pop[pai1].job[i] == filho2.job[j]){
                aux = 1;
            }
        }
        if(aux == 0){
            auxFaltaFilho2[k] = pop[pai1].job[i];
            k++;
        }
    }
    //coloca os que estao faltando no filho2 na ordem que aparece no pai1
    k = 0;
    for(i = 0; i < NR_JOBS; i++){
        if(filho2.job[i] == -1){
            filho2.job[i] = auxFaltaFilho2[k];
            k++;
        }
    }

    copiaIndividuo(&pop_f[0], &filho1);
    copiaIndividuo(&pop_f[1], &filho2);


    mutacao(&pop_f[0]);
    mutacao(&pop_f[1]);


    makespan(&pop_f[0], tempo);
    makespan(&pop_f[1], tempo);

    if (pop_f[0].makespan < pop[NP-1].makespan){
        copiaIndividuo(&pop[NP-1],&pop_f[0]);
        ordenar(pop);
    }
    if (pop_f[1].makespan < pop[NP-1].makespan){
        copiaIndividuo(&pop[NP-1],&pop_f[1]);
        ordenar(pop);
    }

    return calculaReward(pop[pai1], pop[pai2], pop_f);

}

float cruzamento2p(ind *pop, ind *pop_f, int a1, int a2, int tempo[NR_MACH][NR_JOBS]){
    int c1, c2, j;
	ind aux[4];
    ind filho1, filho2;

        do{                             /// pega duas posições aleatorias para ser os pontos de corte
            c1 = rand()%NR_JOBS;
            c2 = rand()%NR_JOBS;
        }while (c2 < c1);

    for(j = 0; j < NR_JOBS; j++){     ///Zera os filhos
        filho1.job[j] = 0;
        filho2.job[j] = 0;
    }

    for(j = c1;j < c2; j++){      ///copia os meios dos pais para os filhos
        filho1.job[j] = pop[a2].job[j];
        filho2.job[j] = pop[a1].job[j];
    }

    int repete = 0, indiceAuxR = 0;
    int auxR[(NR_JOBS) - (c2 - c1)];


    for(j=0;j<NR_JOBS;j++){ /// Guarda em um vetor aux os valores nao repetidos. PARTE 1
        repete = 0;
        for(int jm=c1;jm<c2;jm++){
            if(pop[a1].job[j]==filho1.job[jm]){
                repete = 1;
            }
        }
        if(repete == 0){
            auxR[indiceAuxR] = pop[a1].job[j];
            indiceAuxR++;
        }
    }
    indiceAuxR = 0;
    for(j=0;j<c1;j++){
        filho1.job[j] = auxR[indiceAuxR];
        indiceAuxR++;
    }
    for(j=c2;j<NR_JOBS;j++){
        filho1.job[j] = auxR[indiceAuxR];
        indiceAuxR++;
    }

    indiceAuxR = 0;
    repete = 0;

    for(j=0;j<NR_JOBS;j++){ /// Guarda em um vetor aux os valores nao repetidos. PARTE 2
        repete = 0;
        for(int jm=c1;jm<c2;jm++){
            if(pop[a2].job[j]==filho2.job[jm]){
                repete = 1;
            }
        }
        if(repete == 0){
            auxR[indiceAuxR] = pop[a2].job[j];
            indiceAuxR++;
        }
    }

    indiceAuxR = 0;
    for(j=0;j<c1;j++){
        filho2.job[j] = auxR[indiceAuxR];
        indiceAuxR++;
    }
    for(j=c2;j<NR_JOBS;j++){
        filho2.job[j] = auxR[indiceAuxR];
        indiceAuxR++;
    }

    copiaIndividuo(&pop_f[0], &filho1);
    copiaIndividuo(&pop_f[1], &filho2);


    mutacao(&pop_f[0]);
    mutacao(&pop_f[1]);


    makespan(&pop_f[0], tempo);
    makespan(&pop_f[1], tempo);

    if (pop_f[0].makespan < pop[NP-1].makespan){
        copiaIndividuo(&pop[NP-1],&pop_f[0]);
        ordenar(pop);
    }
    if (pop_f[1].makespan < pop[NP-1].makespan){
        copiaIndividuo(&pop[NP-1],&pop_f[1]);
        ordenar(pop);
    }

    return calculaReward(pop[a1], pop[a2], pop_f);
}

float calculaReward(ind pai1,ind pai2,ind pop_f[2]){
    float maior, menor;
    if (pai1.makespan >pai2.makespan){
        maior = pai2.makespan;
    }else{
        maior = pai1.makespan;
    }
    if (pop_f[0].makespan > pop_f[1].makespan){
        menor = pop_f[0].makespan;
    }else{
        menor = pop_f[1].makespan;
    }
    return maior - menor;
}

void mutacao(ind *mut_1){
    int i,j,aux;

    i = rand()%NR_JOBS;
    do{
        j = rand()%NR_JOBS;
    }while (i==j);
    aux = mut_1->job[i];
    mut_1->job[i] = mut_1->job[j];
    mut_1->job[j] = aux;
}

/*----------------------CHOICE FUNCTION-------------------*/
typedef struct ChoiceF{
    float CF;
    float f1;
    float f3;
    clock_t tempo1; // em segundos
}ChoiceF;

void choiceFunction(int s,float reward, ChoiceF *C, float *lastReward, int *lastOperator, float f2[4][4], float *a, float *b, float *c ){
    C[s].f1 = C[s].f1 + reward;
    f2[*lastOperator][s] = f2[*lastOperator][s]  + reward + *lastReward ;
    C[s].f3 = 0;

    for (int i = 0; i < 4; i++){
        if (i != s){
            C[i].f3 += 1;
        }
    }

    if (reward >= 0){
        *a = 0.49;
        *b = 0.49;
        *c = 0.02;
    }else if (*a >= 0.02){
            *a = *a - 0.02;
            *b = *b - 0.02;
            *c = 1 - (*a + *b);
    }

    *lastReward = reward;
    *lastOperator = s;

    for (int i = 0; i < 4; i++){
        C[i].CF = (*a * C[i].f1) + (*b * f2[s][i]) + (*c * C[i].f3);
    }
}


int maiorCF(ChoiceF *C){
    int maior = 0;
    for (int i = 1; i < 4; i++){
        if (C[i].CF > C[maior].CF){
            maior = i;
        }
    }
    return maior;
}

void IniciaChoiceFunction(ChoiceF *C){
    for(int i=0; i<4; i++){
        C[i].CF = 10;
        C[i].f1 = 0;
        C[i].f3 = 0;
        C[i].tempo1 = clock();
    }
}

/*------------------------ MAIN -------------------------*/

/// cx = 0, ox = 1, doisP = 2, unif = 3;

int main(int argc, char *argv[]){
///argv[0] - nome do programa, argv[1] = arquivo que vai ler, argv[2] = arquivo de saida

    clock_t Ticks[2];           ///isso pode calcular o f3
    Ticks[0] = clock();
    float reward = 0,lastReward = 0;
    ind pop[NP], pop_f[2];
    int tempo[NR_MACH][NR_JOBS];
    int i,j;
    int s = -1, lastOperator =0;
    int pai1 = 0, pai2 = 0;
    float f2[4][4], a=0.49,b=0.49,c=0.2;
    ChoiceF C[4];

    FILE * tempo1 = fopen(argv[2], "a+");
    FILE * melhor = fopen(argv[3], "a+");
    FILE * permutacao = fopen(argv[4], "a+");
    FILE * sequenciaS = fopen(argv[5], "a+");

    IniciaChoiceFunction(C);

    leArquivo(tempo, argv[1]);
    srand(time(NULL));
    pop_inicial(pop);
    for(i=0; i<NP; i++){                ///faz makespan da pop inicial
        makespan(&pop[i], tempo);
    }
    ordenar(pop);                       ///  talvez fazer uma lista ja ordenada

    for (j = 0; j < NR_GER; j++){

        for(i = 0; i < 2; i++){              ///zera a populacao dos filhos
            deletar(pop_f,i);
        }

        do{
            pai1 = rand()%(NP/2);
            pai2 = rand()%NP;
        }while (pai1 == pai2);

        if (j>=4){
            s = maiorCF(C);
        }
        else{
            s++;
        }

        switch(s){          /// cx = 0, ox = 1, doisP = 2, unif = 3;
        case 0:
            reward = crossoverCX(pop,pop_f,pai1,pai2,tempo);
            break;
        case 1:
            reward = crossover_OX(pop, pop_f,pai1,pai2,tempo);
            break;
        case 2:
            reward = cruzamento2p(pop, pop_f, pai1,pai2, tempo);
            break;
        case 3:
            reward = cruzamentoUniforme(pop, pop_f, pai1,pai2, tempo);
            break;
        }
        fprintf(sequenciaS,"%d ", s);

        printf("\n op-  %d ", s);
        printf("\n LastOp-  %d ", lastOperator);
        printf("\n reward- %f ", reward);
        printf("\n LastReward- %f ", lastReward);

        choiceFunction(s ,reward, C, &lastReward, &lastOperator, f2,&a, &b, &c);

   }

    printf ("Melhor makespan: %d", pop[0].makespan );
   /* printf("\nMelhor individuo: " );
    for (i=0;i<NR_JOBS;i++){
         printf("%d ",pop[0].job[i]);
    }*/
    Ticks[1] = clock();
    double Tempo = (double)(Ticks[1] - Ticks[0]) *(CLOCKS_PER_SEC/ 1000);
    printf("Tempo gasto: %g ms.", Tempo);

    fprintf(tempo1, "%g\n", Tempo);
    fprintf(melhor, "%d\n", pop[0].makespan);

    for (i=0;i<NR_JOBS;i++){
        fprintf(permutacao,"%d",pop[0].job[i]);
        if(i!=NR_JOBS-1){
            fprintf(permutacao,"-");
        }
    }
    fprintf(sequenciaS,"\n",s);

    fclose(tempo1);
    fclose(melhor);
    fclose(permutacao);
    fclose(sequenciaS);

    return 0;
}