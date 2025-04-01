//Fichier qui répertorie les fonctions de créations et affichage des labyrinthes
#include <iostream>
#include <vector>
#include <thread>
#include "labyrinthe.cpp"

using namespace std;

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
            // Ligne vide ou non pertinente, ignorer
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

void afficherLabyrinthes(const vector<Labyrinthe>& labyrinthes) {
    for (Labyrinthe lab : labyrinthes) {
        lab.afficherNom(); 
        lab.afficherLabyrinthe();
        cout << endl;
    }
}

void afficherLabyrinthesAvecCheminEnVert(const vector<Labyrinthe>& labyrinthes) {
    for (Labyrinthe lab : labyrinthes) {
        lab.afficherNom(); 
        lab.AfficherLabyrintheAvecCheminEnVert();
        cout <<"------------------------"<< endl;
    }
}