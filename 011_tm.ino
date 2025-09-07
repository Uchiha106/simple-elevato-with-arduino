#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define NUMBER_OF_SHIFT_CHIPS  2
#define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8
#define TotalIC 3
#define TotalICPins TotalIC * 8

unsigned long preClose = 0;
unsigned long time5s = 5000;
const unsigned long tmove = 2000;
unsigned long preOpen = 0;
unsigned long timeclose = 2000;
int dlbt = 5;
int cr_Floor = 1;
int LoadPin    = 7;
int EnablePin  = 4;
int DataPin    = 5;
int ClockPin   = 6;

int RCLK = 10;//st cp 12
int SER = 12;// data 14
int SRCLK = 11;//clk 11 sh cp

unsigned long pinValues;
unsigned long oldPinValues;
int currentFloor = 1;         // Biến để theo dõi tầng hiện tại của thang máy
bool isMoving = false;        // Biến theo dõi trạng thái di chuyển
//.....................Trạng thái LED
//..........IC74HC165_1
bool ledState11 = false;  // Trạng thái LED (bật/tắt)đèn ngoài t5
bool ledState12 = false;  // Trạng thái LED (bật/tắt)đèn ngoài t4
bool ledState13 = false;  // Trạng thái LED (bật/tắt)đèn ngoài t3
bool ledState14 = false;  // Trạng thái LED (bật/tắt)đèn ngoài t2
bool ledState15 = false;  // Trạng thái LED (bật/tắt)đèn ngoài t1

bool lastButtonState28 = false;

//..........IC74HC165_2
bool ledState21 = false;  // Trạng thái LED (bật/tắt)DB5
bool ledState22 = false;  // Trạng thái LED (bật/tắt)DB4
bool ledState23 = false;  // Trạng thái LED (bật/tắt)DB3
bool ledState24 = false;  // Trạng thái LED (bật/tắt)DB2
bool ledState25 = false;  // Trạng thái LED (bật/tắt)DB1

bool ledState26 = false;  // Trạng thái LED (bật/tắt)LOP
bool ledState27 = false;  // Trạng thái LED (bật/tắt)LCL
bool ledState28 = false;  // Trạng thái LED (bật/tắt)LWN

//...............END

//...............Led hiển thị
bool ledState_DT1 = false, ledState_DT2 = false, ledState_DT3 = false, ledState_DT4 = false, ledState_DT5 = false;
bool ledState_DC1 = false, ledState_DC2 = false, ledState_DC3 = false, ledState_DC4 = false, ledState_DC5 = false;
bool ledState_DCT = false;
//end
void setup() {
  //SET LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Floor:");
  lcd.setCursor(7, 0);
  lcd.print("_ED:");
  lcd.setCursor(11, 0);
  lcd.print("CLOSE");
  lcd.setCursor(6, 0);
  lcd.print("1");
  lcd.setCursor(0, 1);
  lcd.print("State:");
  lcd.setCursor(7, 1);
  lcd.print("--STOP---");

  pinMode(LoadPin, OUTPUT);
  pinMode(EnablePin, OUTPUT);
  pinMode(ClockPin, OUTPUT);
  pinMode(DataPin, INPUT);

  digitalWrite(ClockPin, LOW);
  digitalWrite(LoadPin, HIGH);
  pinMode(SER, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLK, OUTPUT);
  digitalWrite(EnablePin, HIGH);
  //Trang thai ban dau (LED hien thi tang 1 sang)
  ledState_DT1 = true;
  ledState_DT2 = false;
  ledState_DT3 = false;
  ledState_DT4 = false;
  ledState_DT5 = false;

  ledState_DC1 = false;
  ledState_DC2 = false;
  ledState_DC3 = false;
  ledState_DC4 = false;
  ledState_DC5 = false;

  ledState_DCT = false;
  ledState21 = false; ledState22 = false; ledState23 = false; ledState24 = false; ledState25 = false; ledState26 = false; ledState27 = false; ledState28 = false;
  ledState11 = false; ledState12 = false; ledState13 = false; ledState14 = false; ledState15 = false;
  updateLEDs();

}

void loop() {
  pinValues = read_shift_regs();

  if (pinValues != oldPinValues)
  {
    oldPinValues = pinValues;
  }

  digitalWrite(LoadPin, LOW);    // Kích hoạt để tải dữ liệu song song
  delayMicroseconds(5);
  digitalWrite(LoadPin, HIGH);   // Đưa PL trở lại HIGH để đọc dữ liệu nối tiếp

  // Bắt đầu đọc dữ liệu từ các IC
  byte dataIC1 = shiftIn(DataPin, ClockPin, MSBFIRST);
  byte dataIC2 = shiftIn(DataPin, ClockPin, MSBFIRST);

  //...................Set button
  //.................ic74HC165_1

  bool cBtnS11 = bitRead(dataIC1, 5); delay(dlbt);//bt led ngoai tang 5
  bool cBtnS12 = bitRead(dataIC1, 4); delay(dlbt);//bt led ngoai tang 4
  bool cBtnS13 = bitRead(dataIC1, 3); delay(dlbt);//bt led ngoai tang 3
  bool cBtnS14 = bitRead(dataIC1, 2); delay(dlbt);//bt led ngoai tang 2
  bool cBtnS15 = bitRead(dataIC1, 1); delay(dlbt);//bt led ngoai tang 1

  bool cBtnS26 = bitRead(dataIC2, 6); delay(dlbt);//bt open
  bool cBtnS27 = bitRead(dataIC2, 7); delay(dlbt);//bt close
  bool cBtnS28 = bitRead(dataIC1, 6); delay(dlbt);//bt warning

  bool cBtnS21 = bitRead(dataIC2, 1); delay(dlbt);//bt led trong tang 5
  bool cBtnS22 = bitRead(dataIC2, 2); delay(dlbt);//bt led trong tang 4
  bool cBtnS23 = bitRead(dataIC2, 3); delay(dlbt);//bt led trong tang 3
  bool cBtnS24 = bitRead(dataIC2, 4); delay(dlbt);//bt led trong tang 2
  bool cBtnS25 = bitRead(dataIC2, 5); delay(dlbt);//bt led trong tang 1

  if (ledState_DC1 || ledState_DC2 || ledState_DC3 || ledState_DC4 || ledState_DC5) {
    ledState_DCT = true;
  } else {
    ledState_DCT = false;
  }
  // BT5
  if (cBtnS11 == true && currentFloor == 5) {
    ledState11 = false;
    ledState_DC5 = true;
    preOpen = millis();
    updateLEDs();
  } else if (cBtnS11 == true && currentFloor != 5) {
    ledState11 = true;

  }
  
  // BT4
  if (cBtnS12 == true && currentFloor == 4) {
    ledState12 = false;
    ledState_DC4 = true;
    preOpen = millis();
    updateLEDs();
  } else if (cBtnS12 == true && currentFloor != 4) {
    ledState12 = true;

  }

  // BT3
  if (cBtnS13 == true && currentFloor == 3) {
    ledState13 = false;
    ledState_DC3 = true;
    preOpen = millis();
    updateLEDs();
  } else if (cBtnS13 == true && currentFloor != 3) {
    ledState13 = true;
  }

  // BT2
  if (cBtnS14 == true && currentFloor == 2) {
    ledState14 = false;
    ledState_DC2 = true;
    preOpen = millis();
    updateLEDs();
  } else if (cBtnS14 == true && currentFloor != 2) {
    ledState14 = true;

  }

  // BT1
  if (cBtnS15 == true && currentFloor == 1) {
    ledState15 = false;
    ledState_DC1 = true;
    preOpen = millis();
    updateLEDs();
  } else if (cBtnS15 == true && currentFloor != 1) {
    ledState15 = true;

  }


  // T5
  if (cBtnS21 == true && currentFloor == 5) {
    ledState21 = false;
  } else if (cBtnS21 == true && currentFloor != 5) {
    ledState21 = true;
  }

  // T4
  if (cBtnS22 == true && currentFloor == 4) {
    ledState22 = false;

  } else if (cBtnS22 == true && currentFloor != 4) {
    ledState22 = true;

  }


  // T3
  if (cBtnS23 == true && currentFloor == 3) {
    ledState23 = false;
  } else if (cBtnS23 == true && currentFloor != 3) {
    ledState23 = true;

  }
  // T2
  if (cBtnS24 == true && currentFloor == 2) {
    ledState24 = false;  // Đảo ngược trạng thái của LED
  } else if (cBtnS24 == true && currentFloor != 2) {
    ledState24 = true;

  }


  // T1
  if (cBtnS25 == true && currentFloor == 1) {
    ledState25 = false;  // Đảo ngược trạng thái của LED
  } else if (cBtnS25 == true && currentFloor != 1) {
    ledState25 = true;

  }

  // .............................OP
  if (cBtnS26 == true) {
    ledState26 = true;
  }
  if (cBtnS26 == false) {
    ledState26 = false;
  }
  //............................. CL
  if (cBtnS27 == true) {
    ledState27 = true;
  }
  if (cBtnS27 == false) {
    ledState27 = false;
  }
  if (cBtnS26 == true ) {
    if (ledState26 == true && ledState_DT1 == true ) {
      ledState_DC1 = true;
      preOpen = millis();
      updateLEDs();
    }
    if (ledState26 == true && ledState_DT2 == true ) {
      ledState_DC2 = true;
      preOpen = millis();
      updateLEDs();
    }
    if (ledState26 == true && ledState_DT3 == true ) {
      ledState_DC3 = true;
      preOpen = millis();
      updateLEDs();
    }
    if (ledState26 == true && ledState_DT4 == true ) {
      ledState_DC4 = true;
      preOpen = millis();
      updateLEDs();
    }
    if (ledState26 == true && ledState_DT5 == true ) {
      ledState_DC5 = true;
      preOpen = millis();
      updateLEDs();
    }
  } else if (cBtnS27 == true ) {
    //time5s = timeclose;
    preOpen = millis() - 3000;
  }
  if (ledState_DC1 && millis() - preOpen >= time5s) {
    ledState_DC1 = false;
    updateLEDs();
  }
  if (ledState_DC2 && millis() - preOpen >= time5s) {
    ledState_DC2 = false;
    updateLEDs();
  }
  if (ledState_DC3 && millis() - preOpen >= time5s) {
    ledState_DC3 = false;
    updateLEDs();
  }
  if (ledState_DC4 && millis() - preOpen >= time5s) {
    ledState_DC4 = false;
    updateLEDs();
  }
  if (ledState_DC5 && millis() - preOpen >= time5s) {
    ledState_DC5 = false;
    updateLEDs();
  }
  // WN
  if (cBtnS28 && !lastButtonState28) {
    ledState28 = !ledState28;
  } lastButtonState28 = cBtnS28;

  ///..........END

  // Xử lý các logicState và di chuyển thang máy
  if
  (ledState15 && currentFloor != 1) {
    moveToFloor(1);
    ledState15 = false;
  } else if (ledState14 && currentFloor != 2) {
    moveToFloor(2);
    ledState14 = false;
  } else if (ledState13 && currentFloor != 3) {
    moveToFloor(3);
    ledState13 = false;
  } else if (ledState12 && currentFloor != 4) {
    moveToFloor(4);
    ledState12 = false;
  } else if (ledState11 && currentFloor != 5) {
    moveToFloor(5);
    ledState11 = false;
  }

  if (ledState25 && currentFloor != 1) {
    moveToFloor(1);
    ledState25 = false;
  } else if (ledState24 && currentFloor != 2) {
    moveToFloor(2);
    ledState24 = false;
  } else if (ledState23 && currentFloor != 3) {
    moveToFloor(3);
    ledState23 = false;
  } else if (ledState22 && currentFloor != 4) {
    moveToFloor(4);
    ledState22 = false;
  } else if (ledState21 && currentFloor != 5) {
    moveToFloor(5);
    ledState21 = false;
  }
  updateLEDs();
  //UPDATE LCD
  if (ledState28 == true) {
    lcd.setCursor(6, 1);
    lcd.print(" --ERROR--");
  }
  if (ledState_DCT) {
    lcd.setCursor(11, 0);
    lcd.print("OPEN ");
  } else {
    lcd.setCursor(11, 0);
    lcd.print("CLOSE");
  }

}
unsigned long read_shift_regs()
{
  long bitVal;
  unsigned long bytesVal = 0;

  digitalWrite(EnablePin, HIGH);
  digitalWrite(LoadPin, LOW);
  delayMicroseconds(5);
  digitalWrite(LoadPin, HIGH);
  digitalWrite(EnablePin, LOW);

  for (int i = 0; i < DATA_WIDTH; i++)
  {
    bitVal = digitalRead(DataPin);

    digitalWrite(ClockPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(ClockPin, LOW);
  }

  return (bytesVal);
}

void updateLCD( int crFloor) {
  if (crFloor == 1 ) {
    lcd.setCursor(11, 0);
    lcd.print("CLOSE");
    lcd.setCursor(6, 0);
    lcd.print("1");
  } else if (crFloor == 2) {
    lcd.setCursor(11, 0);
    lcd.print("CLOSE");
    lcd.setCursor(6, 0);
    lcd.print("2");
  } else if (crFloor == 3) {
    lcd.setCursor(6, 0);
    lcd.print("3");
  } else if (crFloor == 4) {
    lcd.setCursor(6, 0);
    lcd.print("4");
  } else if (crFloor == 5) {
    lcd.setCursor(6, 0);
    lcd.print("5");
  }

}
void moveToFloor(int targetFloor) {
  if (ledState_DCT == false) {
    if (isMoving || targetFloor == currentFloor) {
      //...


      //...
      return;
    }

    isMoving = true;  // Thang bắt đầu di chuyển

    while (currentFloor != targetFloor) {


      delay(tmove);

      if (currentFloor < targetFloor) {
        currentFloor++;  // Di chuyển lên
      } else {
        currentFloor--;  // Di chuyển xuống
      }
      if (currentFloor == 1) {
        updateLCD(1);
        ledState_DT1 = true; ledState_DT2 = false;
        ledState_DT3 = false; ledState_DT4 = false;
        ledState_DT5 = false; ledState25 = false;
        ledState15 = false;
      } else if (currentFloor == 2) {
        updateLCD(2);
        ledState_DT1 = false;
        ledState_DT2 = true;
        ledState_DT3 = false;
        ledState_DT4 = false;
        ledState_DT5 = false;
        ledState24 = false;
        ledState14 = false;
      } else if (currentFloor == 3) {
        updateLCD(3);
        ledState_DT1 = false;
        ledState_DT2 = false;
        ledState_DT3 = true;
        ledState_DT4 = false;
        ledState_DT5 = false;
        ledState23 = false;
        ledState13 = false;
      } else if (currentFloor == 4) {
        updateLCD(4);
        ledState_DT1 = false;
        ledState_DT2 = false;
        ledState_DT3 = false;
        ledState_DT4 = true;
        ledState_DT5 = false;
        ledState22 = false;
        ledState12 = false;
      } else if (currentFloor == 5) {
        updateLCD(5);
        ledState_DT1 = false;
        ledState_DT2 = false;
        ledState_DT3 = false;
        ledState_DT4 = false;
        ledState_DT5 = true;
        ledState21 = false;
        ledState11 = false;
      }
      if (currentFloor == 1 && targetFloor == 1) {
        ledState_DC1 = true;
        preOpen = millis();
        lcd.setCursor(6, 1);
        lcd.print(" --STOP---");
      } else if (currentFloor == 2 && targetFloor == 2) {
        ledState_DC2 = true;
        preOpen = millis();
        lcd.setCursor(6, 1);
        lcd.print(" --STOP---");
      } else if (currentFloor == 3 && targetFloor == 3) {
        ledState_DC3 = true;
        preOpen = millis();
        lcd.setCursor(6, 1);
        lcd.print(" --STOP---");
      } else if (currentFloor == 4 && targetFloor == 4) {
        ledState_DC4 = true;
        preOpen = millis();
        lcd.setCursor(6, 1);
        lcd.print(" --STOP---");
      } else if (currentFloor == 5 && targetFloor == 5) {
        ledState_DC5 = true;
        preOpen = millis();
        lcd.setCursor(6, 1);
        lcd.print(" --STOP---");
      } else {
        lcd.setCursor(6, 1);
        lcd.print(" --MOVING-");
      }
      updateLEDs();
    }
  } else {
    isMoving = false;  // Thang dừng di chuyển
  }

}
void updateLEDs() {
  // Gửi dữ liệu để điều khiển LED qua 74HC595
  byte ic595_1 = (ledState_DT1 << 0) | (ledState_DT2 << 1) | (ledState_DT3 << 2) | (ledState_DT4 << 3) | (ledState_DT5 << 4) | (ledState_DC1 << 5) | (ledState_DC2 << 6) | (ledState_DC3 << 7);
  byte ic595_2 = (ledState_DC4 << 0) | (ledState_DC5 << 1) | (ledState25 << 2) | (ledState24 << 3) | (ledState23 << 4) | (ledState22 << 5) | (ledState21 << 6) | (ledState_DCT << 7);
  byte ic595_3 = (ledState11 << 4) | (ledState12 << 3) | (ledState13 << 2) | (ledState14 << 1) | (ledState15 << 0) | (ledState26 << 5) | (ledState27 << 6) | (ledState28 << 7);
  digitalWrite(RCLK, LOW);

  shiftOut(SER, SRCLK, MSBFIRST, ic595_3);  // 74HC595_3
  shiftOut(SER, SRCLK, MSBFIRST, ic595_2);  //74HC595_2
  shiftOut(SER, SRCLK, MSBFIRST, ic595_1);  //74HC595_1

  digitalWrite(RCLK, HIGH);
  delay(5);
}
