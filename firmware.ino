
// includes
#include <Servo.h>

// pin defs
#define TRIG_PIN 12
#define ECHO_PIN 13
#define SERVO_PIN 4

// constants 
#define SAMPLE_C 15
#define DISTANCE_THRESH_CM 10
#define FINAL_ROT_D 150

// function defs
int getSonarDistance();
int getSonarRaw();
int filterSampleNoise(int arr[SAMPLE_C]);
int movingAverage(int arr[SAMPLE_C]);

// objects
Servo servo;

// dynamic variables
int d = 0, pos = 0, goal_pos = 0;
bool closed = false, statesw = false;

int dsamples[SAMPLE_C];

void setup() {
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  servo.attach(SERVO_PIN);

  closed = false;
}

void loop() {
  d = getSonarDistance();

  if (d < DISTANCE_THRESH_CM && !statesw) {
    closed = !closed;
    statesw = true;
  }
  if (d > DISTANCE_THRESH_CM) {
    statesw = false;
  }

  pos = closed ? FINAL_ROT_D : 0;

  servo.write(pos);
}

int getSonarDistance() {
    for (int i = 0; i < SAMPLE_C; i++) {
      dsamples[i] = getSonarRaw();
      delay(5);
    }
    return filterSampleNoise(dsamples);
  }
  
  int getSonarRaw() {
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(TRIG_PIN, LOW);
  
    long duration = pulseIn(ECHO_PIN, HIGH, 20000);
    return duration * 0.034 / 2; // cm
  }
  
  int filterSampleNoise(int arr[SAMPLE_C]) { // for high SAMPLE_C (>=15), otherwise worthless
    for (int i = 1; i < SAMPLE_C; i++) {
      int key = arr[i];
      int j = i - 1;
      while (j >= 0 && arr[j] > key) {
          arr[j + 1] = arr[j];
          j--;
      }
      arr[j + 1] = key;
    }
  
  
    int std = SAMPLE_C / 3; 
    float sum = 0.0, c = 0.0;
    for (int i = std; i < SAMPLE_C - std; i++) { // eliminate highest and lowest std from average calculations 
      sum += arr[i];
      c += 1.0;
    }
    return (int)((sum / c) + 0.5); // cheap rounding
  }
  