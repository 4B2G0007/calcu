#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

char KeyValue[] = {'1', '2', '3', 'A', 
                   '4', '5', '6', 'B', 
                   '7', '8', '9', 'C', 
                   '*', '0', '#', 'D'};
byte Row = 0, Col = 0;

unsigned long lastKey = 0;
const int debounceDelay = 200;

String inputBuffer = "";

void setup() {
  lcd.begin(16, 2);
  pinMode(12, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  pinMode(13, OUTPUT);

  lcd.clear();
  lcd.print("enter number");
}

void loop() {
  if (scankey()) {
    unsigned long currentTime=millis();
    if (currentTime-lastKey>debounceDelay) {
      lastKey = currentTime;

      int keyIndex = (Row-1)*4+(Col-1);
      char key = KeyValue[keyIndex];

      if (key >='0' && key<='9') {
        inputBuffer += key;
        lcd.clear();
        lcd.print(inputBuffer);
      } else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
        if (inputBuffer.length() > 0 && inputBuffer[inputBuffer.length() - 1] >= '0' && inputBuffer[inputBuffer.length() - 1] <= '9') {
          char operatorSymbol = (key == 'A') ? '+' : (key == 'B') ? '-' : (key == 'C') ? '*' : '/';
          inputBuffer += operatorSymbol;
          lcd.clear();
          lcd.print(inputBuffer);
        }
      } else if (key == '*') {
        inputBuffer = "";
        lcd.clear();
      } else if (key == '#') {
        lcd.clear();
        if (inputBuffer.length() == 0) {
          lcd.print("pls enter number");
          
          digitalWrite(13, HIGH);
          delay(1000);
          digitalWrite(13, LOW);
          
          delay(2000);
          lcd.clear();
        } else {
          double result = calculateExpression(inputBuffer);

          String displayResult;
          if (result == (int)result) {
            displayResult = String((int)result);
          } else {
            char buffer[16];
            dtostrf(result, 1, 2, buffer);
            displayResult = String(buffer);

            while (displayResult.endsWith("0")) {
              displayResult.remove(displayResult.length() - 1);
            }
            if (displayResult.endsWith(".")) {
              displayResult.remove(displayResult.length() - 1);
            }
          }

          inputBuffer = "";
          lcd.print(displayResult);
        }
      }
    }
  }
}

double calculateExpression(String expr) {
  for (int i = 0; i < expr.length(); i++) {
    char c = expr[i];
    if (c == '*' || c == '/') {
      int leftStart = i - 1;
      while (leftStart >= 0 && ((expr[leftStart] >= '0' && expr[leftStart] <= '9') || expr[leftStart] == '.')) {
        leftStart--;
      }
      leftStart++;
      int rightEnd = i + 1;
      while (rightEnd < expr.length() && ((expr[rightEnd] >= '0' && expr[rightEnd] <= '9') || expr[rightEnd] == '.')) {
        rightEnd++;
      }
      double leftNum = expr.substring(leftStart, i).toDouble();
      double rightNum = expr.substring(i + 1, rightEnd).toDouble();
      double result = (c == '*') ? leftNum * rightNum : (rightNum == 0 ? 0 : leftNum / rightNum);
      expr = expr.substring(0, leftStart) + String(result) + expr.substring(rightEnd);
      i = leftStart - 1;
    }
  }

  double total = 0;
  char op = '+';
  String numStr = "";

  for (int i = 0; i < expr.length(); i++) {
    char c = expr[i];
    if ((c >= '0' && c <= '9') || c == '.') {
      numStr += c;
    } else {
      total = (op == '+') ? total + numStr.toDouble() : total - numStr.toDouble();
      op = c;
      numStr = "";
    }
  }
  if (numStr.length() > 0) {
    total = (op == '+') ? total + numStr.toDouble() : total - numStr.toDouble();
  }
  return total;
}

bool scankey() {
  Row = 0;
  Col = 0;
  bool keyPressed = false;

  for (int i = 0; i < 4; i++) {
    digitalWrite(A0, i == 0 ? LOW : HIGH);
    digitalWrite(A1, i == 1 ? LOW : HIGH);
    digitalWrite(A2, i == 2 ? LOW : HIGH);
    digitalWrite(A3, i == 3 ? LOW : HIGH);
    delayMicroseconds(100);

    if (digitalRead(12) == LOW) { Row = 1; Col = i + 1; keyPressed = true; break; }
    if (digitalRead(11) == LOW) { Row = 2; Col = i + 1; keyPressed = true; break; }
    if (digitalRead(10) == LOW) { Row = 3; Col = i + 1; keyPressed = true; break; }
    if (digitalRead(3) == LOW) { Row = 4; Col = i + 1; keyPressed = true; break; }
  }

  return keyPressed;
}
