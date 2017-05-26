#include <Arduino.h>
#include <TimerOne.h>
#include <TimerThree.h>
#include <Servo.h>

// CONVENTIONS
// distances = mm
// ne pas utiliser les broches 9, 10, 11 et 12 (utilisées par les timers)

// CAPTEURS
#define PIN_TIRETTE 31
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

#define SEUIL_BOULE 120

#define PIN_ULT_BL_TRIG 42
#define PIN_ULT_BL_ECHO 42
bool capteurBoule() {
    digitalWrite(PIN_ULT_BL_TRIG , HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_ULT_BL_TRIG, LOW);
    unsigned long lecture_echo = pulseIn(PIN_ULT_BL_ECHO, HIGH, 60);
    float mm = lecture_echo / 580;
    return mm < SEUIL_BOULE;
}

// ACTIONNEURS

Servo pont;
#define PIN_PONT A0

#define PONT_OUVERT 45
void ouvrirPont() {
    Serial.println("Ouverture du pont");
    pont.write(PONT_OUVERT);
}

#define PONT_FERME 135
void fermerPont() {
    Serial.println("Fermeture du pont");
    pont.write(PONT_FERME);
}

void leverBras() {
    Serial.println("Élévation du bras");

}

void baisserBras() {
    // Ça veut pas dire qu'il faut baisser les bras ! \o/
    Serial.println("Baissage du bras");

}


// DIRECTION

float x, y;
char sens; // [0-3], sens trigo

void stop() {

}

int tempsRestant = 0;

// en mm/sec
#define VIT_RAPIDE 0.1
#define VIT_LENTE 0.03

void stepMouvement() {
    tempsRestant--;
    if (tempsRestant <= 0) {
        stop();
    }
}

void avance(float vitesse) {
    // TODO
}

#define FREQ_ECH 100

void avancer(float dist) {
    Serial.print("On va avancer de ");
    Serial.println(dist);
    // Ou recule
    float temps = abs(VIT_RAPIDE * dist); // en seconde
    tempsRestant = temps * FREQ_ECH;
    bool marcheAvant = dist > 0;
    Timer3.initialize(1E6 / FREQ_ECH);
    bool obstacle = false, lastObstacle = true;
    while (tempsRestant > 0) {
        // Si obstacle a changé (ou si c'est le premier tour vu les valeurs d'initialisation)
        if (obstacle != lastObstacle) {
            if (obstacle) {
                stop();
                Timer3.detachInterrupt();
            } else {
                avance(VIT_RAPIDE * (marcheAvant ? 1 : -1));
                Timer3.attachInterrupt(stepMouvement);
            }
        }

        if (marcheAvant) {
            obstacle = capteurAvant();
        } else {
            obstacle = capteurArriere();
        }
    }
    Timer3.detachInterrupt();
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
    if (tempsEcoule == DUREE_JEU) {
        fin();
        Timer1.detachInterrupt();
    }
}

void parcours() {
    avancer(800 - x);
    tournerDroite();
    avancer(500 - y);
    /* ouvrirPont(); */
}


// SPÉCIFIQUES

void setup() {
    Serial.begin(9600);
    Serial.println("Initialisation");

    pinMode(PIN_TIRETTE, INPUT);
    pinMode(PIN_COTE, INPUT);
    pinMode(PIN_ULT_AV_TRIG, OUTPUT);
    pinMode(PIN_ULT_AV_ECHO, INPUT);
    pinMode(PIN_ULT_AR_TRIG, OUTPUT);
    pinMode(PIN_ULT_AR_ECHO, INPUT);
    pinMode(PIN_ULT_BL_TRIG, OUTPUT);
    pinMode(PIN_ULT_BL_ECHO, INPUT);
    pont.attach(PIN_PONT);

    fermerPont();
    baisserBras();

    Serial.println("Initialisation terminée. Attente de la tirette");

    attendreTirette();

    Serial.println("Tirette tirée. C'est parti !");

    Timer1.initialize(); // Initialise timer1 à 1 seconde
    Timer1.attachInterrupt(seconde);

    lireCote();

    Serial.print("On est du côté ");
    Serial.println(bleu ? "bleu" : "jaune");

    x = 200;
    y = 200;
    sens = 0;

    parcours();

}

void loop() {
}
