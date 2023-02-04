int jin = 3;
int cnt = 0;

void setup() {
  Serial.begin(115200);
  pinMode(jin, INPUT);
}

void loop() {
  if(digitalRead(jin) == LOW) {
    cnt += 1;
    delay(500);
  }

  Serial.print(cnt);
}
