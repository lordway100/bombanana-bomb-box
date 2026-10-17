// 봄바나나 폭탄 박스
// 불빛(LED) 색에 따라 정답 선이 바뀌는 선 자르기 게임

// ===== 설정 =====
const int LED_PINS[] = {9, 10, 11};   // 빨강, 노랑, 초록 LED
const char* LIGHT_NAMES[] = {"빨간불", "노란불", "초록불"};

const int WIRE_PINS[] = {2, 3, 4};
const char* WIRE_NAMES[] = {"빨강", "파랑", "노랑"};
const int WIRE_COUNT = 3;

// 불빛 → 잘라야 할 선 번호 (0=빨강선, 1=파랑선, 2=노랑선)
const int RULE[] = {2, 0, 1};   // 빨간불→노랑선, 노란불→빨강선, 초록불→파랑선

// ===== 상태 =====
bool wasCut[WIRE_COUNT];
bool armed = false;
bool gameOver = false;
int correctWire = -1;

void allOff() {
  for (int i = 0; i < 3; i++) digitalWrite(LED_PINS[i], LOW);
}

void lightOn(int i) {
  allOff();
  digitalWrite(LED_PINS[i], HIGH);
}

bool allConnected() {
  for (int i = 0; i < WIRE_COUNT; i++) {
    if (digitalRead(WIRE_PINS[i]) == HIGH) return false;
  }
  return true;
}

void setup() {
  for (int i = 0; i < WIRE_COUNT; i++) {
    pinMode(WIRE_PINS[i], INPUT_PULLUP);
    wasCut[i] = false;
  }
  for (int i = 0; i < 3; i++) pinMode(LED_PINS[i], OUTPUT);
  allOff();

  for (int i = 0; i < 3; i++) { lightOn(i); delay(300); }  // LED 점검
  allOff();

  randomSeed(analogRead(A5));   // A5는 비워두기
  Serial.begin(9600);
  Serial.println("=== 폭탄 박스 시작 ===");
  Serial.println("선을 모두 꽂아주세요...");
}

void loop() {
  if (gameOver) return;

  // 1단계: 선이 전부 꽂힐 때까지 대기
  if (!armed) {
    for (int i = 0; i < WIRE_COUNT; i++) {
      Serial.print(WIRE_NAMES[i]);
      Serial.print(digitalRead(WIRE_PINS[i]) == LOW ? ":연결  " : ":끊김  ");
    }
    Serial.println();

    if (allConnected()) {
      armed = true;
      int light = random(3);
      correctWire = RULE[light];
      lightOn(light);
      Serial.println("폭탄 작동 시작!");
      Serial.print("불빛: ");
      Serial.println(LIGHT_NAMES[light]);
    }
    delay(500);
    return;
  }

  // 2단계: 잘린 선 감지
  for (int i = 0; i < WIRE_COUNT; i++) {
    if (digitalRead(WIRE_PINS[i]) == HIGH && !wasCut[i]) {
      wasCut[i] = true;
      Serial.print(WIRE_NAMES[i]);
      Serial.println(" 선이 잘렸다!");

      if (i == correctWire) {
        Serial.println("해제 성공!");
      } else {
        Serial.println("펑! 폭발!");
      }
      gameOver = true;
    }
  }
  delay(50);
}
