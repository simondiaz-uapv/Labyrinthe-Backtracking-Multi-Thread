#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include "labyrinthesManager.cpp"

using namespace std;

vector<char> objetRecoltes ;

/**
 * @brief Remplit un vecteur avec les directions possibles à partir de la position actuelle.
 */
vector<char> remplirVecteurDesDirectionsPossibles(Labyrinthe& labyrinthe, int x, int y) {
    vector<char> v;

    if (labyrinthe.estJouable(x - 1, y) && labyrinthe.getCase(x - 1, y) != '*') v.push_back('H'); // Haut
    if (labyrinthe.estJouable(x + 1, y) && labyrinthe.getCase(x + 1, y) != '*') v.push_back('B'); // Bas
    if (labyrinthe.estJouable(x, y - 1) && labyrinthe.getCase(x, y - 1) != '*') v.push_back('G'); // Gauche
    if (labyrinthe.estJouable(x, y + 1) && labyrinthe.getCase(x, y + 1) != '*') v.push_back('D'); // Droite

    return v;
}

/**
 * @brief Algorithme de backtracking pour resoudre le labyrinthe.
 */
bool backtracking(Labyrinthe& labyrinthe, int x, int y, string& chemin, Labyrinthe* labyrinthePrime = nullptr) {
    // Verifier si c'est la sortie
    if (x == get<0>(labyrinthe.getSortie()) && y == get<1>(labyrinthe.getSortie())) {
        const vector<char>& objetsARecuperer = labyrinthe.getObjetsARecuperer();
        if (all_of(objetsARecuperer.begin(), objetsARecuperer.end(), [](char obj) {
            return find(objetRecoltes.begin(), objetRecoltes.end(), obj) != objetRecoltes.end();
        })) {
            cout << "Tous les objets necessaires ont ete collectes. Sortie trouvee à la position : [" << x << "," << y << "]" << endl;
            return true;
        } 
    }

    // Verifier si la case est un mur, dejà visitee ou infranchissable
    if (labyrinthe.getCase(x, y) == '*' || labyrinthe.getCase(x, y) == '#') {
        return false;
    }

    // Collecter l'objet si present sur la case
    char caseOriginale = labyrinthe.getCase(x, y);
    bool objetCollecte = false;
    if (isalpha(caseOriginale) && caseOriginale != 'M' && caseOriginale != 'A' && caseOriginale != 'D' && caseOriginale != 'T') {
        if (find(objetRecoltes.begin(), objetRecoltes.end(), caseOriginale) == objetRecoltes.end()) {
            objetRecoltes.push_back(caseOriginale);
            objetCollecte = true; // Marquer que l'objet a ete collecte
        }
    }

    // Marquer la case actuelle comme visitee
    labyrinthe.marquerCase(x, y, '*');

    // Obtenir les directions possibles
    vector<char> directions = remplirVecteurDesDirectionsPossibles(labyrinthe, x, y);

    // Explorer chaque direction
    for (char direction : directions) {
        int newX = x, newY = y;

        // Deplacer en fonction de la direction
        switch (direction) {
            case 'H': newX--; chemin += 'H'; break;
            case 'B': newX++; chemin += 'B'; break;
            case 'G': newY--; chemin += 'G'; break;
            case 'D': newY++; chemin += 'D'; break;
        }

        // Si on tombe sur une TNT, on passe au labyrinthePrime
        if (labyrinthe.getCase(newX, newY) == 'T' && labyrinthePrime) {
            const vector<char>& objetsARecuperer = labyrinthe.getObjetsARecuperer();
            if (all_of(objetsARecuperer.begin(), objetsARecuperer.end(), [](char obj) {
                return find(objetRecoltes.begin(), objetRecoltes.end(), obj) != objetRecoltes.end();
            })) {
                cout << "LabyrinthePrime trouve et tous les objets collectes !" << endl;
                labyrinthePrime->copierCasesVisitees(labyrinthe);
                if (backtracking(*labyrinthePrime, newX, newY, chemin, nullptr)) {
                    labyrinthePrime->AfficherLabyrintheAvecCheminEnVert();
                    return true;
                }
            }
        }

        if (backtracking(labyrinthe, newX, newY, chemin, labyrinthePrime)) return true;

        // Revenir en arrière
        chemin.pop_back();
    }

    // Reinitialiser la case
    labyrinthe.marquerCase(x, y, caseOriginale);

    // Si un objet a ete collecte sur cette case, le retirer
    if (objetCollecte) {
        objetRecoltes.pop_back();
    }

    return false;
}

int main() {
    string cheminFichier = "labyrinthe.txt";
    vector<Labyrinthe> labyrinthes = chargerLabyrinthes(cheminFichier); 
    cout<<"Resolution des labyrinthes :"<<endl;
    int indexLabyrinthePrime = -4;
    for (int i = 0; i < labyrinthes.size(); i++) {
        if(i == indexLabyrinthePrime) {
            continue; // Passer le labyrinthe "prime"
        }
        
        Labyrinthe& labyrinthe = labyrinthes[i];
        cout << "Resolution du labyrinthe " << labyrinthe.getNom() << " :" << endl;
        Labyrinthe* labyrinthePrime = nullptr;
        
        if (i + 1 < labyrinthes.size() && labyrinthes[i + 1].getNom() == labyrinthe.getNom() + "Prime") {
            labyrinthePrime = &labyrinthes[i + 1];
            indexLabyrinthePrime = i + 1;
        }

        int x = get<0>(labyrinthe.getEntree());
        int y = get<1>(labyrinthe.getEntree());
        string chemin = "";

        if (backtracking(labyrinthe, x, y, chemin, labyrinthePrime)) {
            labyrinthe.AfficherLabyrintheAvecCheminEnVert();
        } else {
            cout << "Aucun chemin trouve." << endl;
        }
    }
    cout << "Objets collectees : "<<endl;
    for (char obj : objetRecoltes) {
        cout << obj << " ";
    }
    return 0;
}

