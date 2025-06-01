#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// — Pines y objetos —
#define DHT_PIN     2       // Pin digital para DHT22
#define DHT_TYPE    DHT22
DHT dhtSensor(DHT_PIN, DHT_TYPE);

const int LDR_PIN             = A0;  // Pin analógico para la LDR

// Pines de los 5 LEDs de iluminación artificial
const int LED1_PIN            = 3;
const int LED2_PIN            = 4;
const int LED3_PIN            = 5;
const int LED4_PIN            = 6;
const int LED5_PIN            = 7;

// Servo que simula válvula térmica
Servo valveServo;
const int SERVO_PIN           = 9;
// Ángulos del servo: 0° = calentar, 90° = reposo, 180° = enfriar
const int ANGLE_HEAT          = 0;
const int ANGLE_IDLE          = 90;
const int ANGLE_COOL          = 180;

// Pines para las electroválvulas simuladas (LEDs)
const int HEAT_VALVE_PIN      = 10;
const int COOL_VALVE_PIN      = 11;

// — Parámetros de control de temperatura —
const float TARGET_TEMP       = 25.0;
const float DEAD_ZONE_WIDTH   = 2.0;
const float MIN_TEMP_LIMIT    = TARGET_TEMP - DEAD_ZONE_WIDTH;
const float MAX_TEMP_LIMIT    = TARGET_TEMP + DEAD_ZONE_WIDTH;

// — Parámetros de caracterización de la LDR —
const float GAMMA             = 0.7;
const float RL10              = 50;   // kΩ en el punto 10 lux

// — Umbrales de lux para 5 niveles de LEDs (en lux reales) —
const float LUX_THRESHOLD1    = 100.0;   // ajustar según entorno
const float LUX_THRESHOLD2    = 500.0;
const float LUX_THRESHOLD3    = 2000.0;
const float LUX_THRESHOLD4    = 10000.0;
const float LUX_THRESHOLD5    = 50000.0;

// — Pantalla LCD 20×4 I2C —
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  // Inicializar sensor DHT22
  dhtSensor.begin();
  // Inicializar servo en posición reposo
  valveServo.attach(SERVO_PIN);
  valveServo.write(ANGLE_IDLE);
  // Configurar LEDs de iluminación artificial
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(LED5_PIN, OUTPUT);
  // Configurar electroválvulas simuladas
  pinMode(HEAT_VALVE_PIN, OUTPUT);
  pinMode(COOL_VALVE_PIN, OUTPUT);
  // Inicializar LCD
  lcd.init();
  lcd.backlight();
}

void loop() {
  // 1) Leer sensores de temperatura y humedad
  float temperature = dhtSensor.readTemperature();
  float humidity    = dhtSensor.readHumidity();

  // 2) Leer LDR y convertir a lux real
  int   ldrVal      = analogRead(LDR_PIN);
  float voltage = ldrVal / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

  // 3) Control de temperatura ON-OFF con zona muerta
  String servoState;
  bool   heatOn     = false;
  bool   coolOn     = false;

  if (temperature < MIN_TEMP_LIMIT) {
    valveServo.write(ANGLE_HEAT);
    servoState = "CALENTAR";
    heatOn     = true;
  }
  else if (temperature > MAX_TEMP_LIMIT) {
    valveServo.write(ANGLE_COOL);
    servoState = "ENFRIAR";
    coolOn     = true;
  }
  else {
    valveServo.write(ANGLE_IDLE);
    servoState = "REPOSO";
  }
  digitalWrite(HEAT_VALVE_PIN, heatOn ? HIGH : LOW);
  digitalWrite(COOL_VALVE_PIN, coolOn ? HIGH : LOW);

  // 4) Control de iluminación escalonada (5 LEDs) según lux
  int ledLevel = 0;
  if      (lux < LUX_THRESHOLD1) ledLevel = 5;
  else if (lux < LUX_THRESHOLD2) ledLevel = 4;
  else if (lux < LUX_THRESHOLD3) ledLevel = 3;
  else if (lux < LUX_THRESHOLD4) ledLevel = 2;
  else if (lux < LUX_THRESHOLD5) ledLevel = 1;
  else                            ledLevel = 0;

  digitalWrite(LED1_PIN, ledLevel >= 1);
  digitalWrite(LED2_PIN, ledLevel >= 2);
  digitalWrite(LED3_PIN, ledLevel >= 3);
  digitalWrite(LED4_PIN, ledLevel >= 4);
  digitalWrite(LED5_PIN, ledLevel >= 5);

  // 5) Mostrar datos en la pantalla LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print("C Hum:");
  lcd.print(humidity, 1);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Lux:");
  lcd.print(lux, 0);
  lcd.print(" Level:");
  lcd.print(ledLevel);

  lcd.setCursor(0, 2);
  lcd.print("Accion:");
  lcd.print(servoState);

  lcd.setCursor(0, 3);
  lcd.print("Heat:");
  lcd.print(heatOn ? "ON " : "OFF");
  lcd.print(" Cool:");
  lcd.print(coolOn ? "ON " : "OFF");

  // Esperar 2 segundos antes de la siguiente iteración
  delay(2000);
}
