//Fichier qui répertorie les fonctions de créations et affichage des labyrinthes
#include <iostream>
#include <vector>
#include <unordered_map>
#include "labyrinthe.cpp"

using namespace std;

/**
 * @brief Fonction qui charge les labyrinthes à partir d'un fichier texte.
 * 
 * @param cheminFichier 
 * @return vector<Labyrinthe> 
 */
vector<Labyrinthe> chargerLabyrinthes(const string& cheminFichier) {
    vector<Labyrinthe> labyrinthes;
    ifstream fichier(cheminFichier);
    if (!fichier) {
        cout << "Erreur lors de l'ouverture du fichier." << endl;
        return labyrinthes;
    }

    string ligne;
    vector<string> labyrintheCourant;
    vector<string> labyrinthePrime;
    int compteur = 1; // Compteur pour nommer les labyrinthes

    while (getline(fichier, ligne)) {
        if (ligne.find('#') == string::npos) {
            continue;
        }

        // Ajouter la ligne au labyrinthe courant
        labyrintheCourant.push_back(ligne.substr(0, 20)); // Prendre les 20 premiers caractères

        // Vérifier la version "prime" (2 caractères à droite)
        if (ligne.size() > 22) {
            string lignePrime = "#" + ligne.substr(22, 19); // Ajouter un mur de gauche et prendre 19 caractères
            labyrinthePrime.push_back(lignePrime);
        }

        // Si 20 lignes sont atteintes, ajouter les labyrinthes
        if (labyrintheCourant.size() == 20) {
            labyrinthes.push_back(Labyrinthe(labyrintheCourant, "Labyrinthe" + to_string(compteur)));
            labyrintheCourant.clear();
            compteur++;

            // Ajouter la version "prime" si elle existe
            if (!labyrinthePrime.empty()) {
                labyrinthes.push_back(Labyrinthe(labyrinthePrime, "Labyrinthe" + to_string(compteur-1) + "Prime"));
                labyrinthePrime.clear();
            }
        }
    }

    // Ajouter le dernier labyrinthe s'il est complet
    if (labyrintheCourant.size() == 20) {
        labyrinthes.push_back(Labyrinthe(labyrintheCourant, "Labyrinthe" + to_string(compteur)));
    }

    fichier.close();
    return labyrinthes;
}

/**
 * @brief Fonction qui affiche les labyrinthes.
 * 
 * @param labyrinthes 
 */
void afficherLabyrinthes(const vector<Labyrinthe>& labyrinthes) {
    for (Labyrinthe lab : labyrinthes) {
        lab.afficherNom(); 
        lab.afficherLabyrinthe();
        cout << endl;
    }
}

/**
 * @brief Fonction qui affiche les labyrinthes avec le chemin en vert.
 * 
 * @param labyrinthes 
 */
void afficherLabyrinthesAvecCheminEnVert(const vector<Labyrinthe>& labyrinthes) {
    for (Labyrinthe lab : labyrinthes) {
        lab.afficherNom(); 
        lab.AfficherLabyrintheAvecCheminEnVert();
        cout <<"------------------------"<< endl;
    }
}


/**
 * @brief Prépare les transitions entre labyrinthes "Prime" et leurs versions originales.
 * 
 * @param labyrinthes 
 */
void preparerTransitionsTNT(vector<Labyrinthe>& labyrinthes, unordered_map<string, Labyrinthe*>& transitionsTNT) {
    transitionsTNT.clear();
    for (Labyrinthe& lab : labyrinthes) {
        const string& nom = lab.getNom();
        if (nom.size() >= 5 && nom.substr(nom.size() - 5) == "Prime") {
            string originalName = nom.substr(0, nom.size() - 5);
            transitionsTNT[originalName] = &lab;
        }
    }
}
