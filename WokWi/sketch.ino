#include <ThingerESP32.h>
#include <WiFi.h>
#include <Arduino.h>
#include <DHT.h>
 #include <Wire.h>


#define potenciometro 34    // Pino do potenciometro definido 


#define pinoDHT 17
#define tipoDHT DHT22
 
DHT dht(pinoDHT, tipoDHT);



#define USERNAME "aaaaaaaaana"           // Usuário do Thinger.Io 
#define DEVICE_ID "gs"                   // Id do arduino
#define DEVICE_CREDENTIAL "esp32gs"      // Credencial do arduino
 

#define SSID "Wokwi-GUEST"          // Wifi simulado do próprio dispositivo
#define SSID_PASSWORD ""
 

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);



float consumoEnergia = 0; 
unsigned long ultimoTempo = 0;
float consumoTaxa;           
const float limiteEnergia = 200.0;  
float temp, umid;



void setup() {  

  dht.begin();

  Serial.begin(115200);
  thing.add_wifi(SSID, SSID_PASSWORD);

  pinMode(potenciometro, INPUT);
  pinMode(pinoDHT, OUTPUT);
  digitalWrite(pinoDHT, LOW);
  ultimoTempo = millis();


  // Quando conectar no wifi vai verificar se conectou e manda uma mensagem no log
  WiFi.begin(SSID, SSID_PASSWORD);
  Serial.print("Conectando à rede Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }
  Serial.println("Conectado à rede Wi-Fi!");
 

  thing["Consumo de Energia"] >> outputValue(consumoEnergia);


}



void leituraConsumo() {
  delay(5000);

  int valorPotenciometro = analogRead(potenciometro);
  
  if (valorPotenciometro == 0) {
    Serial.println("Erro: leitura do potenciômetro é zero. Verifique as conexões.");
    return;
  }
  

  float maxValorPot = 4095.0; 
  float maxConsumo = 4095.0; 
  float percentual = valorPotenciometro / maxValorPot; 
  consumoTaxa = percentual * maxConsumo; 
  

  unsigned long tempoAtual = millis();
  float tempoDecorrido = (tempoAtual - ultimoTempo) / 1000.0;  
  

  consumoEnergia += consumoTaxa * tempoDecorrido;

  ultimoTempo = tempoAtual;

  if (consumoEnergia >= limiteEnergia) {
    digitalWrite(pinoDHT, LOW);  
  } else {
    digitalWrite(pinoDHT, HIGH);   
  }
  

  Serial.print("Valor do Potenciômetro: ");
  Serial.print(valorPotenciometro);
  Serial.print(" | Taxa de Consumo: ");
  Serial.print(consumoTaxa, 1);
  Serial.print(" kWh/s | Energia Consumida: ");
  Serial.print(consumoEnergia, 1); 
  Serial.println(" kWh");

  thing["Consumo de Energia"] >> outputValue(consumoEnergia);


}


 
void leituraSensor(){

  temp = dht.readTemperature();
  umid = dht.readHumidity();
  delay(2000);

  Serial.print("Temperatura: ");
  Serial.print(temp);

  Serial.print(" graus - Umidade: ");
  Serial.print(umid);
  Serial.println(" %");

   thing["Temperatura"] >> outputValue(temp);

}
 
void loop() {


  leituraConsumo(); 
  leituraSensor();

  thing.handle();

}
 


