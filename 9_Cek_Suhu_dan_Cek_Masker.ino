#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();


#define pwm1 D4
#define pwm2 D0
// ------------ PIN and Configuration ------------
int trigger1      = 30;     // 30cm jarak sensor terdeteksi
int suhuTrigger   = 37;     // > 37 maka suhu badan panas

int TRIG_PIN_1    = D7;     // D7
int ECHO_PIN_1    = D8;     // D8
int pinIn         = D5;     // D5
int pinIn2        = D6;     // D6
int pinOut        = D3;     // D3
//int pinRelay      = D4;      // D4
//int pinButton1    = 5;      // D1 -- abaikan hanya mode tes.
//int pinButton2    = 4;      // D2 -- abaikan hanya mode tes.


bool mode = true;
// ------------------------------------------------
int distance1;
float suhu;
int out, in;
bool aman, raspi, kunci, hasil, masker;

void setup() {
  Serial.begin(115200);
  mlx.begin();
  lcd.init();
  lcd.backlight();//menyalakan backlight
  pinMode(pinIn, INPUT);
  pinMode(pinIn2, INPUT);
  pinMode(pinOut, OUTPUT);
  //  pinMode(pinRelay, OUTPUT);
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  //  if (!mode) {
  //    pinMode(pinButton1, INPUT_PULLUP);
  //    pinMode(pinButton2, INPUT_PULLUP);
  //  }
}

void loop() {
  bacaSensor(mode);
  kunciJarak(mode);
  //  Serial.print("Jarak= ");
  //  Serial.print(distance1);
  //  Serial.print(" ");
  //  Serial.print("Suhu= ");
  //  Serial.println(suhu);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Membaca Orang...");
  delay(500);

  while (kunci) {
    for (int i = 3; i > 1; i--) {
      cekSensor(mode);
      lcd.setCursor(0, 0); lcd.print("Menghitung Suhu Tubuh " + String(i));
      //      lcd.clear();
      delay(1000);
    }
    lcd.setCursor(0, 0); lcd.print("Suhu anda adalah = ");
    lcd.setCursor(0, 1); lcd.print( String(suhu) + " | " + String(aman));
    //    lcd.clear();
    delay(2000);
    if (!aman) {    // KALAU SUHU TIDAK AMAN
      lcd.setCursor(0, 0); lcd.print("Maaf Suhu=-e panas!");
      delay(2000);
      lcd.clear();
      return;
    }
    digitalWrite(pinOut, HIGH);
    do {
      hasil = digitalRead(pinIn);
      masker = digitalRead(pinIn2);
      lcd.setCursor(0, 0); lcd.print("Menunggu Hasil Masker...");
      //      lcd.clear();
    } while (!hasil);
    lcd.setCursor(0, 0); lcd.print(String() + "Hasil = " );
    lcd.setCursor(0, 1); lcd.print(hasil + " | Masker = " + masker);
    //    lcd.clear();
    if (!masker) {
      // lcd print masker digunakan
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Masker Digunakan.. Membuka pintu! ");
      lcd.setCursor(0, 1); lcd.print("Membuka pintu! ");
      analogWrite(pwm1, 50);
      analogWrite(pwm2, 0);
      delay(1000);
      analogWrite(pwm1, 0);
      analogWrite(pwm2, 0);
      delay(3000);
      analogWrite(pwm1, 0);
      analogWrite(pwm2, 60);
      delay(1000);
      analogWrite(pwm1, 0);
      analogWrite(pwm2, 0);
      delay(5000);
      //      lcd.clear();
      // buka pintu
    } else {
      // lcd print masker tidak digunakan
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Masker tidak digunakan..");
      lcd.setCursor(0, 1); lcd.print("pakai masker terlebih dahulu!");
      //
    }
    delay(2000);
    digitalWrite(pinOut, LOW);

    kunci = false;
    //    return;
    break;
  }
}


void cekSensor(bool mode) {
  bacaSensor(mode);
  if (suhu > 37) aman = false;
  else aman = true;
}


void kunciJarak(bool mode) {
  if (mode) {
    if (distance1 < trigger1) kunci = true;
    else kunci = false;
  }
  else {
    if (distance1 == 0) kunci = false;
    else kunci = true;
    Serial.println(kunci);
  }
}

void bacaSensor(bool mode) {
  //  if (!mode) {
  //    suhu = digitalRead(pinButton1);
  //    distance1 = digitalRead(pinButton2);
  //  }
  //  else {
  digitalWrite(TRIG_PIN_1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_1, LOW);
  const unsigned long duration1 = pulseIn(ECHO_PIN_1, HIGH);
  distance1 = duration1 / 29 / 2;
  suhu = mlx.readObjectTempC();
  Serial.print("Hasil= ");
  Serial.print(hasil);
  Serial.print(" ");
  Serial.print("Masker= ");
  Serial.println(masker);

  //  }
}
