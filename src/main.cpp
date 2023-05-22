#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <secrets.h>

#define PWM_L D10
#define PWM_R D9
#define DIR_L D8
#define DIR_R D7

ESP8266WebServer server(80);

bool LEDstatus = LOW;

void mover(){
  String dx=server.arg("dx");
    String dy=server.arg("dy");
    server.send(200);
    int x = dx.toInt();    
    int y = dy.toInt();
    Serial.print(x);  Serial.print("  ");  Serial.println(y);  
    int speedR = 0;
    int speedL = 0;
    int max = 255;
    if (x>0) {
      speedL = abs(y);
      if(abs(y)>x) {
      speedR = abs(y) - x;
      } else {
        speedR = 0;
      }
    } else if (x<0) {
      speedR = abs(y);
      if(abs(y)>x) {
        speedL = abs(y) - abs(x);
      } else {
        speedL = 0;
      }
    } else {
      speedR = abs(y);
      speedL = abs(y);
    }

    if(y>0) {
      analogWrite(PWM_R, speedR);
      analogWrite(PWM_L, speedL);
      digitalWrite(DIR_R, LOW);
      digitalWrite(DIR_L, LOW);
    }else if (y<0) {
      analogWrite(PWM_R, speedR);
      analogWrite(PWM_L, speedL);
      digitalWrite(DIR_R, HIGH);
      digitalWrite(DIR_L, HIGH);
    } else {
      analogWrite(PWM_R, 0);
      analogWrite(PWM_L, 0);
    }
}
void setup() {
  //Motores Zumo Shield 1.2
  pinMode(PWM_L,  OUTPUT);
  pinMode(PWM_R,  OUTPUT);
  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);
  analogWrite(PWM_R, 0);
  analogWrite(PWM_L, 0);

  //LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 
  
  //Monitor serie
  Serial.begin(115200);
  delay(10);
 
  //Carga memoria
  if(!LittleFS.begin()){
    Serial.println("Error al cargar LittleFS");
    return;
  }

  // Conectar WiFi
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.hostname("ESP");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  digitalWrite(LED_BUILTIN, HIGH); 

  // Imprimir IP Local
  Serial.print("IP: ");
  Serial.print(WiFi.localIP());

  //Iniciar servidor
  server.on("/", []() {
    File index = LittleFS.open("/index.html", "r");
    if(!index){
      Serial.println("Error al cargar archivo");
      return;
    }
    server.streamFile(index, "text/html");
    index.close();
  });

  server.on("/stick", []() {
    mover();
  });
  server.begin();
}

void loop() {
  server.handleClient();
}