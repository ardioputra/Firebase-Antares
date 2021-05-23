#include <AntaresESP8266MQTT.h>

#define ACCESSKEY "7dc57000dea4a3d5:83e96f357b69d921" //sesuai antares
#define WIFISSID "1728" //SSID WiFi
#define PASSWORD "duadelapann" //Password WiFi 
#define projectName "AutomaticLamp" //sesuai antares
#define deviceName "AutomaticLamp" //sesuai antares

#define relay 4 //inisialisasi relay sebagai 4(pin relay di D2)
#define pir 5 //inisialisasi pir sebagai 5(pin pir di D1)
#define ldr A0 //inisialisasi ldr sebagai A0(pin dari ldr)

AntaresESP8266MQTT antares(ACCESSKEY);
int statusldr = 0; //inisialisasi variabel pembacaan ldr
int statuspir = 0; //inisialisasi variabel pembacaan pir

void setup() {
  Serial.begin(115200);
  antares.setDebug(true);
  antares.wifiConnection(WIFISSID,PASSWORD);
  antares.setMqttServer();
  pinMode(relay, OUTPUT); //relay(pin 4) sebagai output
  pinMode(pir, INPUT); //pir(pin 2) sebagai input
  pinMode(ldr, INPUT); //ldr(pin A0) sebagai input
  Serial.begin(115200); //inisialisasi serial dengan 115200 baud
}

void loop() {
  antares.checkMqttConnection();
  digitalWrite(relay, HIGH); //set relay high(lampu mati)
  statusldr = analogRead(ldr); //membaca data ldr
  statuspir = digitalRead(pir); //membaca data pir
  //dibawah ini code untuk konversi data ldr ke lux
  float Vout = statusldr * (3.3 / 675); //Vout = inputLDR * (Vin / ADC_maks)
  float RLDR = (1000 * (3.3 - Vout)) / Vout; //RLDR = (Resistor * (Vin - Vout)) / Vout
  statusldr = 500 / (RLDR*0.001); //Lux = 500 / (RLDR * 0.001)

  antares.add("ldr", statusldr);
  antares.add("pir", statuspir);
  
  if(statuspir == HIGH && statusldr < 200){ //cek kondisi jika ada gerakan(statuspir == HIGH) dan cahaya kurang(statusldr < 200)
    Serial.printf("Lampu Menyala\n"); //output "Lampu Menyala" pada serial
    digitalWrite(relay, LOW); //set relay low(lampu menyala)
    antares.add("led", "Lampu Menyala!");
    antares.publish(projectName, deviceName);
    delay(300000); //delay 300000 ms / 300 second / 5 minute
  }
  else{ //jika kondisi diatas tidak terpenuhi
    Serial.printf("Status PIR : %d\n",statuspir); //output nilai pir pada serial
    Serial.printf("Status LDR : %d Lux\n",statusldr); //output nilai ldr pada serial
    antares.add("led", "Lampu Mati");
    antares.publish(projectName, deviceName);
  }
  
  delay(1000); //memberi jeda antar pembacaan sensor
}
