#include <iostream>
#include <vector>
#include <thread>
#include "labyrinthesManager.cpp"

using namespace std;

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
 * @brief Algorithme de backtracking pour résoudre le labyrinthe.
 */
bool backtracking(Labyrinthe& labyrinthe, int x, int y, string& chemin, Labyrinthe* labyrinthePrime = nullptr) {
    // Vérifier si c'est la sortie
    if (x == get<0>(labyrinthe.getSortie()) && y == get<1>(labyrinthe.getSortie())) {
        cout << "Sortie trouvée à la position : [" << x << "," << y << "]" << endl;
        cout << "Chemin suivi : " << chemin << endl;
        return true;
    }

    // Vérifier si la case est un mur, déjà visitée ou infranchissable
    if (labyrinthe.getCase(x, y) == '*' || labyrinthe.getCase(x, y) == '#') {
        return false;
    }

    cout << "Position actuelle : [" << x << "," << y << "]" << endl;

    // Marquer la case actuelle comme visitée
    char caseOriginale = labyrinthe.getCase(x, y);
    labyrinthe.marquerCase(x, y, '*');

    // Obtenir les directions possibles
    vector<char> directions = remplirVecteurDesDirectionsPossibles(labyrinthe, x, y);
    cout << "Directions possibles depuis [" << x << "," << y << "] : ";
    for (char direction : directions) cout << direction << " ";
    cout << endl;

    // Explorer chaque direction
    for (char direction : directions) {
        int newX = x, newY = y;

        // Déplacer en fonction de la direction
        switch (direction) {
            case 'H': newX--; chemin += 'H'; break;
            case 'B': newX++; chemin += 'B'; break;
            case 'G': newY--; chemin += 'G'; break;
            case 'D': newY++; chemin += 'D'; break;
        }

        // Si on tombe sur une TNT, on passe au labyrinthePrime
        if (labyrinthe.getCase(newX, newY) == 'T' && labyrinthePrime) {
            cout << "TNT trouvée ! Passage au labyrinthe prime." << endl;
            labyrinthePrime->copierCasesVisitees(labyrinthe);
            if (backtracking(*labyrinthePrime, newX, newY, chemin, nullptr)) return true;
        }

        // Appel récursif
        if (backtracking(labyrinthe, newX, newY, chemin, labyrinthePrime)) return true;

        // Revenir en arrière
        chemin.pop_back();
    }

    // Réinitialiser la case
    labyrinthe.marquerCase(x, y, caseOriginale);

    cout << "Retour en arrière depuis [" << x << "," << y << "]" << endl;
    return false;
}

int main() {
    string cheminFichier = "labyrinthe.txt";
    vector<Labyrinthe> labyrinthes = chargerLabyrinthes(cheminFichier);

    cout << "Résolution du labyrinthe 1 :" << endl;
    Labyrinthe& labyrinthe = labyrinthes[0];
    Labyrinthe* labyrinthePrime = nullptr;

    if (1 < labyrinthes.size() && labyrinthes[1].getNom() == labyrinthe.getNom() + "Prime") {
        labyrinthePrime = &labyrinthes[1];
    }

    int x = get<0>(labyrinthe.getEntree());
    int y = get<1>(labyrinthe.getEntree());
    string chemin = "";

    if (backtracking(labyrinthe, x, y, chemin, labyrinthePrime)) {
        cout << "Chemin trouvé ! " << chemin << endl;
        cout << "Labyrinthe résolu :" << endl;
        labyrinthe.AfficherLabyrintheAvecCheminEnVert();
    } else {
        cout << "Aucun chemin trouvé." << endl;
    }

    cout << "Fin du programme." << endl;
    return 0;
}
