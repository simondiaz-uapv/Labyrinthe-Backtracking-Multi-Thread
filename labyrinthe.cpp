#include <iostream>
#include <fstream>
/*
    Ceci est la classe Labyrinthe qui permet de créer les labyrinthes à partir d'un fichier texte et qu'ils font tous 20x20.
*/
using namespace std;
class Labyrinthe {
    private:
        char grille[20][20];
    public:
        Labyrinthe(string s){
            ifstream fichier(s.c_str());
            if (fichier) {
                for (int i = 0; i < 20; i++) {
                    for (int j = 0; j < 20; j++) {
                        fichier >> grille[i][j]; // Read character into 2D array
                    }
                }
                fichier.close();
            } else {
                cout << "Erreur lors de l'ouverture du fichier." << endl;
            }
        }
};