#include <iostream>
#include <vector>
using namespace std;

class Curseur{
    private :
        int x;
        int y;
        vector<char> objets; // représente les objets ramassé par le joueur
    public : 
        Curseur(){
            x = 0;
            y = 0;
        }
        Curseur(int x, int y){
            this->x = x;
            this->y = y;
        }
        void setPosition(int x, int y){
            this->x = x;
            this->y = y;
        }
        void allerEnHaut(){
            y--;
        }
        void allerEnBas(){
            y++;
        }
        void allerAGauche(){
            x--;
        }
        void allerADroite(){
            x++;
        }
        int getX(){
            return x;
        }
        int getY(){
            return y;
        }
        void rajouterObjet(char objet){
            objets.push_back(objet);
        }
        void afficherObjets(){
            cout << "Objets ramassés : ";
            for (char objet : objets) {
                cout << objet << " ";
            }
            cout << endl;
        }
        void deplacerCurseur(char direction){
            switch (direction)
            {
            case 'H': // Haut
            allerEnHaut();
            break;
            case 'B': // Bas
            allerEnBas();
            break;
            case 'G': // Gauche
            allerAGauche();
            break;
            case 'D': // Droite
            allerADroite();
            break;
            default:
            cout << "Direction invalide !" << endl;
            break;
            }
        }
};