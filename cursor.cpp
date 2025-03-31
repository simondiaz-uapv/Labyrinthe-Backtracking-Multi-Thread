#include <iostream>

using namespace std;

class Curseur{
    private :
        int x;
        int y;
    public : 
        Curseur(){
            x = 0;
            y = 0;
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
};