#include <ESP8266WiFi.h>
// datos para la conexion al sensor de temperatura y humedad

// datos para la inicializacion de sensor movimiento
int pir_pin = D2;      // Pin D2 en NodeMCU para el sensor PIR
int led_pin = D3;      // Pin D3 en NodeMCU para el LED
int buzzer_pin = D4;   // Pin D4 en NodeMCU para el zumbador
int moving;

// datos para la conexion a internet
const char* ssid = "Sofi-IoT";// SSID, the name of your WiFi connection
const char* password = "password";// password of the WiFi
const char* host = "172.20.10.11";// IP server, obtained with 'ipconfig' in command shell
const int port = 80;// server port, configured in xampp for Apache server, default is: 80

const int watchdog = 5000;// watchdog frequency, reconnect timeout
unsigned long previousMillis = millis();
int c = 0;// the data

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);// initialize the serial port
  Serial.println("Counter\n\n");

  // inicializacion de pines para sensor
  pinMode(pir_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  
  WiFi.begin(ssid, password);// initialize the WiFi connection
  
  // checking for unsuccessful connection
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  // continue after successful connection
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  delay(100);
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("\nValor = ");
  Serial.println(c++);// print and increment the data
  
  // procesos de sensor movimiento
  if (digitalRead(pir_pin) == LOW)
  {
    digitalWrite(led_pin, HIGH);
    digitalWrite(buzzer_pin, HIGH);
    moving = 1;
    Serial.println("¡VIGILO!");
  }
  else
  {
    digitalWrite(led_pin, LOW);
    digitalWrite(buzzer_pin, LOW);
    moving = 0;
    Serial.println("MOVIMIENTO!");
  }

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > watchdog){
    previousMillis = currentMillis;
    WiFiClient client;// creating the client

    // checking if we can connect to the server
    if(!client.connect(host, port)){
      Serial.println("Fail to connect");
      return;
    }

    // building our url according our database
    String url = "/test/index.php?cont=";
    url += c;
    url += "&move=";
    url += moving;

    //Print data to the server that a client is connected to
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    // if watchdog timeout, the client is stopped
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > 5000){
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    //Read reply from the server
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  
  delay(2000);
}
