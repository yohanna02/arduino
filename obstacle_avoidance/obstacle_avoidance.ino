#define TRIG_ONE 11
#define ECHO_ONE 10

#define TRIG_TWO A2
#define ECHO_TWO A1

#define RELAY_1 9
#define RELAY_2 6

#define RELAY_3 5
#define RELAY_4 3



void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG_ONE, OUTPUT);
  pinMode(ECHO_ONE, INPUT);
  pinMode(TRIG_TWO, OUTPUT);
  pinMode(ECHO_TWO, INPUT);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);

  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);

  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // int distance_one = getDistance(TRIG_ONE, ECHO_ONE);
  int distance_two = getDistance(TRIG_TWO, ECHO_TWO);

  // Serial.println(distance_one);
  // delay(1000);


  if (/* distance_one < 10 || */ distance_two > 3 && distance_two < 10) {
    digitalWrite(RELAY_1, LOW);
    digitalWrite(RELAY_2, LOW);
    digitalWrite(RELAY_3, LOW);
    digitalWrite(RELAY_4, LOW);
    // Serial.println(F("Stopped"));
    while (/* distance_one < 10 || */ distance_two > 3 && distance_two < 10) {
      // distance_one = getDistance(TRIG_ONE, ECHO_ONE);
      distance_two = getDistance(TRIG_TWO, ECHO_TWO);
    }
    delay(3000);
  }

  // Serial.println(F("Moving"));
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, LOW);

  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, LOW);
}

int getDistance(int trigPin, int echoPin) {
  long duration;
  int distance;

  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond pulse to trigPin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin and calculate the distance
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert time to distance (cm)

  return distance;  // Return the distance value
}