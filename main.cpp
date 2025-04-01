#include <iostream>
#include <vector>
#include <thread>
#include "labyrinthesManager.cpp"
using namespace std;

/**
 * @brief Fonction qui remplit le vecteur des directions possibles
 * 
 * @param labyrinthe 
 * @param v 
 * @param curseur 
 */
vector<char> remplirVecteurDesDirectionsPossibles(Labyrinthe& labyrinthe, Curseur& curseur){
    vector<char> v;
    int positionX = curseur.getX();
    int positionY = curseur.getY();
    if(labyrinthe.estJouable(positionX-1, positionY)){
        v.push_back('H');
    }
    if(labyrinthe.estJouable(positionX+1, positionY)){
        v.push_back('B');
    }
    if(labyrinthe.estJouable(positionX, positionY-1)){
        v.push_back('G');
    }
    if(labyrinthe.estJouable(positionX, positionY+1)){
        v.push_back('D');
    }
    return v;
}

//En considérant que le labyrinthe est un tableau 2D de caractères,
// En considérant que le personage peut faire que haut,bas,gauche et droite 
bool backtracking(Labyrinthe& labyrinthe, Curseur& curseur, int profondeur, string& chemin) {

    if (curseur.getX() == get<0>(labyrinthe.getSortie()) && curseur.getY() == get<1>(labyrinthe.getSortie())) {
        cout << "Sortie trouvée à la position : [" << curseur.getX() << "," << curseur.getY() << "]" << endl;
        cout << "Chemin suivi : " << chemin << endl;
        return true;
    }

    int x = curseur.getX();
    int y = curseur.getY();
    labyrinthe.marquerCase(x, y, true);

    vector<char> directions = remplirVecteurDesDirectionsPossibles(labyrinthe, curseur);
    for (char direction : directions) {
        switch (direction) {
            case 'H':
                curseur.allerEnHaut();
                chemin += 'H';
                break;
            case 'B':
                curseur.allerEnBas();
                chemin += 'B';
                break;
            case 'G':
                curseur.allerAGauche();
                chemin += 'G';
                break;
            case 'D':
                curseur.allerADroite();
                chemin += 'D';
                break;
        }

        if (backtracking(labyrinthe, curseur, profondeur + 1, chemin)) {
            return true;
        }

        // Revenir en arrière
        switch (direction) {
            case 'H':
                curseur.allerEnBas();
                chemin.pop_back();
                break;
            case 'B':
                curseur.allerEnHaut();
                chemin.pop_back();
                break;
            case 'G':
                curseur.allerADroite();
                chemin.pop_back();
                break;
            case 'D':
                curseur.allerAGauche();
                chemin.pop_back();
                break;
        }
    }

    labyrinthe.marquerCase(x, y, false);

    return false;
}

int main() {

    string cheminFichier = "labyrinthe.txt";
    vector<Labyrinthe> labyrinthes = chargerLabyrinthes(cheminFichier);
    cout<<"Labyrinthes chargés : Execution backtracking sans thread :"<<endl;
    cout<<"voici les labyrinthes"<<endl;
    afficherLabyrinthes(labyrinthes);

    for (int i = 0; i < labyrinthes.size(); ++i) {
        cout << "Résolution du " << i + 1 << "ème labyrinthe : " << endl;

        Labyrinthe& labyrinthe = labyrinthes[i];
        int x = get<0>(labyrinthe.getEntree());
        int y = get<1>(labyrinthe.getEntree());
        Curseur curseur(x, y);
        string chemin = ""; // Chemin suivi
        if (backtracking(labyrinthe, curseur, 0, chemin)) {
            cout << "Chemin trouvé pour le labyrinthe " << i + 1 << " : " << chemin << endl;
        } else {
            cout << "Aucun chemin trouvé pour le labyrinthe " << i + 1 << "." << endl;
        }
    }

    return 0;
}

