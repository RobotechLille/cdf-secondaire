#include <SimpleTimer.h>

// CONVENTIONS
// distances = mm

// CAPTEURS
void attendreTirette() {

}

bool capteurAvant() {

}

bool capteurArriere() {

}

bool capteurBoule() {

}

// ACTIONNEURS

void ouvrirPontLevis() {

}

void fermerPontLevis() {

}

void leverBras() {

}

void baisserBras() {
    // Ça veut pas dire qu'il faut baisser les bras ! \o/

}


// DIRECTION

void stop() {

}

void avance(float dist) {
    // Ou recule

}

void tournerDroite() {

}

void tournerGauche() {

}

// GÉNÉRALES

void fin() {
    Serial.println("Fin du timer");
    stop();
    // TODO Autres capteurs
}

void setup() {
    Serial.begin(9600);
    Serial.println("Setup");

    // Initialisation des broches

    Serial.println("Initialisation des broches terminé");

}

void loop() {

}
