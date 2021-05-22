#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "fir-autolamp-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "VE2jIvoD26ItKYuY4rXx4Tkj5H0c1Ae2YWnfrAP0"
#define WIFISSID "your-wifi-ssid" //SSID WiFi
#define PASSWORD "your-wifi-password" //Password WiFi 

#define relay 4 //inisialisasi relay sebagai 4(pin relay di D2)
#define pir 5 //inisialisasi pir sebagai 5(pin pir di D1)
#define ldr A0 //inisialisasi ldr sebagai A0(pin dari ldr)
int statusldr = 0; //inisialisasi variabel pembacaan ldr
int statuspir = 0; //inisialisasi variabel pembacaan pir
String statusled;


void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  pinMode(relay, OUTPUT); //relay(pin 4) sebagai output
  pinMode(pir, INPUT); //pir(pin 2) sebagai input
  pinMode(ldr, INPUT); //ldr(pin A0) sebagai input
}

void loop() {

  digitalWrite(relay, HIGH); //set relay high(lampu mati)
  statusldr = analogRead(ldr); //membaca data ldr
  statuspir = digitalRead(pir); //membaca data pir
  //dibawah ini code untuk konversi data ldr ke lux
  float Vout = statusldr * (3.3 / 675); //Vout = inputLDR * (Vin / ADC_maks)
  float RLDR = (1000 * (3.3 - Vout)) / Vout; //RLDR = (Resistor * (Vin - Vout)) / Vout
  statusldr = 500 / (RLDR*0.001); //Lux = 500 / (RLDR * 0.001)

  if(statuspir == HIGH && statusldr < 200){ //cek kondisi jika ada gerakan(statuspir == HIGH) dan cahaya kurang(statusldr < 200)
    Serial.printf("Lampu Menyala\n"); //output "Lampu Menyala" pada serial
    digitalWrite(relay, LOW); //set relay low(lampu menyala)
    statusled = "HIDUP";
    delay(300000); //delay 300000 ms / 300 second / 5 minute
  }
  else{ //jika kondisi diatas tidak terpenuhi
    Serial.printf("Status PIR : %d\n",statuspir); //output nilai pir pada serial
    Serial.printf("Status LDR : %d Lux\n",statusldr); //output nilai ldr pada serial
    statusled = "MATI";
  }

  Firebase.setString("LED",statusled);
  if (Firebase.failed()) {
    Serial.print("setting /number failed:");
    Serial.println(Firebase.error());  
    return;
  }
  delay(1000); //memberi jeda antar pembacaan sensor
}
