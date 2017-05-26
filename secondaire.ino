#include <Arduino.h>
#include <SimpleTimer.h>

// CONVENTIONS
// distances = mm

// CAPTEURS
#define PIN_TIRETTE 42
void attendreTirette() {
    while (digitalRead(PIN_TIRETTE) == HIGH) {
        continue;
    }
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
    Serial.println("Tourne à droite");

}

void tournerGauche() {
    Serial.println("Tourne à gauche");

}

// GÉNÉRALES

void fin() {
    Serial.println("Fin du timer");
    stop();
    // TODO Autres capteurs
}

int tempsEcoule = 0;
#define DUREE_JEU 5
SimpleTimer timer;
int secondI;

void seconde() {
    tempsEcoule++;
    Serial.print("Temps = ");
    Serial.println(tempsEcoule);
    if (tempsEcoule >= DUREE_JEU) {
        fin();
        timer.disable(secondI);
    }
}

void parcours() {

}


// SPÉCIFIQUES

void setup() {
    Serial.begin(9600);
    Serial.println("Setup");

    // Initialisation des broches

    Serial.println("Setup terminé. Attente de la tirette");

    attendreTirette();

    Serial.println("Tirette tirée. C'est parti !");

    secondI = timer.setInterval(1000, seconde);

    parcours();

}

void loop() {
    timer.run();
}
