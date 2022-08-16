#include <EEPROM.h>
#define POS_ADDR 0
//---------------------------------------------------
#include <ESP8266WiFi.h>
const char *ssid = "HS-Slider";
const char *password = "Sebastian012";
IPAddress localIp(192,168,5,1);
IPAddress gateway(192,168,5,1);
IPAddress subnet(255,255,255,0);
//---------------------------------------------------
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);  //listen to port 80
//---------------------------------------------------
#include <Arduino.h>
#include "BasicStepperDriver.h"
// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 250
#define STEPPER_DIR_PIN 12
#define STEPPER_STEP_PIN 13
#define MICROSTEPS 8
#define MS1_PIN 9
#define MS2_PIN 4
#define MS3_PIN 2
#define STEPPER_EN_PIN 16
BasicStepperDriver stepper(MOTOR_STEPS, STEPPER_DIR_PIN, STEPPER_STEP_PIN);

#define PHOTO_DELAY 100 //time for the ralay to open
#define RELAY_PIN 5
#define AVOID_VIBRATIONS_DELAY 300 //in ms

#define ENDSTOP_PIN 14
#define STEPPER_DIR -1 //-1 or 1 to change direction
#define MAX_POS 440000  //in steps
#define HOMING_SPEED 5


//minified with https://kangax.github.io/html-minifier/
//use option "Case sensitive" and disable "remove optional tags", and "remove tag whitespace"
const char index_html[] PROGMEM = R"rawliteral(<!doctype html><html lang=en><head><script>var width=screen.width,meta=document.createElement("meta");meta.name="viewport",meta.content=width<600?"width=device-width,initial-scale="+width/540:"width=device-width,initial-scale=1",document.getElementsByTagName("head")[0].appendChild(meta)</script><style>.slidercontainer{width:500px;margin:auto}.slidecontainer input{position:relative;width:200px;margin:0 10px 0 10px}input{font-size:18pt!important}.slidecontainer button{width:60px;height:40px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:0}body{font-size:16pt;font-family:sans-serif}.start-stop{width:420px;height:40px;border-radius:20px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:10px 10px}.config{width:200px;height:40px;border-radius:20px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:10px 10px}button:hover{background-color:#0094e2;color:#fff}.heading{text-align:center;font-size:20pt;font-weight:700}td{text-align:center}.slider-l{border-bottom-left-radius:20px;border-top-left-radius:20px}.slider-r{border-bottom-right-radius:20px;border-top-right-radius:20px}#container{width:500px;margin:auto;text-align:center;padding:20px 10px 0 10px}table{margin:auto;padding-top:10px}body,html{margin:0;padding:0;width:100%;height:100%;background-color:#f7f7f7}button{touch-action:manipulation}.slider{width:200px;margin:0 10px}</style></head><body><div id=container><table><tr><td><label for=duration>Duration (s):</label></td><td><input id=duration type=number inputmode=numeric name=duration pattern=[0-9]*></td></tr><tr><td><label for=noPics>No. Pictures:</label></td><td><input id=noPics type=number inputmode=numeric name=noPics pattern=[0-9]*></td></tr></table><hr><div class=slidecontainer><table><tr class=heading><td colspan=3>Start Position</td></tr><tr><td><button class=slider-l id=slider-minus-big-1>-5</button><button class=slider-r id=slider-minus-1>-1</button></td><td><input id=slider-1 type=range class=slider max=100 min=0 value=0></td><td><button class=slider-l id=slider-plus-1>+1</button><button class=slider-r id=slider-plus-big-1>+5</button></td></tr><tr><td></td><td><center><div class=slider-value id=slider-value-1>16</div></center></td><td></td></tr></table></div><hr><div class=slidecontainer><table><tr class=heading><td colspan=3>End Position</td></tr><tr><td><button class=slider-l id=slider-minus-big-2>-5</button><button class=slider-r id=slider-minus-2>-1</button></td><td><input id=slider-2 type=range class=slider max=100 min=0 value=100></td><td><button class=slider-l id=slider-plus-2>+1</button><button class=slider-r id=slider-plus-big-2>+5</button></td></tr><tr><td></td><td><center><div class=slider-value id=slider-value-2>16</div></center></td><td></td></tr></table></div><hr><div class=slidecontainer><table><tr class=heading><td colspan=3>Current Position</td></tr><tr><td><button class=slider-l id=slider-minus-big-3 onclick=goToMinus(10)>-10</button><button class=slider-r id=slider-minus-3 onclick=goToMinus(1)>-1</button></td><td><progress class=slider id=slider-3 max=100 value=32></progress></td><td><button class=slider-l id=slider-plus-3 onclick=goToPlus(1)>+1</button><button class=slider-r id=slider-plus-big-3 onclick=goToPlus(10)>+10</button></td></tr><tr><td></td><td><center><div class=slider-value id=slider-value-3>16</div></center></td><td></td></tr></table></div><hr><table><tr><td><button class=config id=gotostart onclick=goToStart()>Go to Start</button></td><td><button class=config id=gotoend onclick=goToEnd()>Go to End</button></td></tr><tr><td><button class=config id=home onclick=home()>Home</button></td><td><button class=config id=takephoto onclick=takePhoto()>Take Photo</button></td></tr><tr><td colspan=2><button class=start-stop id=start onclick=startJob()>Start</button></td></tr></table></div><script>for(var slider=[],sliderValues=[],sliderC=2,smallIncrement=1,bigIncrement=5,i=0;i<sliderC;++i)slider.push(document.getElementById("slider-"+(i+1))),sliderValues.push(document.getElementById("slider-value-"+(i+1))),sliderValues[i].innerHTML=slider[i].value,function(n){slider[n].oninput=function(e){sliderValues[n].innerHTML=slider[n].value}}(i),function(n){document.getElementById("slider-minus-"+(n+1)).onclick=function(e){slider[n].value=parseFloat(slider[n].value)-smallIncrement,sliderValues[n].innerHTML=slider[n].value}}(i),function(n){document.getElementById("slider-minus-big-"+(n+1)).onclick=function(e){slider[n].value=parseFloat(slider[n].value)-bigIncrement,sliderValues[n].innerHTML=slider[n].value}}(i),function(n){document.getElementById("slider-plus-"+(n+1)).onclick=function(e){slider[n].value=parseFloat(slider[n].value)+smallIncrement,sliderValues[n].innerHTML=slider[n].value}}(i),function(n){document.getElementById("slider-plus-big-"+(n+1)).onclick=function(e){slider[n].value=parseFloat(slider[n].value)+bigIncrement,sliderValues[n].innerHTML=slider[n].value}}(i);function startJob(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString(),""==dur||""==noPics||"0"==dur||"0"==noPics?alert("Duration and Number of Pictures cannot be none!"):window.location.replace("?start&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics)}function home(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var e=new XMLHttpRequest;e.open("GET","?home&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),e.send(),refresh()}function goToStart(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var e=new XMLHttpRequest;e.open("GET","?gotopos="+slider[0].value+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),e.send(),refresh()}function goToEnd(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var e=new XMLHttpRequest;e.open("GET","?gotopos="+slider[1].value+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),e.send(),refresh()}function goToPlus(e){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var n=new XMLHttpRequest;n.open("GET","?gotoplus="+e+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),n.send(),refresh()}function goToMinus(e){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var n=new XMLHttpRequest;n.open("GET","?gotominus="+e+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),n.send(),refresh()}function takePhoto(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var e=new XMLHttpRequest;e.open("GET","?takephoto&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),e.send(),refresh()}function refresh(){var e=new XMLHttpRequest;e.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("slider-3").value=parseFloat(this.responseText),document.getElementById("slider-value-3").innerHTML=parseFloat(this.responseText))},e.open("GET","?currentpos",!0),e.send()}window.setInterval("refresh()",1e3),refresh()</script></body></html>
)rawliteral";

const char status_html[] PROGMEM = R"rawliteral(<!doctypehtml><html lang=en><head><script>var width=screen.width,meta=document.createElement("meta");meta.name="viewport",meta.content=width<600?"width=device-width,initial-scale="+width/540:"width=device-width,initial-scale=1",document.getElementsByTagName("head")[0].appendChild(meta)</script><style>input{font-size:18pt!important}body{font-size:16pt;font-family:sans-serif}.start-stop{width:420px;height:40px;border-radius:20px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:10px 10px}.config{width:200px;height:40px;border-radius:20px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:10px 10px}button:hover{background-color:#0094e2;color:#fff}.heading{text-align:center;font-size:20pt;font-weight:700}td{text-align:center}#container{width:500px;margin:auto;text-align:center;padding:20px 10px 0 10px}table{margin:auto;padding-top:10px}body,html{margin:0;padding:0;width:100%;height:100%;background-color:#f7f7f7}button{touch-action:manipulation}progress{min-width:400px;margin-top:30px;margin-bottom:30px}.start-stop{margin-top:30px}</style></head><body><div id=container><table><tr class=heading><td colspan=2><label for=file>Current Job:</label></td></tr><tr><td colspan=2><progress id=file max=100 value=0></progress></td><td></td></tr><tr><td id=photosRemaining>Img: NaN</td><td id=timeRemaining>Time remaining: NaN</td></tr></table><a href=?stop><button class=start-stop id=start>Stop</button></a></div><script>function refresh(){var e;(e=new XMLHttpRequest).onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("timeRemaining").innerHTML=this.responseText)},e.open("GET","?timeRemaining",!0),e.send(),(e=new XMLHttpRequest).onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("photosRemaining").innerHTML=this.responseText)},e.open("GET","?photosRemaining",!0),e.send(),(e=new XMLHttpRequest).onreadystatechange=function(){4==this.readyState&&200==this.status&&(parseFloat(this.responseText)<-1&&window.location.replace(""),document.getElementById("file").value=parseFloat(this.responseText))},e.open("GET","?percentDone",!0),e.send()}window.setInterval("refresh()",5e3),refresh()</script></body></html>
)rawliteral";



char currentStatus = 'i';
//current status is used to store the curretn page. i=index.html, s=status.html

unsigned int currentPos = 0;
unsigned int currentPhoto = 0;
unsigned int numberPhotos = 1;
unsigned int photoInterval = 1;
unsigned int photoSteps = 0;

unsigned long mainTimer = 0;
unsigned long lastPhotoTime = 0;

unsigned long timeToMove = 0;


void enableStepper(){
  digitalWrite(STEPPER_EN_PIN, LOW);
}
void disableStepper(){
  digitalWrite(STEPPER_EN_PIN, HIGH);
}
void takePhoto(){
  digitalWrite(RELAY_PIN,HIGH);
  delay(PHOTO_DELAY);
  digitalWrite(RELAY_PIN,LOW);
}
char goToPos(int pos,char homing = 0){
  if((pos>MAX_POS+100 || pos<0)&& !homing)
  {
    Serial.println("Invalid move");
    return 1;
  }
  Serial.println("OK");
  enableStepper(); //enalbe stepper
  int steps = pos-currentPos;
  stepper.move(STEPPER_DIR*steps);
  
  currentPos = pos;
  if(!homing){
    EEPROM.put(POS_ADDR, currentPos);
    EEPROM.commit();
    disableStepper();
  }
  
  return 0;
}

void handleRequest(){
  Serial.println("request for / received..");

  if(currentStatus == 'i' && server.hasArg("home")){
    Serial.println("homing.. ");
    server.send ( 200, "text/html", index_html);
    while(!digitalRead(ENDSTOP_PIN)){
      goToPos(currentPos-MOTOR_STEPS*MICROSTEPS*4,1);
    }
    currentPos=0;
    EEPROM.put(POS_ADDR, currentPos);
    EEPROM.commit();
    Serial.println("homed.");
    disableStepper();
  }else if(currentStatus == 'i' && server.hasArg("start") && server.hasArg("pos1") && server.hasArg("pos2") && server.hasArg("duration") && server.hasArg("nopics")){
    Serial.println("starting timelapse.");
    server.send ( 200, "text/html", status_html);

    currentPhoto=0;
    numberPhotos = (server.arg("nopics")).toFloat();
    photoInterval = (server.arg("duration")).toFloat()/(server.arg("nopics")).toFloat()*1000;
    goToPos((int)((float)(server.arg("pos1")).toFloat()/100*MAX_POS));
    photoSteps = (int)((float)(((server.arg("pos2")).toFloat()-(server.arg("pos1")).toFloat())/100*MAX_POS)/numberPhotos);
    currentStatus ='s';

    Serial.print("currentPhoto: ");
    Serial.println(currentPhoto);
    Serial.print("numberPhotos: ");
    Serial.println(numberPhotos);
    Serial.print("photoInterval: ");
    Serial.println(photoInterval);
    Serial.print("currentPos: ");
    Serial.println(currentPos);
    Serial.print("photoSteps: ");
    Serial.println(photoSteps);
  }
  else if(currentStatus == 'i' && server.hasArg("currentpos")){
    Serial.println("sending curretn position update");
    server.send ( 200, "text/html", String(100*(float)currentPos/MAX_POS));
  }
  else if(currentStatus == 'i' && server.hasArg("gotominus")){
    Serial.println("moving in negative direction");
    goToPos(currentPos-(server.arg("gotominus")).toFloat()/100*MAX_POS);
    server.send ( 200, "text/html", String(currentPos));
  }
  else if(currentStatus == 'i' && server.hasArg("gotoplus")){
    Serial.println("moving in positive direction");
    goToPos(currentPos+(server.arg("gotoplus")).toFloat()/100*MAX_POS);
    server.send ( 200, "text/html", String(currentPos));
  }
  else if(currentStatus == 'i' && server.hasArg("gotopos")){
    Serial.println("moving into position");
    goToPos((server.arg("gotopos")).toFloat()/100*MAX_POS);
    server.send ( 200, "text/html", String(currentPos));
  }
  else if(currentStatus == 'i' && server.hasArg("takephoto")){
    Serial.println("taking photo");
    takePhoto();
    server.send ( 200, "text/html", String(currentPos));
  }
  else if(currentStatus == 's' && server.hasArg("percentDone")){
    Serial.println("sending status update");
    server.send ( 200, "text/html", String(100*(float)currentPhoto/numberPhotos));
  }
  else if(currentStatus == 'i' && server.hasArg("percentDone")){
    Serial.println("sending status update, done!");
    server.send ( 200, "text/html", "-2");
  }
  else if(currentStatus == 's' && server.hasArg("photosRemaining")){
    server.send ( 200, "text/html", "Img: " + String(currentPhoto) + "/" + String(numberPhotos));
  }
  else if(currentStatus == 's' && server.hasArg("timeRemaining")){
    server.send ( 200, "text/html", "Time remaining: " + String((int)(photoInterval*(numberPhotos - currentPhoto))/1000) + "s");
  }

  else if(currentStatus == 's' && server.hasArg("stop")){
    Serial.println("stop current job");
    server.send ( 200, "text/html", index_html);
    currentStatus = 'i';
  }
  
  
  else if(currentStatus = 'i'){
    Serial.println("bad arguments. sending index.html.. ");
    server.send ( 200, "text/html", index_html);
  }
  else if(currentStatus = 's'){
    Serial.println("bad arguments. sending status.html.. ");
    server.send ( 200, "text/html", status_html);
  }
}


void setup() {
  Serial.begin(115200); //start the serial output
  Serial.println();
  Serial.println("Starting up");
  EEPROM.begin(sizeof(currentPos));
  EEPROM.get(POS_ADDR, currentPos);
  Serial.print("Curretn Pos: ");
  Serial.println(currentPos);

  pinMode(ENDSTOP_PIN, INPUT);

  //steper motor
  //microstepping
  //pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);
  //digitalWrite(MS1_PIN, HIGH);
  digitalWrite(MS2_PIN, HIGH);
  digitalWrite(MS3_PIN, LOW);
  pinMode(STEPPER_EN_PIN, OUTPUT);
  disableStepper();
  stepper.begin(RPM, MICROSTEPS);

  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN,LOW);
  
  
  Serial.print("Setting Access Point configuration... ");
  WiFi.mode(WIFI_AP_STA);
  
  Serial.println(WiFi.softAPConfig(localIp, gateway, subnet) ? "done" : "failed");
  
  Serial.print("Starting Access Point... ");
  Serial.println(WiFi.softAP(ssid) ? "done" : "failed");

  IPAddress myip = WiFi.softAPIP();
  Serial.print("IP: ");
  Serial.println(myip);

  Serial.println("------------------------------");
  server.on ( "/", handleRequest );
  server.begin(); //start the webserver
  Serial.println("Webserver started");
  
}

void loop() {
  //code to find MAX_POS value
  //int maxsteps=0;
  //while(!digitalRead(ENDSTOP_PIN)){
  //  enableStepper(); //enalbe stepper
  //  maxsteps += 10000;
  //  stepper.move(STEPPER_DIR*-10000);
  //  Serial.println(maxsteps);
  //  delay(500);
  //}
  
  //check if job is running, if so handle timer, movement, and shutter
  if(currentStatus=='s' && currentPhoto>= numberPhotos){
    Serial.println("Job done");
    currentStatus='i';
  }
  else if(currentStatus=='s'){ //job running
    //check if time since last photo is larger than photoInterval
    if(millis() - lastPhotoTime + timeToMove  > photoInterval){
      Serial.println("moving to new location");
      lastPhotoTime=millis();
      //move to position
      timeToMove = millis();
      goToPos(currentPos + photoSteps);
      timeToMove = millis()-timeToMove;
      //delay to avoid vibrations
      Serial.println("vibration delay");
      delay(AVOID_VIBRATIONS_DELAY);
      Serial.println("photo");
      //take photo
      takePhoto();

      //update stats
      ++currentPhoto;
      disableStepper();
    }
  }


  
  server.handleClient();  //process all the requests for the Webserver
}
