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

#define PIN_COTE 42
bool bleu = true;
void lireCote() {
    bleu = digitalRead(PIN_COTE) == LOW;
}

#define SEUIL_OBSTACLE 120

#define PIN_ULT_AV_TRIG 42
#define PIN_ULT_AV_ECHO 42
bool capteurAvant() {
    digitalWrite(PIN_ULT_AV_TRIG , HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_ULT_AV_TRIG, LOW);
    unsigned long lecture_echo = pulseIn(PIN_ULT_AV_ECHO, HIGH, 60);
    float mm = lecture_echo / 580;
    return mm < SEUIL_OBSTACLE;
}

#define PIN_ULT_AR_TRIG 42
#define PIN_ULT_AR_ECHO 42
bool capteurArriere() {
    digitalWrite(PIN_ULT_AR_TRIG , HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_ULT_AR_TRIG, LOW);
    unsigned long lecture_echo = pulseIn(PIN_ULT_AR_ECHO, HIGH, 60);
    float mm = lecture_echo / 580;
    return mm < SEUIL_OBSTACLE;
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

float x, y;
char sens; // [0-3], sens trigo

void stop() {

}

bool enMouvement = false;
SimpleTimer timer;
int mouvTimeout;

// en mm / sec
#define VIT_RAPIDE 100
#define VIT_LENTE 25

void finMouvement() {
    stop();
    enMouvement = false;
}

void avance(float vitesse) {
    // TODO
}

void avancer(float dist) {
    // Ou recule
    int temps = abs(VIT_RAPIDE * dist);
    bool marcheAvant = dist > 0;
    avance(VIT_RAPIDE * (marcheAvant ? 1 : -1));
    mouvTimeout = timer.setTimeout(temps, finMouvement);
    enMouvement = true;
    bool obstacle;
    while (enMouvement) {
        Serial.println(104);
        if (marcheAvant) {
            obstacle = capteurAvant();
        } else {
            obstacle = capteurArriere();
        }
        if (obstacle) {
            timer.disable(mouvTimeout);
            stop();
        } else {
            timer.enable(mouvTimeout);
            avance(VIT_RAPIDE * (marcheAvant ? 1 : -1));
        }
    }
    stop();
    switch (sens) {
        case 0:
            x += dist;
            break;
        case 1:
            y += dist;
            break;
        case 2:
            x -= dist;
            break;
        case 3:
            y += dist;
            break;
    }
    Serial.print("Position : (");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.println(")");
}

void tournerVraieDroite() {
    // TODO
}

void tournerVraieGauche() {
    // TODO
}

void tournerDroite() {
    sens -= 1;
    sens = sens % 4;
    Serial.println("Tourne à droite");
    if (bleu) {
        tournerVraieDroite();
    } else {
        tournerVraieGauche();
    }
}

void tournerGauche() {
    sens += 1;
    sens = sens % 4;
    Serial.println("Tourne à gauche");
    if (bleu) {
        tournerVraieGauche();
    } else {
        tournerVraieDroite();
    }
}

// GÉNÉRALES

void fin() {
    Serial.println("Fin du timer");
    stop();
    // TODO Autres capteurs
}

int tempsEcoule = 0;
#define DUREE_JEU 5
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
    avancer(800 - x);
    tournerDroite();
    avancer(500 - y);
}


// SPÉCIFIQUES

void setup() {
    Serial.begin(9600);
    Serial.println("Initialisation");

    pinMode(PIN_TIRETTE, INPUT);
    pinMode(PIN_COTE, INPUT);

    Serial.println("Setup terminé. Attente de la tirette");

    attendreTirette();

    Serial.println("Tirette tirée. C'est parti !");

    secondI = timer.setInterval(1000, seconde);

    lireCote();

    Serial.print("On est du côté ");
    Serial.println(bleu ? "bleu" : "jaune");

    x = 200;
    y = 200;
    sens = 0;

    // parcours();

}

void loop() {
    timer.run();
}
