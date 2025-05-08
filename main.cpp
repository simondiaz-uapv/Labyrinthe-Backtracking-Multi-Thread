#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include "labyrinthesManager.cpp"
#include <unordered_map> // bibliothèque pour unordered_map qui permet de stocker les labyrinthes "Prime"

using namespace std;
unordered_map<string, Labyrinthe*> transitionsTNT; // map clé-valeur pour stocker les labyrinthes "Prime"
vector<char> objetRecoltes;
mutex mutexObjets;// Mutex pour protéger l'accès à la liste des objets récoltés

/**
 * @brief Remplit un vecteur avec les directions possibles à partir d'une position donnée dans le labyrinthe.
 * 
 * @param labyrinthe 
 * @param x 
 * @param y 
 * @return vector<char> 
 */
vector<char> remplirVecteurDesDirectionsPossibles(Labyrinthe& labyrinthe, int x, int y) {
    vector<char> v;
    if (labyrinthe.estJouable(x - 1, y) && labyrinthe.getCase(x - 1, y) != '*') v.push_back('H');
    if (labyrinthe.estJouable(x + 1, y) && labyrinthe.getCase(x + 1, y) != '*') v.push_back('B');
    if (labyrinthe.estJouable(x, y - 1) && labyrinthe.getCase(x, y - 1) != '*') v.push_back('G');
    if (labyrinthe.estJouable(x, y + 1) && labyrinthe.getCase(x, y + 1) != '*') v.push_back('D');
    return v;
}

/**
 * @brief Fonction de backtracking pour résoudre le labyrinthe.
 * 
 * @param labyrinthe 
 * @param x 
 * @param y 
 * @param chemin 
 * @param transitionsTNT 
 * @return true 
 * @return false 
 * 
 */
bool backtracking(Labyrinthe& labyrinthe, int x, int y, string& chemin, unordered_map<string, Labyrinthe*>* transitionsTNT){
    if (x == get<0>(labyrinthe.getSortie()) && y == get<1>(labyrinthe.getSortie())) {
        const vector<char>& objetsARecuperer = labyrinthe.getObjetsARecuperer();

        // Vérifier si tous les objets nécessaires ont été collectés
        if (all_of(objetsARecuperer.begin(), objetsARecuperer.end(), [](char obj) {
            lock_guard<mutex> lock(mutexObjets);
            return find(objetRecoltes.begin(), objetRecoltes.end(), obj) != objetRecoltes.end();
        })) {
            cout << "Tous les objets necessaires ont ete collectes. Sortie trouvee a la position : [" << x << "," << y << "]" << endl;
            return true;
        }
    }

    //Vérifier si la case est un mur ou un monstre
    if (labyrinthe.getCase(x, y) == '*' || labyrinthe.getCase(x, y) == '#') {
        return false;
    }

    char caseOriginale = labyrinthe.getCase(x, y);
    bool objetCollecte = false;
    // Vérifier si la case est un objet à récolter
    if (isalpha(caseOriginale) && caseOriginale != 'M' && caseOriginale != 'A' && caseOriginale != 'D' && caseOriginale != 'T') {
        lock_guard<mutex> lock(mutexObjets);
        if (find(objetRecoltes.begin(), objetRecoltes.end(), caseOriginale) == objetRecoltes.end()) {
            objetRecoltes.push_back(caseOriginale);
            objetCollecte = true;
        }
    }

    labyrinthe.marquerCase(x, y, '*');
    vector<char> directions = remplirVecteurDesDirectionsPossibles(labyrinthe, x, y);

    for (char direction : directions) {
        int newX = x, newY = y;
        switch (direction) {
            case 'H': newX--; chemin += 'H'; break;
            case 'B': newX++; chemin += 'B'; break;
            case 'G': newY--; chemin += 'G'; break;
            case 'D': newY++; chemin += 'D'; break;
        }

        // On est sur un tnt on passe a la version "Prime"
        if (labyrinthe.getCase(newX, newY) == 'T' && transitionsTNT) {
            Labyrinthe* nextPrime = (*transitionsTNT)[labyrinthe.getNom()];
            if (nextPrime) {
                const vector<char>& objetsARecuperer = labyrinthe.getObjetsARecuperer();
                if (all_of(objetsARecuperer.begin(), objetsARecuperer.end(), [](char obj) {
                    lock_guard<mutex> lock(mutexObjets);
                    return find(objetRecoltes.begin(), objetRecoltes.end(), obj) != objetRecoltes.end();
                })) {
                    nextPrime->copierCasesVisitees(labyrinthe);
                    if (backtracking(*nextPrime, newX, newY, chemin, transitionsTNT)) {
                        return true;
                    }
                }
            }
        }
        if (backtracking(labyrinthe, newX, newY, chemin, transitionsTNT)) return true;
        chemin.pop_back();
    }
    // On retourne en arriere donc on remet la case d'origine
    // et on retire l'objet de la liste des objets récoltés si c'était un objet
    labyrinthe.marquerCase(x, y, caseOriginale);
    if (objetCollecte) {
        lock_guard<mutex> lock(mutexObjets);
        objetRecoltes.erase(remove(objetRecoltes.begin(), objetRecoltes.end(), caseOriginale), objetRecoltes.end());
    }
    return false;
}


/**
 * @brief Fonction de résolution séquentielle des labyrinthes.
 * 
 * @param labyrinthes 
 */
void resolutionSequentielle(vector<Labyrinthe>& labyrinthes) {
    cout << "Execution de la methode sequentielle..." << endl;
    for (Labyrinthe& labyrinthe : labyrinthes) {
        if (labyrinthe.getNom().find("Prime") != string::npos) continue;
        cout << "Resolution du labyrinthe " << labyrinthe.getNom() << " :" << endl;
        int x = get<0>(labyrinthe.getEntree());
        int y = get<1>(labyrinthe.getEntree());
        string chemin = "";
        if (!backtracking(labyrinthe, x, y, chemin, &transitionsTNT)) {
            cout << "Aucun chemin trouve." << endl;
        }
    }
}

/**
 * @brief Fonction de résolution avec un thread par labyrinthe.
 * 
 * @param labyrinthes 
 * @param transitionsTNT 
 */
void resolutionThreadParLabyrinthe(vector<Labyrinthe>& labyrinthes, unordered_map<string, Labyrinthe*>& transitionsTNT) {
    cout << "Execution de la methode avec un thread par labyrinthe..." << endl;
    vector<thread> threads;

    for (Labyrinthe& labyrinthe : labyrinthes) {
        if (labyrinthe.getNom().find("Prime") != string::npos) continue;

        // Créer un thread pour chaque labyrinthe
        threads.emplace_back([&labyrinthe, &transitionsTNT]() {
            int x = get<0>(labyrinthe.getEntree());
            int y = get<1>(labyrinthe.getEntree());
            string chemin = "";

            Labyrinthe* labyrinthePrime = nullptr;
            // Vérifier si le labyrinthe a une version "Prime"
            auto it = transitionsTNT.find(labyrinthe.getNom());
            if (it != transitionsTNT.end()) {
                labyrinthePrime = it->second;
            }
            // Vérifier si le labyrinthe est "Prime" si oui on lance le backtracking sur le labyrinthe "Prime"
            if (labyrinthePrime) {
                const vector<char>& objetsARecuperer = labyrinthe.getObjetsARecuperer();
                if (all_of(objetsARecuperer.begin(), objetsARecuperer.end(), [](char obj) {
                    lock_guard<mutex> lock(mutexObjets);
                    return find(objetRecoltes.begin(), objetRecoltes.end(), obj) != objetRecoltes.end();
                })) {
                    labyrinthePrime->copierCasesVisitees(labyrinthe);
                    backtracking(*labyrinthePrime, x, y, chemin, &transitionsTNT);
                    return;
                }
            }
            // Lancer le backtracking sur le labyrinthe courant
            backtracking(labyrinthe, x, y, chemin, &transitionsTNT);
        });
    }
    // Attendre la fin de tous les threads

    for (auto& t : threads) t.join();
}

/**
 * @brief Fonction de résolution avec un thread par direction possible.
 * 
 * @param labyrinthe 
 * @param transitionsTNT 
 */
void resolutionThreadParDirection(Labyrinthe& labyrinthe, unordered_map<string, Labyrinthe*>& transitionsTNT) {
    cout << "Execution de la methode avec un thread par direction possible..." << endl;

    int x = get<0>(labyrinthe.getEntree());
    int y = get<1>(labyrinthe.getEntree());
    vector<char> directions = remplirVecteurDesDirectionsPossibles(labyrinthe, x, y);
    vector<thread> threads;
    for (char direction : directions) {
        // Créer un thread pour chaque direction possible
        threads.emplace_back([&, direction]() {
            int newX = x, newY = y;
            string chemin = "";

            switch (direction) {
                case 'H': newX--; chemin += 'H'; break;
                case 'B': newX++; chemin += 'B'; break;
                case 'G': newY--; chemin += 'G'; break;
                case 'D': newY++; chemin += 'D'; break;
            }

            Labyrinthe* labyrinthePrime = nullptr;
            auto it = transitionsTNT.find(labyrinthe.getNom());
            // Vérifier si le labyrinthe a une version "Prime"
            if (it != transitionsTNT.end()) {
                labyrinthePrime = it->second;
            }

            // Vérifier si le labyrinthe on passe à un labyrinthe "Prime"
            if (labyrinthe.getCase(newX, newY) == 'T' && labyrinthePrime) {
                const vector<char>& objetsARecuperer = labyrinthe.getObjetsARecuperer();
                if (all_of(objetsARecuperer.begin(), objetsARecuperer.end(), [](char obj) {
                    lock_guard<mutex> lock(mutexObjets);
                    return find(objetRecoltes.begin(), objetRecoltes.end(), obj) != objetRecoltes.end();
                })) {
                    cout << "LabyrinthePrime trouve et tous les objets collectes !" << endl;
                    labyrinthePrime->copierCasesVisitees(labyrinthe);
                    backtracking(*labyrinthePrime, newX, newY, chemin, nullptr);
                    return;
                }
            }
            // Lancer le backtracking sur le labyrinthe courant
            if (backtracking(labyrinthe, newX, newY, chemin, labyrinthePrime ? &transitionsTNT : nullptr)) {
                cout << "Sortie trouvee dans la direction " << direction << " !" << endl;
            }
        });
    }

    // Attendre la fin de tous les threads
    for (auto& t : threads) {
        t.join();
    }
}



int main() {
    string cheminFichier = "labyrinthe.txt";
    vector<Labyrinthe> labyrinthes = chargerLabyrinthes(cheminFichier);
    preparerTransitionsTNT(labyrinthes, transitionsTNT);

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
            for (Labyrinthe& lab : labyrinthes) {
                if (!(lab.getNom().size() >= 5 && lab.getNom().substr(lab.getNom().size() - 5) == "Prime")) {
                    resolutionThreadParDirection(lab, transitionsTNT);
                }
            }
            break;
        case 3:
            resolutionThreadParLabyrinthe(labyrinthes, transitionsTNT);
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
    cout << endl;

    for (Labyrinthe& lab : labyrinthes) {
        lab.AfficherLabyrintheAvecCheminEnVert();
    }
    return 0;
}