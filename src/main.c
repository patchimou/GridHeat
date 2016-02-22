#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/**
@authors : Jeremy Melchor
Matthieu Perrin
**/
#define TAILLE_MIN 256
#define TAILLE_MAX 67108864
#define NB_MIN_THREAD 1
#define NB_MAX_THREAD 1024
#define TEMP_CHAUD 150
#define TEMP_FROID 0

int taillePlaque;
bool MFLAG=false;
bool AFLAG=false;
bool MMFLAG=false;

struct Param{
	int *p_array;
	int size;
};


/* Affiche la grille sur le terminal */
void print_plaque(float (*plaque)[taillePlaque]) {
	for (int i=0; i<taillePlaque; i++) {
		printf("\n");
		for (int j=0; j<taillePlaque; j++) {
			printf("%.1f     ",plaque[i][j]);
		}
	}
	printf("\n");
}


/* Initialise la plaque avec TEMP_CHAUD au milieu et le reste a TEMP_FROID */
void init_plaque(float (*plaque)[taillePlaque], int i_min, int i_max) {
	for (int i=0; i<taillePlaque; i++) {
		for (int j=0; j<taillePlaque; j++) {
			if ( (j>=i_min && j<i_max) && (i>=i_min && i<i_max) )
				plaque[i][j] = TEMP_CHAUD;
			else
				plaque[i][j] = TEMP_FROID;
		}
	}
}


/* Convertit la puissance passee en parametre en taille */
int convertPowToSize(int puissance_taillePlaque) {
	return pow(2,(puissance_taillePlaque+4));
}


/* Fais les calculs verticalement sur la grille a partir
de l'emplacement i et j passe en parametre ainsi que la valeur 
de la case du milieu. Return la valeur de la case du milieu */
float vertical(float (*plaque_apres)[taillePlaque], float current_value, int i, int j) {
	if ( (i-1) < 0 ) 
		plaque_apres[i+1][j] += current_value/6;
	else if ( (i+1) >= taillePlaque) 
		plaque_apres[i-1][j] += current_value/6; 
	else {
		plaque_apres[i-1][j] += current_value/6;
		plaque_apres[i+1][j] += current_value/6;
	}
	return current_value -= current_value/3;
}


/* methode principale de calcul de la grille, pour chaque case ayant une valeur
non nulle de temperature, on calcule ses valeurs verticale avec la fonction
vertical, puis on inscrit la valeur de la case centrale */
void calculate_grid(float (*plaque_avant)[taillePlaque], float (*plaque_apres)[taillePlaque]) {
	for (int i=0; i<taillePlaque; i++) {
		for (int j=0; j<taillePlaque; j++) {
			if (plaque_avant[i][j] != TEMP_FROID) {
				// Si on sort de la grille a gauche
				if ( (j-1) < 0 ) 
					plaque_apres[i][j+1] += vertical(plaque_apres,(plaque_avant[i][j])/6, i, j+1);
				// Si on sort de la grille a droite
				else if ( (j+1) >= taillePlaque ) 
					plaque_apres[i][j-1] += vertical(plaque_apres,(plaque_avant[i][j])/6, i, j-1); 
				else {
					plaque_apres[i][j-1] += vertical(plaque_apres,(plaque_avant[i][j])/6, i, j-1);
					plaque_apres[i][j+1] += vertical(plaque_apres,(plaque_avant[i][j])/6, i, j+1);
				}
				float value = plaque_avant[i][j] - plaque_avant[i][j]/3;
				plaque_apres[i][j] += vertical(plaque_apres,value, i, j);
			}
		}
	}
}


/* Copie la matrice finale dans l'ancienne afin de pouvoir la reutiliser
en tant qu'ancienne plaque */
void matrix_copy(float (*dest)[taillePlaque], float (*src)[taillePlaque]) {
	memcpy(dest, src, taillePlaque*taillePlaque*sizeof(float));
}


/* fonction gerant la methode iterative */
void iterative_way(float (*plaque_avant)[taillePlaque], float (*plaque_apres)[taillePlaque], int nbIterations) {
	for (int i=0; i<nbIterations; i++) {
		calculate_grid(plaque_avant, plaque_apres);
		matrix_copy(plaque_avant, plaque_apres);
		// Permet de reinitialiser la plaque a 0 
		memset(plaque_apres,0,taillePlaque*taillePlaque*sizeof(float));
		if(MFLAG){
			print_plaque(plaque_avant);
		}
	}
}


/* methode principale, c'est elle qui dit quelle methode sera lancee
en fonction des differents parametres recus */
void simulation(int puissance_taillePlaque, int nbIterations, int nbThread) {
	taillePlaque = convertPowToSize(puissance_taillePlaque);

	if (taillePlaque*taillePlaque > TAILLE_MAX || taillePlaque*taillePlaque < TAILLE_MIN) {

		if (MFLAG) {
			printf("%d\n",taillePlaque*taillePlaque);
			printf("Taille de la plaque trop grande\n");
		}
		exit(0);
	}

	float plaque_avant[taillePlaque][taillePlaque];
	float plaque_apres[taillePlaque][taillePlaque];
	memset(plaque_apres,0,taillePlaque*taillePlaque*sizeof(float));

	int i_min = pow(2,sqrt(taillePlaque)-1) - pow(2,sqrt(taillePlaque)-4);
	int i_max = pow(2,sqrt(taillePlaque)-1) + pow(2,sqrt(taillePlaque)-4);

	init_plaque(plaque_avant, i_min, i_max);

	if (nbThread == 1) 
		iterative_way(plaque_avant, plaque_apres, nbIterations);
	else {
		printf("Pas encore géré !");
		exit(0);
	}
}


/* Affiche les options avec lesquelles on a lance le programme */
void display_options(int nb_iterations,struct Param *program_step,struct Param *num_threads,struct Param *nb_case_per_line,bool MFLAG, bool AFLAG){
	int index = 0;

	printf("NB of iterations: %d\n",nb_iterations);

	if(program_step->size != 0){
		printf("Steps to be executed:\n");
		while (index < program_step->size ) {
			printf("%d", program_step->p_array[index]);
			index++;
		}
		puts("");
		index = 0;
	}

	if(num_threads->size != 0){
		printf("Numbers of threads:\n");
		while (index < num_threads->size) {
			printf("%d", num_threads->p_array[index]);
			index++;
		}
		puts("");
		index = 0;
	}

	if(nb_case_per_line->size != 0){
		printf("Size of the grid:\n");
		while (index < nb_case_per_line->size ) {
			printf("%d", nb_case_per_line->p_array[index]);
			index++;
		}
		puts("");
	}

	printf("MFLAG: %d\n",MFLAG);

	printf("AFLAG: %d\n",AFLAG);
}

/*Fonction qui permet d'afficher la moyenne des 10 executions quand l'option -M*/
void chercherMoyenne(double *time_spent){
	double max=0,max2=0;
	int indice1, indice2;
	for(int i=0; i < 10; i++){
		if(time_spent[i] > max){
			max = time_spent[i];
			indice1 = i;
		}
		if(time_spent[i] > max2 && time_spent[i] != max){
			max2= time_spent[i];
			indice2 = i;
		}
	}
	time_spent[indice1] = -1;
	time_spent[indice2] = -1;

	double moy=0;
	for(int i=0; i < 10; i++){
		if(time_spent[i] > 0){
			moy += time_spent[i];
		}
	}
	moy = moy/8;

	printf("Moyenne : %G\n", moy);

}


/* La recuperation des parametres se fait dans le main */
int main(int argc, char *argv[]) {

	int nb_iterations = 10000;

	struct Param program_step;
	program_step.size=0;
	struct Param num_threads;
	num_threads.size=0;

	struct Param nb_case_per_line;
	nb_case_per_line.size=0;




	for (int i =1; i<argc; i++) {
		// Récupération de la taille du problème
		if (strcmp(argv[i],"-s") == 0) {
			char *s_param = argv[i+1];
			//récuperer les différentes tailles dans un tableau de int
			int index = 0;
			nb_case_per_line.p_array =(int*) malloc(((int)strlen(s_param)*sizeof(int)));

			while (s_param[index] != '\0') {
				//et on range les int dedans
				char num = s_param[index] - '0';
				nb_case_per_line.p_array[index] = (int)num;
				//on construit le tableau à la bonne taille
				nb_case_per_line.p_array = realloc(nb_case_per_line.p_array,((int)strlen(s_param)*sizeof(int)));
				index++;
			}
			nb_case_per_line.size = index;
			i++;
		}
		if (strcmp(argv[i],"-m") == 0) {
			//mode execution
			MFLAG=true;
		}
		if (strcmp(argv[i],"-M") == 0) {
			//mode stats
			MMFLAG=true;
		}
		if (strcmp(argv[i],"-a") == 0) {
			AFLAG=true;

		}
		if (strcmp(argv[i],"-i") == 0) {
			nb_iterations = atoi(argv[i+1]);
		}
		if (strcmp(argv[i],"-e") == 0) {
			int index = 0;
			char *e_param = argv[i+1];
			program_step.p_array = (int*) malloc(((int)strlen(e_param)*sizeof(int)));

			while (e_param[index] != '\0') {
				//et on range les int dedans
				char num = e_param[index] - '0';

				program_step.p_array[index] = (int)num;
				//on construit le tableau à la bonne taille
				program_step.p_array = realloc(program_step.p_array,((int)strlen(e_param))*sizeof(int));
				index++;
			}
			program_step.size = index;
		}
		if (strcmp(argv[i],"-t") == 0) {
			int index = 0;
			char *t_param = argv[i+1];
			num_threads.p_array = (int*) malloc(((int)strlen(t_param)*sizeof(int)));

			while (t_param[index] != '\0') {
				//on construit le tableau à la bonne taille
				//et on range les int dedans
				char num = t_param[index] - '0';
				num_threads.p_array[index] = (int)num;
				num_threads.p_array = realloc(num_threads.p_array,((int)strlen(t_param))*sizeof(int));

				index++;
			}
			num_threads.size = index;
		}
	}

	if(MFLAG) 
		display_options(nb_iterations,&program_step,&num_threads,&nb_case_per_line,MFLAG,AFLAG);

	clock_t begin, end;
	double time_spent[10];
	int limit,i=0;

	if(MMFLAG)
		limit=10;
	else
		limit=1;

	for(;i < limit; i++){
		begin = clock();
		simulation(0,nb_iterations,NB_MIN_THREAD);
		end = clock();
		time_spent[i] = (double)(end - begin) / CLOCKS_PER_SEC;
		printf(" Time spent: %G \n",time_spent[i]);
	}

	if(MMFLAG)
		chercherMoyenne(time_spent);

}
