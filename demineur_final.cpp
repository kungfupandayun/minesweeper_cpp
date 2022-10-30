//***************************************************************************
//  PROGRAMME :demineur.cpp
//  DATE      :janvier 2017
//  OBJET     :Demineur
//  AUTEUR(S) :Jiayun Po, Pierre Vadrot
//***************************************************************************

#include <conio.h>    //permet d'utiliser la fonction getch()
#include <cstdlib>    //permet d'utiliser la commande system("attr") attr = attribu
#include <iostream>   //permet d'utiliser cin et cout
#include <windows.h>  //permet de modifier les couleurs de la fenettre
#include <fstream>    //permet d'utiliser les flux de données
#include <ctime>      //permet d'utiliser l'horloge
#include <unistd.h>   //permet d'utiliser la fonction sleep()

using namespace std;

// Déclaration des constantes
const int LIG_MAX =31, COL_MAX =31, LIG_MIN =8, COL_MIN =8, NB_BOMB_MIN =1, MIN_TEXTE =9, MAX_COLOR =15, MIN_COLOR =0;
const int BOMBE ='X', VIDE =0, DRAPEAU =33, QUESTION =63, SAVE =99, MAX_HS =10, MAX_NAME =4;
const char BORD = 186;

//Déclaration des nouveux types
typedef int t_tab2[LIG_MAX+2][COL_MAX+2];
typedef struct       // Permet de stoquer un seul et unique high score
{
    char name[MAX_NAME];
    float time1;
} t_high_score;

typedef t_high_score t_nom[MAX_HS+1];    // Permet de stoquer tous les high scores

typedef struct     // Permet de stoquer une seule et unique case de la grille
{
    int valeur;
    int visible;
    int tour;
} t_case;

typedef t_case t_tab[LIG_MAX+2][COL_MAX+2];    // Permet de stoquer toute la grille ainsi qu'une bordure (programme plus tolérant)

//Sous-programmes
void afficher_grille(int largeur,int longueur,t_tab grille, t_tab2 grille_drapeau, int langue) //affiche la grille à l'écran
{
    char drapeau, bombe;
	int i, j, nb_bomb = 0, bombs = 0;
    setlocale(LC_ALL, "C");
	//std::system ( "CLS" );
	bombe=BOMBE;
	cout<<"  ";
    for (i=1;i<largeur+1;i++){
    	for(j=1;j<longueur+1;j++){
    		if(grille_drapeau[i][j]==DRAPEAU)
    			bombs++;
    		if(grille[i][j].valeur==BOMBE)
    			nb_bomb++;
		}
	}
	bombs = nb_bomb-bombs;
	for(i=1; i<longueur+1; i++)
    {
        if (i<10)
            cout<<"   "<<i;
        else
            cout<<"  "<<i;
    }
    cout<<endl<<"   ";

    for(i=0; i<longueur; i++)
        cout<<"====";
    cout<<"=";
    cout<<endl;

    for(i=1; i<largeur+1; i++)
    {
        cout<<i;
        if(i<10)
            cout<<"  "<<BORD<<" ";
        else
            cout<<" "<<BORD<<" ";
        for(j=1; j<longueur+1; j++)
        {
           	if ((grille_drapeau[i][j] == DRAPEAU || grille_drapeau[i][j] == QUESTION) && grille[i][j].visible==0){
            	drapeau=grille_drapeau[i][j];
				cout<<drapeau<<" "<<BORD<<" ";}
            else{
				if (grille[i][j].visible == 1)
            	{
            	    if(grille[i][j].valeur==BOMBE)
            	    	cout<<bombe<<" "<<BORD<<" ";
            	    else
            	    	cout<<grille[i][j].valeur<<" "<<BORD<<" ";
            	}
            	else
            	{
                	cout<<"  "<<BORD<<" ";
            	}
    		}
		}
        cout<<"  "<<endl<<"   ";
        for(j=0; j<longueur; j++)
        cout<<"====";
    cout<<"=";
    cout<<endl;
    }
    cout<<"   ";
    cout<<'\t'<<bombs<<" bombs"<<endl;
    setlocale(LC_ALL, "french");
    if(langue==0)
    	cout<<" Entrer pour découvrir la case\n ! Pour poser un drapeau\n ? Pour signaler une case inconnue\n S Pour sauvegarder la partie\n";
    if(langue==1)
    	cout<<" Enter to discover the affair\n ! To put a flag\n ? To report an unknown affair\n S To save the game";
}

void init_bombes_autour(int x, int largeur, int y, int longueur, t_tab grille) //compte le nombre de bombes dans les cases adjacentes à une case de la grille
{
    int bombe, i, j;
    for(i = 1 ; i < largeur+1 ; i++)
    {
        for(j = 1 ; j < longueur+1 ; j++)
        {
            bombe = 0;
            if(grille[i][j].valeur == VIDE)
            {

                if(grille[i+1][j].valeur ==BOMBE)
                    grille[i][j].valeur++;

                if(grille[i+1][j+1].valeur ==BOMBE)
                    grille[i][j].valeur++;

				if(grille[i][j+1].valeur ==BOMBE)
                    grille[i][j].valeur++;

                if(grille[i-1][j+1].valeur ==BOMBE)
                    grille[i][j].valeur++;

                if(grille[i-1][j].valeur ==BOMBE)
                    grille[i][j].valeur++;

                if(grille[i-1][j-1].valeur ==BOMBE)
                    grille[i][j].valeur++;

                if(grille[i][j-1].valeur ==BOMBE)
                    grille[i][j].valeur++;

                if(grille[i+1][j-1].valeur ==BOMBE)
                    grille[i][j].valeur++;
            }
        }
    }
}

void initialiser_grille(int& largeur, int& longueur, int& nb_bomb, t_tab grille, t_tab2 grille_drapeau) // Initialise la grille (cases vides, bombes et bombes à proximité avec le
{                                                                                                       // sous-programme "init_bombes_autour".
    int i, j, x, y;
    for (i=1; i<largeur+1; i++)
    {
        for(j=1; j<longueur+1; j++)
        {
            grille[i][j].valeur = VIDE;
            grille[i][j].visible = 0;
            grille[i][j].tour = 0;
        }
    }
    for (i=1; i<largeur+1; i++)
    {
        for(j=1; j<longueur+1; j++)
        {
        	grille_drapeau[i][j]=VIDE;
    	}
	}
	for (i = 0 ; i < nb_bomb ; i++)
    {
        do{
        	x = rand()%largeur+1;
        	y = rand()%longueur+1;
		}while(grille[x][y].valeur==BOMBE);
        grille[x][y].valeur = BOMBE;
    }
    init_bombes_autour(x, largeur, y, longueur, grille);
}

void gotoxy(int x, int y) //fonction gotoxy
{
COORD pos = {x, y};
SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void saisir_case(int & x, int largeur, int & y, int longueur, char& z)  //saisir une case de la grille en jeu avec curseur
{
	int a;
	gotoxy(x,y);
	do{
	a=getch();
	//sleep(1/10);
	if(a==13) //entrer
	{
		z='0';
	}
	else if(a==80) //bas
	{
		if((y/2)<largeur)
			y=y+2;
	}
	else if(a==75) //gauche
	{
		if((x/4)>=2)
			x=x-4;
	}
	else if(a==77) //droite
	{
		if((x/4)<longueur)
			x=x+4;
	}
	else if(a==72) //haut
	{
		if((y/2)>=2)
			y=y-2;
	}
	else if(a==33)
	{
		z='!';
	}
	else if(a==63)
	{
		z='?';

	}
	else if(a==115)
	{
		x=99;
	}
	gotoxy(x,y);
}while(a!=63 && a!=33 && a!=13 && a!=115);
}

void exploration_grille(int x, int largeur, int y, int longueur, t_tab grille, int& a) // Permet d'étendre la découverte de la grille
{
    int b, i, j;
    ;
    do
    {
        b = 0;
        for (i = 1 ; i < largeur+1 ; i++)
        {
            for (j = 1 ; j < longueur+1 ; j++)
            {
                if (grille[i][j].tour == a && grille[i][j].valeur == VIDE)
                {
                    if(grille[i+1][j].valeur !=BOMBE && grille[i+1][j].tour != a && grille[i+1][j].tour != a-1)
                    {
                        grille[i+1][j].tour = a+1;
                        grille[i+1][j].visible = 1;
                        b++;
                    }
                    if(grille[i+1][j+1].valeur !=BOMBE && grille[i+1][j+1].tour != a && grille[i+1][j+1].tour != a-1)
                    {
                        grille[i+1][j+1].tour = a+1;
                        grille[i+1][j+1].visible = 1;
                        b++;
                    }
                    if(grille[i][j+1].valeur !=BOMBE && grille[i][j+1].tour != a && grille[i][j+1].tour != a-1)
                    {
                        grille[i][j+1].tour = a+1;
                        grille[i][j+1].visible = 1;
                        b++;
                    }
                    if(grille[i-1][j+1].valeur !=BOMBE && grille[i-1][j+1].tour != a && grille[i-1][j+1].tour != a-1)
                    {
                        grille[i-1][j+1].tour = a+1;
                        grille[i-1][j+1].visible = 1;
                        b++;
                    }
                    if(grille[i-1][j].valeur !=BOMBE && grille[i-1][j].tour != a && grille[i-1][j].tour != a-1)
                    {
                        grille[i-1][j].tour = a+1;
                        grille[i-1][j].visible = 1;
                        b++;
                    }
                    if(grille[i-1][j-1].valeur !=BOMBE && grille[i-1][j-1].tour != a && grille[i-1][j-1].tour != a-1)
                    {
                        grille[i-1][j-1].tour = a+1;
                        grille[i-1][j-1].visible = 1;
                        b++;
                    }
                    if(grille[i][j-1].valeur !=BOMBE && grille[i][j-1].tour != a && grille[i][j-1].tour != a-1)
                    {
                        grille[i][j-1].tour = a+1;
                        grille[i][j-1].visible = 1;
                        b++;
                    }

                    if(grille[i+1][j-1].valeur !=BOMBE && grille[i+1][j-1].tour != a && grille[i+1][j-1].tour != a-1)
                    {
                        grille[i+1][j-1].tour = a+1;
                        grille[i+1][j-1].visible = 1;
                        b++;
                    }
                }
            }
        }
        a++;
    }
    while(b != 0);
    a++;
}

void decouvrir_case (int x, int largeur, int y, int longueur, t_tab grille, int& a, t_tab2 grille_drapeau, char z)  // Permet de rendre une case visible au moment de l'affichage de la grille
{
    if (z == '0'){
		grille[x][y].visible = 1;
    	grille[x][y].tour = a;
    	grille_drapeau[x][y]=z;
    	if (grille[x][y].valeur == VIDE)
        	exploration_grille(x, largeur, y, longueur, grille, a);
	}
	else{
		grille_drapeau[x][y]=z;
	}
}

void menu_1 (int langue)                        // Affiche le menu principal du jeu dans la langue par défaut (français)
{                                               // jusqu'à changement dans les paramètres (menu 2).
    if(langue==0)
        cout<<" [1] Nouvelle partie"<<endl<<" [2] Charger une partie"<<endl<<" [3] Options"<<endl<<" [4] Top-score"<<endl<<" [5] Quitter"<<endl<<"Veuillez choisir."<<endl;


    if(langue==1)
        cout<<" [1] New game"<<endl<<" [2] Load game"<<endl<<" [3] Options"<<endl<<" [4] High-score"<<endl<<" [5] Exit"<<endl<<"Please choose."<<endl;
}

void couleur(int texte,int fond)                // Sous programme permettant d'associer les valeurs de texte et fond à des couleurs
{                                               // car les variables ne sont pas acceptées par la commande color.
	switch (fond){
		case 0 :
			switch (texte){
				case 9:
				system("color 09");
				break;
				case 10:
				system("color 0a");
				break;
				case 11:
				system("color 0b");
				break;
				case 12:
				system("color 0c");
				break;
				case 13:
				system("color 0d");
				break;
				case 14:
				system("color 0e");
				break;
				case 15:
				system("color 0f");
				break;
			}
			break;
			case 1 :
			switch (texte){
				case 9:
				system("color 19");
				break;
				case 10:
				system("color 1a");
				break;
				case 11:
				system("color 1b");
				break;
				case 12:
				system("color 1c");
				break;
				case 13:
				system("color 1d");
				break;
				case 14:
				system("color 1e");
				break;
				case 15:
				system("color 1f");
				break;
			}
			break;
			case 2 :
			switch (texte){
				case 9:
				system("color 29");
				break;
				case 10:
				system("color 2a");
				break;
				case 11:
				system("color 2b");
				break;
				case 12:
				system("color 2c");
				break;
				case 13:
				system("color 2d");
				break;
				case 14:
				system("color 2e");
				break;
				case 15:
				system("color 2f");
				break;
			}
			break;
			case 3 :
			switch (texte){
				case 9:
				system("color 39");
				break;
				case 10:
				system("color 3a");
				break;
				case 11:
				system("color 3b");
				break;
				case 12:
				system("color 3c");
				break;
				case 13:
				system("color 3d");
				break;
				case 14:
				system("color 3e");
				break;
				case 15:
				system("color 3f");
				break;
			}
			break;
			case 4 :
			switch (texte){
				case 9:
				system("color 49");
				break;
				case 10:
				system("color 4a");
				break;
				case 11:
				system("color 4b");
				break;
				case 12:
				system("color 4c");
				break;
				case 13:
				system("color 4d");
				break;
				case 14:
				system("color 4e");
				break;
				case 15:
				system("color 4f");
				break;
			}
			break;
			case 5 :
			switch (texte){
				case 9:
				system("color 59");
				break;
				case 10:
				system("color 5a");
				break;
				case 11:
				system("color 5b");
				break;
				case 12:
				system("color 5c");
				break;
				case 13:
				system("color 5d");
				break;
				case 14:
				system("color 5e");
				break;
				case 15:
				system("color 5f");
				break;
			}
			break;
			case 6 :
			switch (texte){
				case 9:
				system("color 69");
				break;
				case 10:
				system("color 6a");
				break;
				case 11:
				system("color 6b");
				break;
				case 12:
				system("color 6c");
				break;
				case 13:
				system("color 6d");
				break;
				case 14:
				system("color 6e");
				break;
				case 15:
				system("color 6f");
				break;
			}
			break;
			case 7 :
			switch (texte){
				case 9:
				system("color 79");
				break;
				case 10:
				system("color 7a");
				break;
				case 11:
				system("color 7b");
				break;
				case 12:
				system("color 7c");
				break;
				case 13:
				system("color 7d");
				break;
				case 14:
				system("color 7e");
				break;
				case 15:
				system("color 7f");
				break;
			}
			break;
			case 8 :
			switch (texte){
				case 9:
				system("color 89");
				break;
				case 10:
				system("color 8a");
				break;
				case 11:
				system("color 8b");
				break;
				case 12:
				system("color 8c");
				break;
				case 13:
				system("color 8d");
				break;
				case 14:
				system("color 8e");
				break;
				case 15:
				system("color 8f");
				break;
			}
			break;
			case 9 :
			switch (texte){
				case 10:
				system("color 9a");
				break;
				case 11:
				system("color 9b");
				break;
				case 12:
				system("color 9c");
				break;
				case 13:
				system("color 9d");
				break;
				case 14:
				system("color 9e");
				break;
				case 15:
				system("color 9f");
				break;
			}
			break;
			case 10 :
			switch (texte){
				case 9:
				system("color a9");
				break;
				case 11:
				system("color ab");
				break;
				case 12:
				system("color ac");
				break;
				case 13:
				system("color ad");
				break;
				case 14:
				system("color ae");
				break;
				case 15:
				system("color af");
				break;
			}
			break;
			case 11 :
			switch (texte){
				case 9:
				system("color b9");
				break;
				case 10:
				system("color ba");
				break;
				case 12:
				system("color bc");
				break;
				case 13:
				system("color bd");
				break;
				case 14:
				system("color be");
				break;
				case 15:
				system("color bf");
				break;
			}
			break;
			case 12 :
			switch (texte){
				case 9:
				system("color c9");
				break;
				case 10:
				system("color ca");
				break;
				case 11:
				system("color cb");
				break;
				case 13:
				system("color cd");
				break;
				case 14:
				system("color ce");
				break;
				case 15:
				system("color cf");
				break;
			}
			break;
			case 14 :
			switch (texte){
				case 9:
				system("color e9");
				break;
				case 10:
				system("color ea");
				break;
				case 11:
				system("color eb");
				break;
				case 12:
				system("color ec");
				break;
				case 13:
				system("color ed");
				break;
				case 15:
				system("color ef");
				break;
			}
			break;
			case 15 :
			switch (texte){
				case 9:
				system("color f9");
				break;
				case 10:
				system("color fa");
				break;
				case 11:
				system("color fb");
				break;
				case 12:
				system("color fc");
				break;
				case 13:
				system("color fd");
				break;
				case 14:
				system("color fe");
				break;
			}
			break;
			case 13 :
			switch (texte){
				case 9:
				system("color d9");
				break;
				case 10:
				system("color da");
				break;
				case 11:
				system("color db");
				break;
				case 12:
				system("color dc");
				break;
				case 14:
				system("color de");
				break;
				case 15:
				system("color df");
				break;
			}
			break;
	}

}

void menu_2(int& largeur, int& longueur, int& nb_bomb, int& langue, int texte, int fond)  // Le menu 2 sert à modifier les paramètres du jeu (grille et apparence) avec un curseur
{
    int choix_menu_2, a, y=0;
    do
    {
        std::system ( "CLS" );
        couleur(texte,fond);
        if(langue==0)
            cout<<" [1] Langue   : Français"<<endl<<" [2] Longueur : "<<longueur<<endl<<" [3] Largeur  : "<<largeur<<endl<<" [4] Bombes   : "<<nb_bomb<<endl<<" Color :"<<endl<<" [5] Fond"<<endl<<" [6] Texte"<<endl<<" [7] Retour au menu 1"<<endl<<"Veuillez choisir"<<endl;

        if(langue==1)
            cout<<" [1] Language : English"<<endl<<" [2] Lenght   : "<<longueur<<endl<<" [3] Widht    : "<<largeur<<endl<<" [4] Bombs    : "<<nb_bomb<<endl<<" Color :"<<endl<<" [5] Background"<<endl<<" [6] Text"<<endl<<" [7] Back to menu 1"<<endl<<"Please choose"<<endl;
	do{
	gotoxy(2,y);
	a=getch();
	//sleep(1/10);
	if(a==13) //entrer
	{
		choix_menu_2=y+1;
		if(y>3)
			choix_menu_2=y;
	}
	else if(a==80) //bas
	{
		if(y==3)
			y++;
		if(y<7)
			y++;
	}
	else if(a==72) //haut
	{
		if(y==5)
			y--;
		if(y>0)
			y--;
	}
}while(a!=13);
		switch (choix_menu_2)
        {
        case 1:
            if (langue==1)
                langue=0;
            else
                langue=1;
            break;
        case 2:
            if(longueur < COL_MAX)
                longueur++;
            else
                longueur = COL_MIN;
            break;
        case 3:
            if(largeur < LIG_MAX)
                largeur++;
            else
                largeur = LIG_MIN;
            break;
        case 4:
            if(nb_bomb < (longueur*largeur/3))
                nb_bomb++;
            else
                nb_bomb = NB_BOMB_MIN;
            break;
        case 5:
            do{
			if(fond < MAX_COLOR)
                fond++;
            else
                fond = MIN_COLOR;
            }while(texte==fond);
            break;
        case 6:
            do{
				if(texte < MAX_COLOR)
                	texte++;
            	else
                	texte = MIN_TEXTE;

            }while(texte==fond);
            break;

        }
    }
    while(choix_menu_2 != 7);
}

void afficher_bombes(int largeur, int longueur, t_tab grille) // En cas d'échec ce sous-programme rend les bombes visible pour l'affichage de la grille en fin de partie
{
    int i, j;
    for(i = 1 ; i < largeur+1 ; i++)
    {
        for(j = 1 ; j < longueur+1 ; j++)
        {
            if(grille[i][j].valeur==BOMBE)
                grille[i][j].visible=1;

        }
    }
}

bool verif_end_game(int largeur, int longueur, int nb_bomb, t_tab grille) //vérifie si il y a une victoire de l'utilisateur
{
    int i, j, bombs = 0;
    bool fin;
    for(i = 1 ; i < largeur+1 ; i++)
    {
        for(j = 1 ; j < longueur+1 ; j++)
        {
            if(grille[i][j].visible == 0)
            {
                bombs++;
            }
        }
    }
    if(bombs == nb_bomb)
    {
        fin = true;
    }
    else
    {
        fin = false;
    }
    return(fin);
}

void sauver_partie(int largeur, int longueur, int nb_bomb, int duration, t_tab grille, t_tab2 grille_drapeau) // Sauvegarde la partie en cours dans le fichier "stocked_game.bin"
{
	ofstream f;
	int i, j;
	f.open ("stocked_game.bin", ios::in | ios::binary);

    if (!f.is_open())
        cout << "Can't find \"stocked_game.bin\"" << endl;
    else
    {
		f.write ((char *)&largeur, sizeof(int));
		f.write ((char *)&longueur, sizeof(int));
		f.write ((char *)&nb_bomb, sizeof(int));
		f.write ((char *)&duration, sizeof(int));
		for(i=1;i<largeur+1;i++)
        {
        	for(j=1;j<longueur+1;j++)
        	{
        		f.write ((char *)&grille[i][j], 3*sizeof(int)); //stock "grille"
			}
		}
		for(i=1;i<largeur+1;i++)
        {
        	for(j=1;j<longueur+1;j++)
        	{
        		f.write ((char *)&grille_drapeau[i][j], sizeof(int)); //stock "grille_tableau"
			}
		}
    }
    f.close();
}

void permuter_high_score(t_high_score& A, t_high_score& B)  // Permute 2 high-scores de la liste
{
	t_high_score score;
	score = B;
	B = A;
	A = score;
}

void ranger_min_temps(t_nom high_score, int debut, int& id) //Range les high-score du plus rapide au plus lent
{
	int min;
	int i;

	min = high_score[debut].time1;
	id = debut;
	for (i = debut; i < MAX_HS+1; i++)
	{
		if (high_score[i].time1 < min)
		{
			min = high_score[i].time1;
			id = i;
		}
	}
}

void classer_high_score(t_nom high_score) //classe les high-scores du plus rapide au plus lent avec "ranger_min_temps()" et "permuter_high_score()"
{
	int i;
	int debut = 0;
	int id = 0;
	for (i = 0; i < MAX_HS+1; i++)
	{
		ranger_min_temps(high_score, debut, id);
		permuter_high_score(high_score[debut], high_score[id]);
		debut++;
	}
}

void ajouter_high_score(int langue, int time1, t_nom high_score) //Ajoute un high-score à la liste
{
	if(langue == 0)
		cout<<"Saisir ton nom en "<<MAX_NAME<<" lettres : ";

	if(langue == 1)
		cout<<"Write your name in "<<MAX_NAME<<" letters : ";

	cin>>high_score[MAX_HS].name;
	high_score[MAX_HS].time1=time1;
}

void jouer(int largeur, int longueur, int nb_bomb, int duration_restart, t_tab grille,t_tab2 grille_drapeau, int langue, t_nom high_score,float duration) // "jouer" une partie
{
	int x=5, y=2, a=2, i;
	char z;
    bool fin = false;
	std::clock_t start;
    std::system ( "CLS" );
    start = std::clock();
    afficher_grille(largeur, longueur, grille, grille_drapeau, langue);
    do
    {
        saisir_case(x,largeur,y,longueur, z);
        std::system ( "CLS" );
		if(x==SAVE || y==SAVE){
			fin=true;
			duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        	duration = duration + duration_restart;
			sauver_partie(largeur,longueur,nb_bomb,duration, grille,grille_drapeau);
			if(langue==0)
				cout<<endl<<"Sauvegardée"<<endl;
			if(langue==1)
				cout<<endl<<"Saved"<<endl;
		}else{
		
        if(grille[y/2][x/4].valeur==BOMBE && z == '0')
        {
            duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
            duration = duration + duration_restart;
			fin = true;
            afficher_bombes(largeur, longueur,grille);
            afficher_grille(largeur, longueur, grille, grille_drapeau,langue);
			gotoxy(0,0);
            for(i=0;i<(largeur*2)+5;i++)
				cout<<endl;
			if(langue == 0){
				cout<<endl<<endl<<"PERDU !!!!"<<endl<<endl;
				cout<<"Temps avant explosion : "<< duration <<" secondes\n";}

			if(langue == 1){
				cout<<endl<<endl<<"LOOSED !!!!"<<endl<<endl;
				cout<<"Time before explosion : "<< duration <<" seconds\n";}
		}
        else
        {
            decouvrir_case(y/2, largeur, x/4, longueur, grille, a, grille_drapeau, z);
            fin = verif_end_game(largeur, longueur, nb_bomb, grille);
            if (fin == true)
            {
                duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
				duration = duration + duration_restart;
				if (langue == 0){
					cout<<endl<<endl<<"VICTOIRE !!!!"<<endl<<endl;
					cout<<"Temps de résolution : "<< duration <<" secondes\n";}
				if (langue == 1){
					cout<<endl<<endl<<"VICTORY !!!!"<<endl<<endl;
					cout<<"Temps de résolution : "<< duration <<" seconds\n";}
				ajouter_high_score(langue, duration, high_score);
    			classer_high_score(high_score);
            }
        }
		if (fin==false)
			afficher_grille(largeur, longueur, grille, grille_drapeau,langue);
    }
}
    while(fin != true);
    system("PAUSE");
}

void dificultee(int& largeur, int& longueur, int& nb_bomb, int langue) //permet de sélectionner la difficulté du jeu (avec curseur)
{
    int choix_dificultee, a, y=0;
    std::system ( "CLS" );
	if(langue == 0)
        cout<<" [1] Débutant"<<endl<<" [2] Intermédiaire"<<endl<<" [3] Expert"<<endl<<" [4] Personnalisé"<<endl<<"Choisissez un mode de jeu."<<endl;

    if(langue == 1)
        cout<<" [1] Beginer"<<endl<<" [2] Intermediate"<<endl<<" [3] Expert"<<endl<<" [4] Custom"<<endl<<"Choose a game mode."<<endl;
{
	do{
	gotoxy(2,y);
	a=getch();
	//sleep(1/10);
	if(a==13) //entrer
	{
		choix_dificultee=y+1;
	}
	else if(a==80) //flèche bas
	{
		if(y<3)
			y++;
	}
	else if(a==72) //flèche haut
	{
		if(y>0)
			y--;
	}
}while(a!=13);
    switch (choix_dificultee){
        case 1: //standard débutant
            largeur = 8;
            longueur = 8;
            nb_bomb = 10;
            break;
        case 2: //standard intermédiaire
            largeur = 16;
            longueur = 16;
            nb_bomb = 40;
            break;
        case 3: //standard expert
            largeur = 16;
            longueur = 31;
            nb_bomb = 99;
            break;
    }
}
}

void charger_partie( t_tab grille, t_tab2 grille_drapeau, int langue, t_nom high_score) // Charge une sauvegarde existante dans le fichier "stocked_game.bin"
{
	int largeur, longueur, nb_bomb;
	ifstream file;
	int i, j, duration_restart;
	float time1;
	file.open ("stocked_game.bin", ios::in | ios::binary);

    if (!file.is_open())
        cout << "Can't find \"stocked_game.bin\"" << endl;
    else
    {
		file.read ((char *)&largeur, sizeof(int));
		file.read ((char *)&longueur, sizeof(int));
		file.read ((char *)&nb_bomb, sizeof(int));
		file.read ((char *)&duration_restart, sizeof(int));
		for(i=1;i<largeur+1;i++)
        {
        	for(j=1;j<longueur+1;j++)
        	{
        		file.read ((char *)&grille[i][j], 3*sizeof(int)); // read "grille"
			}
		}
		for(i=1;i<largeur+1;i++)
        {
        	for(j=1;j<longueur+1;j++)
        	{
        		file.read ((char *)&grille_drapeau[i][j], sizeof(int)); // read "grille_drapeau"
			}
		}
    }
    file.close();
    jouer(largeur, longueur, nb_bomb, duration_restart, grille, grille_drapeau, langue, high_score,0);
}

void recup_high_score(int langue, t_nom & high_score)     // Récupère et stoque les high-scores dans un tableau "high_score"
{
	int i;
	ifstream f;
    f.open ("high_score.bin", ios::in| ios::binary);

    if (!f.is_open())
        cout << "Impossible d'ouvrir le fichier en lecture !" << endl;
    else {
        for(i=0;i<MAX_HS;i++)
			f.read ((char *)&high_score[i], MAX_NAME*sizeof(char)+sizeof(int));
    }
    f.close();
}

void stock_high_score(int langue, t_nom high_score)    // Enregistre la liste des high-score dans le fichier "high_score.bin"
{
	int i;
	ofstream f;
    f.open ("high_score.bin", ios::in| ios::binary);

    if (!f.is_open())
        cout << "Impossible d'ouvrir le fichier en lecture !" << endl;
    else {
        for(i=0;i<MAX_HS;i++)
			f.write ((char *)&high_score[i], MAX_NAME*sizeof(char)+sizeof(int));
    }
    f.close();
}

void afficher_high_score(int langue, t_nom high_score)   // Affiche la liste des high-score à l'écran
{
	int i;
	system("CLS");
	if (langue == 0)
		cout<<"\tNom\tTemps\n";
	if (langue == 1)
		cout<<'\t'<<"Name"<<'\t'<<"Time\n";

	for (i=0;i<MAX_HS;i++)
		cout<<'\t'<<high_score[i].name<<'\t'<<high_score[i].time1<<endl;

	system("pause");
}

int curseur_menu_1()  // Gère le curseur dans le menu 1 et retourne le choix de l'utilisateur
{
	int a, choix_menu_1, y=0;
	do{
	gotoxy(2,y);
	a=getch();
	//sleep(1/10);
	if(a==13) //entrer
	{
		choix_menu_1=y+1;
	}
	else if(a==80) //flèche bas
	{
		if(y<4)
			y++;
	}
	else if(a==72) //flèche haut
	{
		if(y>0)
			y--;
	}
}while(a!=13);
	return(choix_menu_1);
}

void init_high_score (t_nom high_score) //Permet d'initialiser les high_score et doit être exécuté au tout premier lancement du jeu  
{
	int i, j;
	for (i=0;i<MAX_HS+1;i++){
		for(j=0;j<MAX_NAME;j++)
			high_score[i].name[j]='_';
		
		high_score[i].time1=999;
	}
}

int main()  // Programme principal
{
    int largeur=8, longueur=8, nb_bomb=10, choix_menu_1, langue=0, texte = MAX_COLOR, fond = MIN_COLOR;
    float time1;
	t_tab grille;
    t_tab2 grille_drapeau;
    t_nom high_score;
	setlocale(LC_ALL, "french");
    system("mode 140,70");
    srand(time(NULL));
    couleur(texte,fond);
	recup_high_score(langue, high_score);
	//init_high_score (high_score);        //enlever le commentaire au tout premier lancement du jeu
	cout << "Welcome from the game !!!" << endl;
    do
    {
		std::system ( "CLS" );
        menu_1(langue);
        choix_menu_1=curseur_menu_1();
		switch (choix_menu_1)
        {
        case 1:
			dificultee(largeur,longueur,nb_bomb, langue);
            initialiser_grille(largeur,longueur,nb_bomb,grille,grille_drapeau);
			jouer(largeur, longueur, nb_bomb, 0, grille, grille_drapeau, langue, high_score,0);
			break;
        case 2:
            charger_partie(/*largeur,longueur,nb_bomb,*/grille,grille_drapeau,langue,high_score);
			break;
        case 3:
            menu_2(largeur, longueur, nb_bomb, langue, texte, fond);
            break;
        case 4:
			afficher_high_score(langue, high_score);
        	break;
        }
    }
    while(choix_menu_1!=5);
    stock_high_score(langue, high_score);
    return 0;
}
