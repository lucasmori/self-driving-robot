#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "COLOQUE_O_NOME_DA_SUA_REDE_AQUI"
#define STAPSK  "COLOQUE_A_SENHA_DA_SUA_REDE_AQUI"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

//Static IP address configuration
IPAddress staticIP(xxx,xxx,xxx,xx); //ESP static ip
IPAddress gateway(xxx,xxx,xx,xx);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(xxx,xxx,xxx,xxx);  //Subnet mask
IPAddress dns(x,x,x,x);  //DNS
 
int go = true;
void handleRUN();
void handleSTOP();
ESP8266WebServer server(80);

// ------------------------------------------------

#include <Servo.h>
#include <Ultrasonic.h>

#define TRIGGER_PIN D0
#define ECHO_PIN D1
#define SERVO_PIN D2
#define IR_PIN D3
#define LEFT_MOTOR_IN1 D5
#define LEFT_MOTOR_IN2 D6
#define RIGHT_MOTOR_IN3 D7
#define RIGHT_MOTOR_IN4 D8
Servo servo;

Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);

void setup()
{
  Serial.begin(115200);
  Serial.println(""); Serial.println(""); Serial.println("");
  Serial.println(analogRead(A0)); Serial.println(""); Serial.println(""); Serial.println("");
  if (analogRead(A0) > 1000 ) {
  WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("Self Driving Robot WiFi screen status\t");
    Serial.print("Connected to SSID: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) {
      Serial.println("MDNS responder started");
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/run", HTTP_GET, handleRUN);
    server.on("/stop", HTTP_GET, handleSTOP);
    server.on("/inline", []() {
      server.send(200, "text/plain", "this works as well");
    });

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
  }
  // -----------------------------

  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN3, OUTPUT);
  pinMode(RIGHT_MOTOR_IN4, OUTPUT);
  pinMode(IR_PIN, INPUT);
  servo.attach(SERVO_PIN);
  delay(5000);
  servo.write(90);
}

void handleRUN() {
  server.sendHeader("Accept" , "application/json");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  go = true;
  server.send(200, "text/plan", "the robot is running");
}

void handleSTOP() {
  server.sendHeader("Accept" , "application/json");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  go = false;
  server.send(200, "text/plan", "the robot has stop");
}

void loop()
{
  server.handleClient();
  if (go == true) {

    if (IRRead()) {
      if (distanceMeasure() > 30) {
        moveRobot("frente");
      }
      else if (distanceMeasure() < 30) {
        moveRobot("parar");
        servo.write(0);
        delay(500);
        if (distanceMeasure() > 20) {
          moveRobot("direita");
          delay(500);
          moveRobot("parar");
          servo.write(90);
          delay(500);
          moveRobot("frente");
        }
        else {

          servo.write(179);
          delay(500);
          if (distanceMeasure() > 20) {
            moveRobot("esquerda");
            delay(500);
            moveRobot("parar");
            servo.write(90);
            delay(500);
            moveRobot("frente");
          }
        }
      }
    } // end IRRead
    else {
      moveRobot("parar");
      servo.write(90);
    }
  } else {
    moveRobot("parar");
    servo.write(90);
  }
} // end loop
