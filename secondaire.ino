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

#define PIN_ULT_AV_TRIG 22
#define PIN_ULT_AV_ECHO 24
bool capteurAvant() {
    digitalWrite(PIN_ULT_AV_TRIG , HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_ULT_AV_TRIG, LOW);
    unsigned long lecture_echo = pulseIn(PIN_ULT_AV_ECHO, HIGH, 60);
    Serial.print("Capteur");
    Serial.println(lecture_echo);
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

// Motor One
#define enA 3
#define IN1 7
#define IN2 4

// Motor Two
#define enB 6
#define IN3 2
#define IN4 5

float x, y;
char sens; // [0-3], sens trigo

void stop() {
  digitalWrite (IN1, LOW);
  digitalWrite (IN2, LOW);
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, LOW);
  analogWrite (enA, 0);
  analogWrite (enB, 0);
}

int tempsRestant = 0;

#define PUISSANCE_RAPIDE 400
#define PUISSANCE_LENTE 100
// en mm/sec
#define VIT_RAPIDE 20
#define VIT_LENTE 0.03

void stepMouvement() {
    tempsRestant--;
    if (tempsRestant <= 0) {
        stop();
    }
}

// void avance(float vitesse) {
void avance() {
    digitalWrite (IN1, LOW);
    digitalWrite (IN2, HIGH);
    digitalWrite (IN3, LOW);
    digitalWrite (IN4, HIGH);
    analogWrite (enA, 90);
    analogWrite (enB, 60);
}

#define FREQ_ECH 5

void avancer(float dist) {
    Serial.print("On va avancer de ");
    Serial.println(dist);
    // Ou recule
    float temps = abs(VIT_RAPIDE * dist); // en seconde
    Serial.print("Temps ");
    Serial.println(temps);
    tempsRestant = temps * FREQ_ECH;
    bool marcheAvant = dist > 0;
    Timer3.initialize((int) (1E6 / FREQ_ECH));
    bool obstacle = false, lastObstacle = true;
    while (tempsRestant > 0) {
        // Si obstacle a changé (ou si c'est le premier tour vu les valeurs d'initialisation)
        // Serial.print("Restant ");
        // Serial.println(tempsRestant);
        if (obstacle != lastObstacle) {
            if (obstacle) {
                Serial.println("Obstacle !");
                stop();
                Timer3.detachInterrupt();
            } else {
                //avance(VIT_RAPIDE * (marcheAvant ? 1 : -1));
                avance();
                Timer3.attachInterrupt(stepMouvement);
            }
        }

        lastObstacle = obstacle;
        Serial.println("171");
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

#define PUISSANCE_ROTATION 200

void tourne(bool sens) {
    digitalWrite (IN1, sens ? HIGH : LOW);
    digitalWrite (IN2, sens ? LOW : HIGH);
    analogWrite (enA, 90);
    digitalWrite (IN3, sens ? LOW : HIGH);
    digitalWrite (IN4, sens ? HIGH : LOW);
    analogWrite (enB, 60);
}

// en secondes
#define TEMPS_ANGLE_DROIT 0.4

void tournerVraieDroite() {
    tempsRestant = TEMPS_ANGLE_DROIT * FREQ_ECH;
    Timer3.initialize(1E6 / FREQ_ECH);
    tourne(true);
    Timer3.attachInterrupt(stepMouvement);
    while (tempsRestant > 0) {
        continue;
    }
    Timer3.detachInterrupt();
}

void tournerVraieGauche() {
    tempsRestant = TEMPS_ANGLE_DROIT * FREQ_ECH;
    Timer3.initialize(1E6 / FREQ_ECH);
    tourne(true);
    Timer3.attachInterrupt(stepMouvement);
    while (tempsRestant > 0) {
        continue;
    }
    Timer3.detachInterrupt();
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
#define DUREE_JEU 90
int secondI;

void seconde() {
    tempsEcoule++;
    Serial.print("Temps = ");
    Serial.println(tempsEcoule);
    if (tempsEcoule >= DUREE_JEU) {
        fin();
        Timer1.detachInterrupt();
    }
}

void parcours() {
    avancer(1100 - 200);
    tournerDroite();
    avancer(700 - 200);
    tournerDroite();
    avancer(1100 - 1000);
    tournerDroite();
    avancer(700 - 0);
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
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
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

    /* parcours(); */

    // DEBUG
    avance();
    // tourne(true);
    delay(5000);
    stop();
    /* while(true) { */
    /*     capteurAvant(); */
    /* } */

}

void loop() {
}
