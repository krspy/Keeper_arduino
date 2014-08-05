#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
/*The WiFi shield uses pins 10, 11, 12, and 13 for the SPI connection to the HDG104 module. 
Digital pin 4 is used to control the slave select pin on the SD card.*/

/////wifi , server connect stuff/////////////
int wifi_status = WL_IDLE_STATUS;
char ssid[] = "05"; //  your network SSID (name) 
char pass[] = "wndudgh05";    // your network password (use for WPA, or use as key for WEP)
char send_buff[15]="[ARD_H]Hi";
char recv_buff[20];
int cnt=0;
int alert_mode=LOW;

unsigned int local_port=6609;
IPAddress server(220,85,90,231);  //
unsigned int server_port=6609;

WiFiUDP udp;
//////////wi-fi , server//////////////////////

/////////sensor///////////////
int ledPin = 8;
int pirPin = 2;
int pirState = LOW;
int val;
//////////////////////////////
void setup(){
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin,INPUT);
  Serial.begin(9600);
  
  if(WiFi.status()==WL_NO_SHIELD){   //verify wifi_shield connection
    Serial.print("WiFi shiled not present");
    while(true);
  }
  Serial.print("Firmware ver: ");
  Serial.println(WiFi.firmwareVersion());
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  wifi_status = WiFi.begin(ssid, pass);
  if ( wifi_status != WL_CONNECTED) { 
    Serial.println("setup_Couldn't get a wifi connection");
    // don't do anything else:
    while(wifi_status != WL_CONNECTED){
      Serial.println("Retry to connect");
      wifi_status=WiFi.begin(ssid,pass);
      delay(5000);
    }
  } 
  else
    Serial.println("Connected to wifi");
  int portup=udp.begin(local_port);
  Serial.print("Starting connection to srv: ");
  Serial.println(portup);
  
}
void loop(){  
  if ( wifi_status != WL_CONNECTED) { 
    Serial.println("loop_Couldn't get a wifi connection");
    // don't do anything else:
    while(wifi_status != WL_CONNECTED){
      Serial.println("Retry to connect");
      wifi_status=WiFi.begin(ssid,pass);
      delay(5000);
    }
  }
  else
    Serial.println("wifi connected");
  Serial.print("==="); 
  Serial.print(cnt);
  Serial.println(" step===");
  
  if(alert_mode){
    memset(send_buff,0,sizeof(send_buff));
    recvPack();
    val=digitalRead(pirPin);
    if(val == HIGH){
      digitalWrite(ledPin, HIGH);
      Serial.println("!@#$!^%#$&$&^!@Motion alert!@#$!@#$");
      strcpy(send_buff,"[ARD_D]Detect");
      sendPack();
      val=0;
      /*if(pirState == LOW){
        Serial.println("Motion alert");
        pirState = HIGH;
        strcpy(send_buff,"[ARD_D]Detect");
        sendPack();
      }*/
    }
    else{
      digitalWrite(ledPin, LOW);
      strcpy(send_buff,"[ARD_N]Nope");
      sendPack();
      /*if(pirState == HIGH){
        Serial.println("Motion end");
        pirState = LOW;
        strcpy(send_buff,"[ARD_H]Hi");
        sendPack();
      }*/
    }
  }
  else{
    memset(send_buff,0,sizeof(send_buff));
    strcpy(send_buff,"[ARD_H]Hi");
    recvPack();
    sendPack(); 
    delay(5000);
  }  
  cnt++;
  Serial.println("");
  printWifiStatus();
  Serial.println("");
}

void sendPack(){
  int beganPack=udp.beginPacket(server,server_port);
  int sentByte=udp.write(send_buff);
  int endPack=udp.endPacket();
  Serial.print("Began packet: ");
  Serial.println(beganPack);
  Serial.print("sent byte: ");
  Serial.println(sentByte);
  Serial.print("sent success: ");
  Serial.println(endPack);
}
void recvPack(){
  memset(recv_buff,0,sizeof(recv_buff));
  int size_p=udp.parsePacket();
  int len=udp.read(recv_buff,20);
  if(len>0) recv_buff[len]=0;
  Serial.print(len);
  Serial.print("      ");
  Serial.print(size_p);
  Serial.print("  Server say: ");
  Serial.println(recv_buff);
  
  if(recv_buff[0]=='[' && recv_buff[1]=='S' && recv_buff[2]=='V' 
  && recv_buff[3]=='R' && recv_buff[4]==']' && recv_buff[5]=='O'
  && recv_buff[6]=='N'){
    Serial.println("Watcher On");
    alert_mode=HIGH;
  }
  if(recv_buff[0]=='[' && recv_buff[1]=='S' && recv_buff[2]=='V' 
  && recv_buff[3]=='R' && recv_buff[4]==']' && recv_buff[5]=='O'
  && recv_buff[6]=='F' && recv_buff[7]=='F'){
    Serial.println("Watcher Off");
    alert_mode=LOW;
  }
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
void startWatcher(int flag){
}
