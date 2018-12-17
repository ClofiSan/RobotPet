#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LedControl.h>
#include <Servo.h>
/*
pub:NodeToAnd
sub:AndToNode
	PyToNode
*/
//#include "WaveHC.h"
//#include "WaveUtil.h"
/*
2018.9.4
代码和文档的最终确认

*/


const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "119.23.227.254";
int lenthofpy;
WiFiClient espClient;
PubSubClient client(espClient);
//long lastMsg = 0;
//int value = 0;
char msgfromAnd[60];
char msgtoAnd[40];
char msgDeface[40];
byte *msgfromPy;

uint8_t
BeControlled = 0, 
neckcon = 0,
facecon = 0,
Deface = 0;

int U_D = 0 ,
L_R = 0;

#define CLK     D2     
#define CS        D3    
#define DIN       D1      
LedControl lc = LedControl(DIN, CLK, CS, 1);


#define wir1 D7
#define wir2 D8
uint8_t step = 10;
uint8_t neckLR = 90; 
uint8_t neckUD = 90;


byte
sadFace[][8] = {    
	{
		B00100100,        
		B01000010,
		B10000001,
		B00111100,
		B01111110,
		B11111111,
		B11111111,
		B11111111
	},
	{
		B00100100,
		B01000010,
		B10000001,
		B00000000,
		B01111110,
		B11111111,
		B11111111,
		B11111111
	},

	{
		B00100100,
		B01000010,
		B10000001,
		B00000000,
		B00000000,
		B01111110,
		B11111111,
		B11111111 }
		,
	{
		B00100100,
		B01000010,
		B10000001,
		B00000000,
		B00000000,
		B00000000,
		B01111110,
		B11111111
		},
	{
		B00100100,         
		B01000010,
		B10000001,
		B00000000,
		B00000000,
		B00000000,
		B00000000,
		B11111111 }

};

byte
normalFace[][8] = {    
	{
		B00111100,         
		B01111110,
		B11111111,
		B11111111,
		B11111111,
		B11111111,
		B01111110,
		B00111100 }
		,
	{
		B00000000,
		B01111110,
		B11111111,
		B11111111,
		B11111111,
		B11111111,
		B01111110,
		B00111100 }
			,
	{
		B00000000,
		B00000000,
		B00111100,
		B11111111,
		B11111111,
		B11111111,
		B00111100,
		B00000000 }
				,
	{
		B00000000,
		B00000000,
		B00000000,
		B00111100,
		B11111111,
		B01111110,
		B00011000,
		B00000000 }
					,
	{
		B00000000,         
		B00000000,
		B00000000,
		B00000000,
		B10000001,
		B01111110,
		B00000000,
		B00000000 }
};

byte
happyFace[][8] = {    
	{
		B00111100,        
		B01111110,
		B11111111,
		B11111111,
		B11111111,
		B00000000,
		B10000001,
		B01111110 }
		,
	{
		B00000000,
		B01111110,
		B11111111,
		B11111111,
		B11111111,
		B00000000,
		B10000001,
		B01111110 }
			,
	{
		B00000000,
		B00000000,
		B01111110,
		B11111111,
		B11111111,
		B00000000,
		B10000001,
		B01111110 }
				,
	{
		B00000000,
		B00000000,
		B00000000,
		B01111110,
		B11111111,
		B00000000,
		B10000001,
		B01111110 }
		,
	{
		B00000000,             
		B00000000,
		B00000000,
		B01111110,
		B10000001,
		B00000000,
		B10000001,
		B01111110 }
};

byte
botherFace[][8] = {    
	{
		B10000001,        
		B01100110,
		B00000000,
		B11111111,
		B11111111,
		B11111111,
		B01111110,
		B00111100 }
		,
	{
		B10000001,
		B01100110,
		B00000000,
		B11111111,
		B11111111,
		B11111111,
		B01111110,
		B00000000 }
	,
	{
		B10000001,
		B01100110,
		B00000000,
		B11111111,
		B11111111,
		B01111110,
		B00000000,
		B00000000 }
				,
	{
		B10000001,
		B01100110,
		B00000000,
		B11111111,
		B01111110,
		B00000000,
		B00000000,
		B00000000 }
					,
	{
		B10000001,         
		B01100110,
		B00000000,
		B10000001,
		B01111110,
		B00000000,
		B00000000,
		B00000000 }

};
byte heart[8]{

	B00000000,
	B01100110,
	B11111111,
	B11111111,
	B11111111,
	B01111110,
	B00111100,
	B00011000,
};
byte snicker[8]{

	B00000000,
	B11100111,
	B00000000,
	B00000000,
	B11111111,
	B11011011,
	B01011010,
	B00111100,
};
byte happy[8]{
	B0000000,
	B0110011,
	B0110011,
	B0000000,
	B0000000,
	B0100010,
	B0011100,
	B0000000,
};
byte angry[8]{
	B00000000,
	B10000001,
	B01100110,
	B00000000,
	B00011000,
	B00111100,
	B01111110,
	B11111111,

};
byte sad[8]{
	B00000000,
	B00100100,
	B01000010,
	B10000001,
	B00000000,
	B00111100,
	B01000010,
	B10000001,

};


Servo myser;
Servo myser2;


uint8_t
blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, //sizeof = 7 
blinkTime = 100,          
gazeTime = 75,           
gazeMove = 50;            
int8_t
gazeX = 3, gazeY = 3,       
newX = 3, newY = 3,         
dX = 0, dY = 0;


int TouchNum = 0;
int TouchReact = 15;
byte mood = 1;
uint8_t istouch = 0;

long PreMillis = 0;
#define analog A0

const int decay = 30000;        

unsigned long checkMillis, touchMillis, nowMillis;       


void setup()
{   
	Serial.begin(115200);
	
	setup_wifi();
	client.setServer(mqtt_server, 1883);
	client.setCallback(Receive);

	
	pinMode(D6, INPUT);
	randomSeed(analogRead(analog));
	myser.attach(wir1);
	myser2.attach(wir2);



	
	lc.shutdown(0, false);
	lc.setIntensity(0, 2);
	lc.clearDisplay(0);   

				
}

void setup_wifi() {

	delay(10);
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}
void Receive(char* topic, byte* payload, unsigned int length) {

	Serial.print("Message from  [");
	Serial.print(topic);
	Serial.print("] ");
	if (!strcmp(topic, "AndToNode"))
	{
		for (int i = 0; i < length; i++)
		{
			msgfromAnd[i] = payload[i];
		}
		decodeJson(msgfromAnd);
	}
else if (!strcmp(topic,"DeFace")){
    
    for (int i = 0; i < length; i++){
        msgDeface[i] = payload[i];
    }
    decode_DeFace_Json(msgDeface)
  
  }
	else {
		delete msgfromPy;
		msgfromPy = new byte[300];
		int i = 0;
		for (i = 0; i < length; i++) {
			msgfromPy[i] = (byte)payload[i];
			Serial.print(msgfromPy[i]);
			Serial.print(" ");
		}
		lenthofpy = length;
		Serial.println();

	}
}
void decodeJson(char msg[]) {
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(msg);
	BeControlled = root["BeControlled"];
	neckcon = root["neckcon"];
	facecon = root["facecon"];

	Serial.print("BeControlled:");
	Serial.print(BeControlled);
	Serial.print("neckcon:");
	Serial.print(neckcon);
	Serial.print("facecon:");
	Serial.println(facecon);
}
void decode_DeFace_Json(char msg[]){
  //专门用来解析来自人脸识别控制的json
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg);
  U_D = root["U_D"];
  L_R = root["L_R"];
  Deface = root["Deface"];
  Serial.print("Deface");
  Serial.print(Deface);
  Serial.print("L_R");
  Serial.print(L_R);
  Serial.print("U_D");
  Serial.print(U_D);
  
  }
void encodeJson() {
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root1 = jsonBuffer.createObject();
	root1["mood"] = mood;
	root1["TouchReact"] = TouchReact;
	root1.printTo(msgtoAnd);
}
void reconnect() {
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		if (client.connect("ESP8266Client")) {
			Serial.println("connected");
			//client.publish(outTopic, "hello world");
			client.subscribe("AndToNode", 1);
			client.subscribe("PyToNode", 1);
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			delay(5000);
		}
	}
}

void GazeAprh(int8_t x, int8_t y)
{
	
	lc.setLed(0, y, 7 - x, false);
	lc.setLed(0, y + 1, 7 - x, false);
	lc.setLed(0, y, 7 - (x + 1), false);
	lc.setLed(0, y + 1, 7 - (x + 1), false);

}
void SetNewGaze() {
	if (mood == 0)
	{
		newX = random(0, 7);
		newY = random(5, 7);
	}
	else if (mood == 1)
	{
		newX = random(7);
		newY = random(7);
	}
	else if (mood == 2)
	{
		newX = random(7);
		newY = random(4);
	}
	else if (mood == 3)
	{
		newX = random(7);
		newY = random(3, 7);
	}
}
void MoveGaze()
{
	if (--gazeTime <= gazeMove) {
	
		GazeAprh(newX - (dX * gazeTime / gazeMove), newY - (dY * gazeTime / gazeMove));
		if (gazeTime == 0) {
			gazeX = newX;
			gazeY = newY;
			do {
			
				SetNewGaze();
				dX = newX - 3;
				dY = newY - 3;
			} while ((dX * dX + dY * dY) >= 10);

		}
	}
	else {
		GazeAprh(gazeX, gazeY);
   if(Deface==0)
   //没有检测到人脸就传统模式
		MoveNeck();

	}

}
/*void DrawFaceByRow(byte *face)
{
for (int i = 0; i<8; i++)
{
lc.setRow(0, i, face[i]);
}

}*/
void DrawFaceByColumn(byte *face)
{
	for (int i = 0; i<8; i++)
	{
		lc.setColumn(0, i, face[i]);
		//Serial.println("i am drawing");
	}
}
void BlinkFace()
{
	
	lc.clearDisplay(0);
	if (mood == 0)
		DrawFaceByColumn(sadFace[(blinkIndex[JudgeBlinkTime()])]);
	else if (mood == 1)
		DrawFaceByColumn(normalFace[(blinkIndex[JudgeBlinkTime()])]);
	else if (mood == 2)
		DrawFaceByColumn(happyFace[(blinkIndex[JudgeBlinkTime()])]);
	else if (mood == 3)
		DrawFaceByColumn(botherFace[(blinkIndex[JudgeBlinkTime()])]);

	if (--blinkTime == 0) blinkTime = random(5, 180);
}
uint8_t JudgeBlinkTime()
{
	if (blinkTime < sizeof(blinkIndex))
		return blinkIndex[blinkTime];
	else return 0;

}
void MoveNeck()
{
	
	int8_t y2 = 7 - gazeX;
	int8_t x2 = gazeY;
	myser.write(y2 * 10); 
	myser2.write(180 - (x2 * 10));

}
void CheckTouch()
{

	

  istouch  = digitalRead(D6);
	if (istouch)
	{
		TouchNum++;
		if (TouchNum % 200 == 0) {
			TouchReact++;
      
		}
		if (TouchNum >= 1000)
			TouchNum = 0;

	}
	if (TouchReact <= 10) mood = 0;
	else if (TouchReact <= 20) mood = 1;
	else if (TouchReact <= 30) mood = 2;
	else if (TouchReact > 30) mood = 3;

	nowMillis = millis();
	if (nowMillis - PreMillis > decay) {

		PreMillis = nowMillis;
		TouchReact--;
		if (TouchReact < 0) TouchReact = 0;
		if (TouchReact == 30) TouchReact = 15;
		if (TouchReact > 40) TouchReact = 40;
	}
	  Serial.print("touchnum ");
      Serial.print(TouchNum);
      
      Serial.print("mood ");
      Serial.print(mood);
      Serial.print("TouchReact ");
      Serial.println(TouchReact);
}

void Follow_Face(){

      //检测到了人脸
      //首先要判断上下还是左右
      //如何提取出绝对值
      //正常：上下120‘左右，左右90
      //暂时设定成除以10’
    int face_LR=0,face_UD=0;
      //首先左右
      if(L_R>0){
        //左边旋转
        myser.write(90+L_R/10);
        }else {
         myser.write(90+L_R/10);
        }
      if(U_D>0){
        //左边旋转
        myser.write(90+U_D/10);
        }else {
         myser.write(90+U_D/10);
        }
    
       
     
}




void loop()
{
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
	encodeJson();
 //下面这句不能去掉
	client.publish("NodeToAnd", msgtoAnd);
	if (!BeControlled)
	{
		BlinkFace();
		MoveGaze();
   if(Deface==1){
      Follow_Face();
    }
		touchMillis = millis();
		while (millis() - touchMillis < 40)
		{
			istouch = 0;
			CheckTouch();
		}
	}
	else
	{
		control();
	}
	
	
   
	/*if (!client.connected()) {
	reconnect();
	}
	client.loop();
	encodeJson();
	client.publish("NodeToAnd", msgtoAnd);*/

}

void FaceControl(byte *face)
{
	DrawFaceByColumn(face);
	//blinkTime = random(5, 180);
}
void control()
{

	myser.write(neckUD);
	myser2.write(neckLR);


	switch (facecon)
	{
	case 1: {
		// lc.clearDisplay(0);
		DrawFaceByColumn(heart);
		break;
	}case 2: {
		//lc.clearDisplay(0);
		DrawFaceByColumn(happy);
		break;
	}case 3: {
		// lc.clearDisplay(0);
		DrawFaceByColumn(sad);
		break;
	}case 4: {
		// lc.clearDisplay(0);
		DrawFaceByColumn(angry);
		break;
	}case 5: {
		//lc.clearDisplay(0);
		DrawFaceByColumn(snicker);
		break;
	}case 6: {
		//lc.clearDisplay(0);
		WriteWordsColumn();
		break;
	}
	default: {
		LinkUP();
		break;
	}
	}

	if (neckcon)
		controlneck();



}
void controlneck()
{
	//控制舵机
	
	if (neckcon == 1)
		neckLR -= step;
	else if (neckcon == 2)
		neckLR += step;
	if (neckcon == 3)
		neckUD -= step;
	else if (neckcon == 4)
		neckUD += step;

	neckcon = 0;

}
void WriteWordsColumn()
{
	//书写接收到的字，注意衔接
	int n;
	int row = 0;
	for (n = 0; n < lenthofpy; n++)
	{
		row = n % 8;
			delay(100);
		lc.setColumn(0, row, msgfromPy[n]);
	}

	
}
void LinkUP()
{
	//这里原本是用来衔接的
	DrawFaceByColumn(snicker);
}

/*
* print error message and halt if SD I/O error, great for debugging!
*/
/*
void error_P(const char *str) {
//锟斤拷锟揭诧拷锟�
PgmPrint("Error: ");
SerialPrint_P(str);
sdErrorCheck();
while (1);
}
void sdErrorCheck(void) {
//锟斤拷一锟斤拷锟角癸拷锟斤拷Sd锟斤拷锟斤拷锟斤拷锟斤拷
if (!card.errorCode()) return;
PgmPrint("\r\nSD I/O error: ");
Serial.print(card.errorCode(), HEX);
PgmPrint(", ");
Serial.println(card.errorData(), HEX);
while (1);
}
void PlayVoice(FatReader &dir)
{
FatReader file;
while (dir.readDir(dirBuf) > 0) {    //锟斤拷始锟斤拷锟侥硷拷锟斤拷锟斤拷锟斤拷锟叫碉拷锟侥硷拷

// 锟斤拷锟斤拷锟斤拷锟絛ir锟斤拷锟斤拷wav锟斤拷直锟斤拷锟斤拷锟斤拷
if (!DIR_IS_SUBDIR(dirBuf) && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
continue;
}

Serial.println();            //锟铰碉拷一锟斤拷

for (uint8_t i = 0; i < dirLevel; i++) {
Serial.write(' ');       // 为锟剿讹拷锟侥硷拷锟侥斤拷锟斤拷
}
if (!file.open(vol, dirBuf)) {        // 锟斤拷锟斤拷薹锟斤拷锟�
//      error("file.open failed");          // something went wrong
}

if (file.isDir()) {                   // 锟津开碉拷锟角凤拷锟斤拷一锟斤拷锟铰碉拷锟侥硷拷锟斤拷锟捷癸拷锟斤拷募锟�
printEntryName(dirBuf);
Serial.println();
dirLevel += 2;

PlayVoice(file);
dirLevel -= 2;
}
else {
//锟斤拷锟斤拷锟斤拷锟絛ir
printEntryName(dirBuf);
if (!wave.create(file)) {            // 锟叫讹拷锟角凤拷锟斤拷wav
putstring(" Not a valid WAV");     // ok skip it
return;
}
else {
Serial.println();
wave.play();
uint8_t n = 0;
while (wave.isplaying) {
//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷时锟津串匡拷锟斤拷示
putstring(".");
if (!(++n % 32))Serial.println();
delay(100);
}
sdErrorCheck();                    // everything OK?
if (wave.errors)Serial.println(wave.errors);
}
}
}
}
void playcomplete(char *name) {
//锟斤拷锟斤拷锟斤拷锟斤拷一锟斤拷锟斤拷锟斤拷
playfile(name);
while (wave.isplaying) {

}

}
void playfile(char *name) {

if (wave.isplaying) {   //锟斤拷锟斤拷诓锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷幕锟斤拷锟酵ｏ拷锟斤拷锟�
wave.stop();
}

if (!f.open(root, name)) {  //锟斤拷锟斤拷锟绞э拷艿幕锟斤拷锟紾G
Serial.print("can't open file");
Serial.println (name);
return;
}

if (!wave.create(f)) {  //锟叫讹拷锟角凤拷锟斤拷wav
Serial.println(name);
Serial.println("Not a WAV");
}

// 锟斤拷锟斤拷
wave.play();
}*/

 
