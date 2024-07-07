#include <Servo.h>

// Déclaration des broches
const int hallPin = 2;          // Pin du capteur à effet Hall
const int reflectionPin = 3;    // Pin du capteur de réflexion
const int MagnetPin = 5;        // Pin du capteur de l'aimant
const int servoPinKlappe = 8;   // Pin du servomoteur de la trappe
const int servoPinVereinzelung = 9; // Pin du servomoteur de la séparation
const int servoPinKurve = 10;   // Pin du servomoteur de la courbe
const int triggerPin = 11;      // Pin pour le trigger du contrôleur U9

Servo servoKlappe;  // Servomoteur pour la trappe

volatile bool hallDetected = false;
volatile bool reflectionDetected = false;
volatile unsigned long lastHallTime = 0;
volatile unsigned long lastReflectionTime = 0;

unsigned long rotationTime = 0;
unsigned long targetReleaseTime = 0;
const unsigned long ballFallTime = 361; // Temps en ms que met la bille pour tomber

void setup() {
  pinMode(hallPin, INPUT);
  pinMode(reflectionPin, INPUT);
  pinMode(MagnetPin, OUTPUT);
  pinMode(triggerPin, INPUT);

  servoKlappe.attach(servoPinKlappe);

  attachInterrupt(digitalPinToInterrupt(hallPin), hallInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(reflectionPin), reflectionInterrupt, RISING);
  
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(triggerPin) == HIGH) {
    openTrap();
    while (!hallDetected || !reflectionDetected) {
      // Attente de la détection par les deux capteurs
    }
    hallDetected = false;
    reflectionDetected = false;

    unsigned long currentTime = millis();
    unsigned long timeSinceLastHall = currentTime - lastHallTime;
    unsigned long timeSinceLastReflection = currentTime - lastReflectionTime;

    // Calculer le temps de rotation et le moment de relâcher la bille
    if (timeSinceLastHall < timeSinceLastReflection) {
      rotationTime = timeSinceLastHall;
    } else {
      rotationTime = timeSinceLastReflection;
    }

    targetReleaseTime = currentTime + (rotationTime / 2) - ballFallTime;

    while (millis() < targetReleaseTime) {
      // Attendre le bon moment pour relâcher la bille
    }

    releaseBall();
  }
}

void openTrap() {
  servoKlappe.write(90); // Ouvrir la trappe (ajuster l'angle selon votre configuration)
  delay(500);            // Attendre que la trappe soit complètement ouverte
}

void releaseBall() {
  digitalWrite(MagnetPin, LOW); // Relâcher la bille
  delay(500);                   // Attendre que la bille tombe
  digitalWrite(MagnetPin, HIGH); // Remettre l'aimant en position pour la prochaine bille
}

void hallInterrupt() {
  hallDetected = true;
  lastHallTime = millis();
}

void reflectionInterrupt() {
  reflectionDetected = true;
  lastReflectionTime = millis();
}
