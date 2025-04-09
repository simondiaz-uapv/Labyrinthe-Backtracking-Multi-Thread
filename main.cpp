#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
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


void resolutionSequentielle(vector<Labyrinthe>& labyrinthes) {
    cout << "Execution de la methode sequentielle..." << endl;
    int indexLabyrinthePrime = -4;
    for (int i = 0; i < labyrinthes.size(); i++) {
        if (i == indexLabyrinthePrime) {
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
}

void resolutionThreadParDirection(Labyrinthe& labyrinthe, Labyrinthe* labyrinthePrime) {
    cout << "Execution de la methode avec un thread par direction possible..." << endl;

    int x = get<0>(labyrinthe.getEntree());
    int y = get<1>(labyrinthe.getEntree());
    vector<char> directions = remplirVecteurDesDirectionsPossibles(labyrinthe, x, y);
    vector<thread> threads;
    for (char direction : directions) {
        // Créer un thread pour chaque direction
        threads.emplace_back([&, direction]() {
            int newX = x, newY = y;
            string chemin = "";

            switch (direction) {
                case 'H': newX--; chemin += 'H'; break;
                case 'B': newX++; chemin += 'B'; break;
                case 'G': newY--; chemin += 'G'; break;
                case 'D': newY++; chemin += 'D'; break;
            }

            if (backtracking(labyrinthe, newX, newY, chemin, labyrinthePrime)) {
                labyrinthe.AfficherLabyrintheAvecCheminEnVert();
            } else {
                cout << "Aucun chemin trouve pour la direction " << direction << "." << endl;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}

void resolutionThreadParLabyrinthe(vector<Labyrinthe>& labyrinthes) {
    cout << "Execution de la methode avec un thread par labyrinthe..." << endl;

    vector<thread> threads;

    for (int i = 0; i < labyrinthes.size(); i++) {
        Labyrinthe& labyrinthe = labyrinthes[i];
        Labyrinthe* labyrinthePrime = nullptr;

        if (i + 1 < labyrinthes.size() && labyrinthes[i + 1].getNom() == labyrinthe.getNom() + "Prime") {
            labyrinthePrime = &labyrinthes[i + 1];
            i++; // Sauter le labyrinthe "prime" dans la boucle principale
        }

        threads.emplace_back([&, i, labyrinthePrime]() {
            int x = get<0>(labyrinthes[i].getEntree());
            int y = get<1>(labyrinthes[i].getEntree());
            string chemin = "";

            if (backtracking(labyrinthes[i], x, y, chemin, labyrinthePrime)) {
                labyrinthes[i].AfficherLabyrintheAvecCheminEnVert();
            } else {
                cout << "Aucun chemin trouve pour le labyrinthe " << labyrinthes[i].getNom() << "." << endl;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    string cheminFichier = "labyrinthe.txt";
    vector<Labyrinthe> labyrinthes = chargerLabyrinthes(cheminFichier); 

    cout << "Choisissez une methode de resolution :" << endl;
    cout << "1. Methode sequentielle" << endl;
    cout << "2. Un thread par direction possible" << endl;
    cout << "3. Un thread par labyrinthe" << endl;
    cout << "Votre choix : ";

    int choix;
    cin >> choix;

    auto start = chrono::high_resolution_clock::now();

    switch (choix) {
        case 1:
            resolutionSequentielle(labyrinthes);
            break;
        case 2:
            for (int i = 0; i < labyrinthes.size(); i++) {
                Labyrinthe* labyrinthePrime = nullptr;
                if (i + 1 < labyrinthes.size() && labyrinthes[i + 1].getNom() == labyrinthes[i].getNom() + "Prime") {
                    labyrinthePrime = &labyrinthes[i + 1];
                    i++; // Sauter le labyrinthe "prime"
                }
                resolutionThreadParDirection(labyrinthes[i], labyrinthePrime);
            }
            break;
        case 3:
            resolutionThreadParLabyrinthe(labyrinthes);
            break;
        default:
            cout << "Choix invalide." << endl;
            return 1;
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Temps d'execution : " << duration << " ms" << endl;

    cout << "Objets collectes :" << endl;
    for (char obj : objetRecoltes) {
        cout << obj << " ";
    }
    return 0;
}