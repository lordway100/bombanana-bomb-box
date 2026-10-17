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

const unsigned long RESTART_HOLD_MS = 1000;  // 선을 다 꽂고 이만큼 지나면 새 게임

// ===== 상태 =====
bool wasCut[WIRE_COUNT];
bool armed = false;
bool gameOver = false;
int correctWire = -1;
unsigned long reconnectedSince = 0;

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

void newGame() {
  for (int i = 0; i < WIRE_COUNT; i++) wasCut[i] = false;
  armed = false;
  gameOver = false;
  correctWire = -1;
  reconnectedSince = 0;

  for (int i = 0; i < 3; i++) { lightOn(i); delay(300); }  // LED 점검
  allOff();

  Serial.println();
  Serial.println("=== 새 게임 ===");
  Serial.println("선을 모두 꽂아주세요...");
}

void setup() {
  for (int i = 0; i < WIRE_COUNT; i++) pinMode(WIRE_PINS[i], INPUT_PULLUP);
  for (int i = 0; i < 3; i++) pinMode(LED_PINS[i], OUTPUT);
  Serial.begin(9600);
  newGame();
}

void loop() {
  // 게임이 끝난 뒤: 선을 다 다시 꽂고 1초 지나면 새 게임
  if (gameOver) {
    if (allConnected()) {
      if (reconnectedSince == 0) reconnectedSince = millis();
      if (millis() - reconnectedSince >= RESTART_HOLD_MS) newGame();
    } else {
      reconnectedSince = 0;
    }
    delay(50);
    return;
  }

  // 1단계: 선이 전부 꽂힐 때까지 대기
  if (!armed) {
    for (int i = 0; i < WIRE_COUNT; i++) {
      Serial.print(WIRE_NAMES[i]);
      Serial.print(digitalRead(WIRE_PINS[i]) == LOW ? ":연결  " : ":끊김  ");
    }
    Serial.println();

    if (allConnected()) {
      armed = true;
      randomSeed(micros());          // 선을 꽂은 순간의 시간으로 무작위 결정
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
      Serial.println("다시 하려면 선을 모두 꽂아주세요.");
      gameOver = true;
    }
  }
  delay(50);
}
