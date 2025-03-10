#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#define PCA9548A_ADDRESS 0x70

Adafruit_VL53L0X sensorentrada = Adafruit_VL53L0X();
Adafruit_VL53L0X sensorsaida = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor1 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor2 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor3 = Adafruit_VL53L0X();

Servo servoentrada;
Servo servosaida;

const int led1R = 23;
const int led1G = 27;
const int led2R = 18;
const int led2G = 19;
const int led3R = 25;
const int led3G = 26;

const int botentrada = 12;
const int botsaida = 13;
byte entrada = 0;
byte saida = 0;

unsigned long tempo_ant_e = 0;
unsigned long tempo_ant_s = 0;

bool v1ocupada = false;
bool v2ocupada = false;
bool v3ocupada = false;

const char* ssid = "SATC IOT";
const char* password = "IOT2024@#";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/estado", HTTP_GET, estado);
  server.begin();

  pinMode(led1R, OUTPUT);
  pinMode(led1G, OUTPUT);
  pinMode(led2R, OUTPUT);
  pinMode(led2G, OUTPUT);
  pinMode(led3R, OUTPUT);
  pinMode(led3G, OUTPUT);
  pinMode(botentrada, INPUT_PULLUP);
  pinMode(botsaida, INPUT_PULLUP);

  servoentrada.attach(4);
  servosaida.attach(2);
  servoentrada.write(0);
  servosaida.write(90);

  int ledsR[] = { led1R, led2R, led3R };
  for (int i = 0; i < 3; i++) digitalWrite(ledsR[i], LOW);
  int ledsG[] = { led1G, led2G, led3G };
  for (int i = 0; i < 3; i++) digitalWrite(ledsG[i], HIGH);

  selectChannel(4);
  while (!sensor1.begin()) {
    Serial.println("Não foi possível encontrar um VL53L0X vaga 1");
  }
  Serial.println("VL53L0X vaga 1 pronto");

  selectChannel(5);
  while (!sensor2.begin()) {
    Serial.println("Não foi possível encontrar um VL53L0X vaga 2");
  }
  Serial.println("VL53L0X vaga 2 pronto");

  selectChannel(6);
  while (!sensor3.begin()) {
    Serial.println("Não foi possível encontrar um VL53L0X vaga 3");
  }
  Serial.println("VL53L0X vaga 3 pronto");

  selectChannel(7);
  while (!sensorentrada.begin()) {
    Serial.println("Não foi possível encontrar um VL53L0X entrada");
  }
  Serial.println("VL53L0X entrada pronto");

  selectChannel(2);
  while (!sensorsaida.begin()) {
    Serial.println("Não foi possível encontrar um VL53L0X saida");
  }
  Serial.println("VL53L0X saida pronto");
}

void selectChannel(uint8_t channel) {
  Wire.beginTransmission(PCA9548A_ADDRESS);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void deactivateChannel() {
  Wire.beginTransmission(PCA9548A_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
}

void lerSensor(Adafruit_VL53L0X& sensor, bool& ocupada, int ledR, int ledG, uint8_t canal) {
  selectChannel(canal);
  VL53L0X_RangingMeasurementData_t dist;
  sensor.rangingTest(&dist, false);

  if (dist.RangeStatus != 4) {
    Serial.println(dist.RangeMilliMeter);
    if (dist.RangeMilliMeter < 50) {
      ocupada = true;
      digitalWrite(ledR, HIGH);
      digitalWrite(ledG, LOW);
    } else {
      ocupada = false;
      digitalWrite(ledR, LOW);
      digitalWrite(ledG, HIGH);
    }
  } else {
    Serial.println("Fora de alcance");
  }
  deactivateChannel();
}

void loop() {
  server.handleClient();

  lerSensor(sensor1, v1ocupada, led1R, led1G, 4);

  lerSensor(sensor2, v2ocupada, led2R, led2G, 5);

  lerSensor(sensor3, v3ocupada, led3R, led3G, 6);


  switch (entrada) {
    case 0:
      selectChannel(7);
      VL53L0X_RangingMeasurementData_t distentrada;
      sensorentrada.rangingTest(&distentrada, false);

      if (distentrada.RangeStatus != 4) {
        Serial.print("Distância entrada: ");
        Serial.print(distentrada.RangeMilliMeter);
        Serial.println(" mm");

        if (distentrada.RangeMilliMeter < 50) {
          entrada = 1;
        }

      } else {
        Serial.println("Fora de alcance");
      }
      break;
    case 1:
      {
        bool bote = !digitalRead(botentrada);
        if (bote == 1) {
          servoentrada.write(90);
          tempo_ant_e = millis();
          entrada = 2;
        }
        break;
      }

    case 2:
      if (millis() - tempo_ant_e > 4000) {
        servoentrada.write(0);
        entrada = 0;
      }
      break;
  }

  switch (saida) {
    case 0:
      selectChannel(2);
      VL53L0X_RangingMeasurementData_t distsaida;
      sensorsaida.rangingTest(&distsaida, false);

      if (distsaida.RangeStatus != 4) {
        Serial.print("Distância saida: ");
        Serial.print(distsaida.RangeMilliMeter);
        Serial.println(" mm");

        if (distsaida.RangeMilliMeter < 50) {
          saida = 1;
        }

      } else {
        Serial.println("Fora de alcance");
      }
      break;
    case 1:
      {
        bool bots = !digitalRead(botsaida);
        if (bots == 1) {
          servosaida.write(180);
          tempo_ant_s = millis();
          saida = 2;
        }
        break;
      }
    case 2:
      if (millis() - tempo_ant_s > 4000) {
        servosaida.write(90);
        saida = 0;
      }
      break;
  }
}

void estado() {
  StaticJsonDocument<128> jsonDoc;
  jsonDoc["vaga1"] = v1ocupada;
  jsonDoc["vaga2"] = v2ocupada;
  jsonDoc["vaga3"] = v3ocupada;

  String jsonString;
  serializeJson(jsonDoc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleRoot() {
  String html = R"(
    <!DOCTYPE html>
    <head>
        <title>Status das Vagas</title>
        <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@500&display=swap" rel="stylesheet">
        <style>
            body {
                font-family: 'Roboto', sans-serif;
                text-align: center;
                background-color: #f0f0f0;
            }
            .vaga {
                display: inline-block;
                width: 200px;
                height: 450px;
                margin: 10px;
                background-color: green;
                color: white;
                font-size: 20px;
                vertical-align: middle;
                border-radius: 8px;
            }
            .ocupada {
                background-color: red;
            }
        </style>
    </head>
    <body>
        <h1>Status das Vagas</h1>
    <div class="vaga" id="vaga1">
        <p id="estadoVaga1"></p>
    </div>
    <div class="vaga" id="vaga2">
        <p id="estadoVaga2"></p>
    </div>
    <div class="vaga" id="vaga3">
        <p id="estadoVaga3"></p>
    </div>

    <script>
        function atualizarvagas() {
            fetch(`http://%s/estado`) 
                .then(response => response.json())
                .then(data => {
                    const vagas = [data.vaga1, data.vaga2, data.vaga3];
                    vagas.forEach((estado, index) => {
                        const vagaElemento = document.getElementById(`vaga${index + 1}`);
                        if (estado) {
                            vagaElemento.classList.add('ocupada');
                            vagaElemento.innerText = 'Vaga ocupada';
                        } else {
                            vagaElemento.classList.remove('ocupada');
                            vagaElemento.innerText = 'Vaga livre';
                        }
                    });
                })
                .catch(error => {
                    console.error('Erro ao obter o estado das vagas:', error);
                });
        }
        setInterval(atualizarvagas, 1000);
        </script>
    </body>
    </html>
  )";
  html.replace("%s", WiFi.localIP().toString());
  server.send(200, "text/html", html);
}
