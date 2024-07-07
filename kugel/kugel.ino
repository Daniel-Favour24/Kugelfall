#include <Servo.h>

// Définir les broches
const int hallPin = 2;
const int reflectionPin = 3;
const int MagnetPin = 5;
const int servoPinKlappe = 8;
const int servoPinVereinzelung = 9;
const int servoPinKurve = 10;
const int triggerPin = 11;

// Définir les servos
Servo servoKlappe;
Servo servoVereinzelung;
Servo servoKurve;

// Définir le temps de chute de la bille
const long fallTime = 361; // en millisecondes

// Variables pour mesurer la vitesse de rotation
unsigned long previousHallEffectTime = 0;
float rotationSpeed = 0.0; // en rotations par milliseconde

// Variable pour suivre si la bille est prête à être relâchée
bool isBallReady = false;

// Variable pour suivre le segment actuel
int currentSegment = 0;

void setup() {
  // Initialiser les broches
  pinMode(hallPin, INPUT);
  pinMode(reflectionPin, INPUT);
  pinMode(MagnetPin, OUTPUT);
  pinMode(triggerPin, INPUT);

  // Initialiser les servos
  servoKlappe.attach(servoPinKlappe);
  servoVereinzelung.attach(servoPinVereinzelung);
  servoKurve.attach(servoPinKurve);

  // Activer le magnétisme
  digitalWrite(MagnetPin, HIGH);

  // Initialiser la communication série
  Serial.begin(9600);
}

void loop() {
  // Mesurer la vitesse de rotation
  if (digitalRead(hallPin) == HIGH) {
    unsigned long currentTime = millis();
    unsigned long timeDifference = currentTime - previousHallEffectTime;
    rotationSpeed = 1.0 / timeDifference; // Calculer la vitesse de rotation en rotations par milliseconde
    previousHallEffectTime = currentTime;
  }

  // Détecter le segment actuel
  if (digitalRead(reflectionPin) == HIGH) {
    currentSegment = (currentSegment + 1) % 12;
  }

  // Attendre le signal du déclencheur
  if (digitalRead(triggerPin) == HIGH) {
    // Charger la bille
    LadenKugel();

    // Indiquer que la bille est prête à être relâchée
    isBallReady = true;
  }

  // Si la bille est prête à être relâchée
  if (isBallReady) {
    // Calculer le délai avant de lâcher la bille
    long releaseDelay = (1.0 / rotationSpeed) - fallTime; // en millisecondes

    // Si le délai est négatif
    if (releaseDelay < 0) {
      // Afficher un message d'erreur
      Serial.println("Erreur : Le délai est négatif. Veuillez essayer à nouveau.");
    } else {
      // Si le capteur de réflexion détecte un segment blanc et que c'est le bon segment
      if (digitalRead(reflectionPin) == HIGH && currentSegment == 0) {
        // Attendre le moment parfait pour lâcher la bille
        delay(releaseDelay);

        // Lâcher la bille
        kugelFreigeben();
      }
    }

    // Réinitialiser l'état de la bille
    isBallReady = false;
  }
}

void kugelFreigeben() {
  digitalWrite(MagnetPin, LOW);
  Serial.print(millis());
  Serial.println(" Aimant désactivé");
  delay(250);
  servoKlappe.write(150);  // Fermer la trappe
  Serial.println("Bille libérée !");
}

void LadenKugel() {
  servoVereinzelung.write(0);
  servoKurve.write(0);
  delay(500);
  servoVereinzelung.write(150);
  servoKurve.write(150);
  Serial.println("Bille chargée et prête à être libérée");
}
