#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
using namespace std;

class Labyrinthe {
private:
    vector<int> PositionEntree; // Position de l'entrée
    char grille[20][20];
    string nom; // Nom du labyrinthe
    vector<tuple<int, int>> objets; // Positions des objets (lettres autres que M)
    tuple<int, int> entree; // Position de l'entrée (plus petit chiffre)
    tuple<int, int> sortie; // Position de la sortie (plus grand chiffre ou 'A')
public:

    void afficherLabyrinthe() {
        cout << "Labyrinthe : " << nom << endl;
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                cout << grille[i][j] << " ";
            }
            cout << endl;
        }
        cout<< "La sortie se trouve en : [" << get<0>(sortie) << "," << get<1>(sortie)<<"]" << endl;
        cout<< "L'entree se trouve en : [" << get<0>(entree) << "," << get<1>(entree)<<"]" << endl;
        cout << "Les objets se trouvent aux positions : " << endl;
        for (const tuple<int,int> obj : objets) {
            cout << "Objet a la position : " << get<0>(obj) << "," << get<1>(obj) << endl;
        }
    }

    Labyrinthe() {
        // Initialiser la grille avec des espaces vides
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                grille[i][j] = ' ';
            }
        }
    }

    Labyrinthe(const vector<string>& lignes, const string& nomLabyrinthe) {
        int minChiffre = 10;
        int maxChiffre = -1; 
        bool sortieTrouvee = false;
        bool entreeTrouvee = false;
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                grille[i][j] = lignes[i][j];

                if (isalpha(grille[i][j]) && grille[i][j] != 'M' && grille[i][j] != 'A' && grille[i][j] != 'D' && grille[i][j] != 'T') {
                    objets.push_back(make_tuple(i, j));
                }

                // Vérifier si c'est un chiffre pour déterminer l'entrée et la sortie
                if (isdigit(grille[i][j])) {
                    int chiffre = grille[i][j] - '0'; // Convertir le caractère en entier
                    if (chiffre < minChiffre && !entreeTrouvee) {
                        minChiffre = chiffre;
                        entree = make_tuple(i, j); // Mettre à jour l'entrée
                    }
                    if (chiffre > maxChiffre) {
                        maxChiffre = chiffre;
                        sortie = make_tuple(i, j); // Mettre à jour la sortie
                        sortieTrouvee = true;
                    }
                }
                // Vérifier si c'est la lettre 'D' pour le départ
                if (grille[i][j] == 'D') {
                    entree = make_tuple(i, j);
                    entreeTrouvee = true;
                }
                // Vérifier si c'est la lettre 'A' pour la sortie
                if (grille[i][j] == 'A') {
                    sortie = make_tuple(i, j);
                    sortieTrouvee = true;
                }
            }
        }

        // Si aucune entrée ou sortie n'a été trouvée, lever une erreur
        if (minChiffre == 10&& !entreeTrouvee) {
            throw runtime_error("Aucune entrée trouvee dans le labyrinthe.");
        }
        if (!sortieTrouvee) {
            throw runtime_error("Aucune sortie trouvee dans le labyrinthe.");
        }

        nom = nomLabyrinthe; // Définir le nom du labyrinthe
    }

    // Méthode pour définir le nom
    void setNom(const string& s) {
        nom = s;
    }

    // Méthode pour afficher le nom (facultatif)
    void afficherNom() const {
        cout << "Nom du labyrinthe : " << nom << endl;
    }

    tuple<int,int> getEntree() const {
        return entree;
    }

    tuple<int,int> getSortie() const {
        return sortie;
    }
    vector<tuple<int,int>> getObjets() const {
        return objets;
    }

    char getCase(int x, int y) const {
        return grille[x][y];
    }

    bool estJouable(int x, int y){
        // Vérifier si la position est dans les limites du labyrinthe
        if (x < 0 || x >= 20 || y < 0 || y >= 20) {
            return false;
        }
        // Vérifier si la case n'est pas un mur
        if (grille[x][y] == '#') {
            return false;
        }
        // Vérifier si la case n'est pas un monstre
        if (grille[x][y] == 'M') {
            return false;
        }
        if( grille[x][y] == '*') {
            return false; 
        }
        return true;
    }
    void marquerCase(int x, int y, char symbole) {
        grille[x][y] = symbole;
    }
    void AfficherLabyrintheAvecCheminEnVert() {
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                if (grille[i][j] == '*') {
                    cout << "\033[32m" << grille[i][j] << "\033[0m "; // Afficher en vert
                } else {
                    cout << grille[i][j] << " ";
                }
            }
            cout << endl;
        }
    }
    void copierCasesVisitees(const Labyrinthe& autreLabyrinthe) {
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                if (autreLabyrinthe.grille[i][j] == '*') {
                    grille[i][j] = '*'; // Copier les cases visitées
                }
            }
        }
    }
    string getNom() const {
        return nom;
    }
};