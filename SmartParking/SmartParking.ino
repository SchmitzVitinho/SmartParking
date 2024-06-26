#include <NewPing.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "FIESC_IOT";          // SSID da rede WiFi
const char* password = "C6qnM4ag81";     // Senha da rede WiFi
const char* device_token = "afb1bbe4-0f88-474a-8d6d-66886a61d928"; // Token de dispositivo para autenticação no servidor TagoIO

#define TRIGGER_PIN 12    // Define o pino de trigger do sensor
#define ECHO_PIN 14       // Define o pino de echo do sensor
#define MAX_DISTANCE 20  // Define a distância máxima a ser medida (em centímetros)
#define BLUE_LED_PIN 26  // Define o pino do LED azul
#define RED_LED_PIN 15    // Define o pino do LED vermelho

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Cria uma instância do objeto NewPing

void setup() {
  Serial.begin(9600); // Inicia a comunicação serial
  WiFi.begin(ssid, password);       // Inicia a conexão WiFi com os dados fornecidos
  pinMode(BLUE_LED_PIN, OUTPUT); // Define o pino do LED azul como saída
  pinMode(RED_LED_PIN, OUTPUT);   // Define o pino do LED vermelho como saída 
  
  // Espera até conectar ao WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
}

void sendToTagoIO(int distance) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.tago.io/data"); // URL da API TagoIO
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Device-Token", device_token);

    String jsonData = "{\"variable\": \"distance\", \"value\": " + String(distance) + "}";

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Erro ao enviar dados: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Não conectado ao WiFi");
  }
}

void loop() {                      
  delay(5000); // Espera 5 segundos entre as medições
  unsigned int distance = sonar.ping_cm(); // Realiza uma medição e retorna a distância em centímetros
  
  if (distance == 0) { // Se a distância for 0, a medição falhou
    Serial.println("Vaga Disponível");
    digitalWrite(RED_LED_PIN, HIGH);
  } else {
    Serial.print("Distância: ");
    Serial.print(distance);
    Serial.println(" cm");
    digitalWrite(BLUE_LED_PIN, LOW); // Apaga o LED azul
  }

  // Se a distância for maior que 0 (medida válida) e menor ou igual a 10 cm, acende o LED azul para indicar que não há nada próximo
  if (distance > 0 && distance <= 10) {
    digitalWrite(BLUE_LED_PIN, HIGH); // Acende o LED azul
    digitalWrite(RED_LED_PIN, LOW);    // Apaga o LED vermelho
  } else {
    digitalWrite(BLUE_LED_PIN, LOW); // Apaga o LED azul
    digitalWrite(RED_LED_PIN, HIGH);    // Acende o LED vermelho
  }

sendToTagoIO(distance); // Envia a distância medida para o TagoIO
}
