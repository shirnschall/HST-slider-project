
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
//---------------------------------------------------
#include <ESP8266WiFi.h>
const char *ssid = "HS-Slider";
const char *password = "Sebastian012";
IPAddress localIp(192,168,5,1);
IPAddress gateway(192,168,5,1);
IPAddress subnet(255,255,255,0);
//---------------------------------------------------
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);  //listen to port 80
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


const char index_html[] PROGMEM = R"rawliteral(<!doctype html>
<html lang="en">
    <head>
        <script type="text/javascript">
        var width = screen.width;
        var meta = document.createElement('meta');
            meta.name = "viewport";
            
        if (width < 600) { //the device width is smaller than the min with of body
            meta.content = "width=device-width,initial-scale=" + width / 540;
            document.getElementsByTagName("head")[0].appendChild(meta);
        } else {
            meta.content = "width=device-width,initial-scale=1";
            document.getElementsByTagName("head")[0].appendChild(meta);
        }
        </script>
        
        <style>
            .slidercontainer{
                width: 500px;
                margin: auto;
            }
            
            .slidecontainer input {
                position: relative;
                width: 200px;
                margin: 0px 10px 0px 10px;
            }
            input{
                font-size: 18pt !important;
            }
            .slidecontainer button{
                  width: 60px;
                  height: 40px;
                  background: white;
                  font-size: 24px;
                  border: 1px solid lightgrey;
                  cursor: pointer;
                  -webkit-appearance: none;
                margin: 0px;
            }
            body{
                font-size: 16pt;
                font-family: sans-serif;
            }
            .start-stop{
                  width: 420px;
                  height: 40px;
                  border-radius: 20px;
                  background: white;
                  font-size: 24px;
                  border: 1px solid lightgrey;
                  cursor: pointer;
                  -webkit-appearance: none;
                  margin: 10px 10px;
            }
            .config{
                  width: 200px;
                  height: 40px;
                  border-radius: 20px;
                  background: white;
                  font-size: 24px;
                  border: 1px solid lightgrey;
                  cursor: pointer;
                  -webkit-appearance: none;
                  margin: 10px 10px;
            }
            button:hover{
                background-color: #0094e2;
                color: #fff;
            }
            .heading{
                text-align: center;
                font-size: 20pt;
                font-weight: bold;
            }
            td{
                text-align: center;
            }
            .slider-l{
                border-bottom-left-radius: 20px;
                border-top-left-radius: 20px;
            }
            .slider-r{
                border-bottom-right-radius: 20px;
                border-top-right-radius: 20px;
            }
            #container{
                width: 500px;
                margin: auto;
                text-align: center;
                padding: 20px 10px 0px 10px;
            }
            table{
                /*border: 1px solid black;*/
                margin: auto;
                padding-top: 10px;
            }
            html, body{
                margin: 0px;
                padding:0px;
                width: 100%;
                height: 100%;
                background-color: #f7f7f7;
            }
            button{
                touch-action: manipulation;
            }
            .slider{
                width: 200px;
                margin: 0px 10px;
            }
            
            
        </style>
        
    </head>
    <body>
        <div id="container">
            <table>
                <tr>
                    <td><label for="duration">Duration (s):</label></td>
                    <td><input type="number" inputmode="numeric" pattern="[0-9]*" id="duration" name="duration"></td>
                </tr>
                <tr>
                <td>
            <label for="noPics">No. Pictures:</label></td>
                <td>
            <input type="number" inputmode="numeric" pattern="[0-9]*" id="noPics" name="noPics"></td>
                </tr>
            </table>
            
            <hr>
        <div class="slidecontainer">
            <table>
                
                <tr class="heading"><td colspan="3">Start Position</td></tr>
                <tr>
                <td><button class="slider-l" id="slider-minus-big-1">-5</button><button class="slider-r" id="slider-minus-1">-1</button></td>
                <td><input type="range" min="0" max="100" value="0" class="slider" id="slider-1"></td>
                    <td><button class="slider-l" id="slider-plus-1">+1</button><button class="slider-r" id="slider-plus-big-1">+5</button></td>
                </tr>
                <tr>
                    <td></td>
                    <td><center><div class="slider-value" id="slider-value-1">16</div></center></td>
                
                    <td></td>
                </tr>
            </table>
        </div>
        
        
            <hr>
        
        <div class="slidecontainer">
            <table>
                <tr class="heading"><td colspan="3">End Position</td></tr>
                <tr>
                <td><button class="slider-l" id="slider-minus-big-2">-5</button><button class="slider-r" id="slider-minus-2">-1</button></td>
                <td><input type="range" min="0" max="100" value="100" class="slider" id="slider-2"></td>
                <td><button class="slider-l" id="slider-plus-2">+1</button><button class="slider-r" id="slider-plus-big-2">+5</button></td>
                </tr>
                <tr>
                    <td></td>
                    <td><center><div class="slider-value" id="slider-value-2">16</div></center></td>
                    <td></td>
                </tr>
            </table>
        </div>
        
            <hr>
            <div class="slidecontainer">
                <table>
                    <tr class="heading"><td colspan="3">Current Position</td></tr>
                    <tr>
                    <td><button class="slider-l" id="slider-minus-big-3" onclick="goToMinus(10)">-10</button><button class="slider-r" id="slider-minus-3" onclick="goToMinus(1)">-1</button></td>
                    <td><progress id="slider-3" value="32" max="100" class="slider"> </progress></td>
                    <td><button class="slider-l" id="slider-plus-3" onclick="goToPlus(1)">+1</button><button class="slider-r" id="slider-plus-big-3" onclick="goToPlus(10)">+10</button></td>
                    </tr>
                    <tr>
                        <td></td>
                        <td><center><div class="slider-value" id="slider-value-3">16</div></center></td>
                        <td></td>
                    </tr>
                </table>
            </div>
        
            <hr>
        <table>
            <tr>
                <td><button class="config" id="gotostart"  onclick="goToStart()">Go to Start</button></td>
                <td><button class="config" id="gotoend"  onclick="goToEnd()">Go to End</button></td>
            </tr>
            <tr>
                <td  colspan="2"><button class="start-stop" id="home" onclick="home()">Home</button></td>
            </tr>
            <tr>
                <td colspan="2"><button class="start-stop" id="start" onclick="startJob()">Start</button></td>
            </tr>
        </table>
            
            
        </div>
        <script>
            var slider = [];
            var sliderValues =[];
            var sliderC=2;
            var smallIncrement = 1;
            var bigIncrement = 5;
            
            for(var i=0;i<sliderC;++i){
                slider.push(document.getElementById("slider-"+(i+1)));
                sliderValues.push(document.getElementById("slider-value-"+(i+1)));
                
                //update values on slider input
                sliderValues[i].innerHTML = slider[i].value;
                (function(i) {
                slider[i].oninput = function(e) {
                    sliderValues[i].innerHTML = slider[i].value;
                }
                })(i);
                
                //update slider values on buttonclick
                (function(i) {
                document.getElementById("slider-minus-"+(i+1)).onclick = function(e) {
                    slider[i].value = parseFloat(slider[i].value)-smallIncrement;
                    sliderValues[i].innerHTML = slider[i].value;
                }
                })(i);
                (function(i) {
                document.getElementById("slider-minus-big-"+(i+1)).onclick = function(e) {
                    slider[i].value = parseFloat(slider[i].value)-bigIncrement;
                    sliderValues[i].innerHTML = slider[i].value;
                }
                })(i);
                (function(i) {
                document.getElementById("slider-plus-"+(i+1)).onclick = function(e) {
                    slider[i].value = parseFloat(slider[i].value) + smallIncrement;
                    sliderValues[i].innerHTML = slider[i].value;
                }
                })(i);
                (function(i) {
                document.getElementById("slider-plus-big-"+(i+1)).onclick = function(e) {
                    slider[i].value = parseFloat(slider[i].value)+bigIncrement;
                    sliderValues[i].innerHTML = slider[i].value;
                }
                })(i);
            }
            
            
            function startJob(){
                dur = document.getElementById("duration").value.toString();
                noPics = document.getElementById("noPics").value.toString();
                if(dur =="" || noPics=="" || dur =="0" || noPics=="0")
                    alert("Duration and Number of Pictures cannot be none!");
                else
                window.location.replace("?start"+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics);
            }
            function home(){
                dur = document.getElementById("duration").value.toString();
                noPics = document.getElementById("noPics").value.toString();
                window.location.replace("?home"+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics);
            }
            function goToStart(){
                dur = document.getElementById("duration").value.toString();
                noPics = document.getElementById("noPics").value.toString();
                window.location.replace("?gotopos="+slider[0].value+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics);
            }
            function goToEnd(){
                dur = document.getElementById("duration").value.toString();
                noPics = document.getElementById("noPics").value.toString();
                window.location.replace("?gotopos="+slider[1].value+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics);
            }
            function goToPlus(delta){
                dur = document.getElementById("duration").value.toString();
                noPics = document.getElementById("noPics").value.toString();
                window.location.replace("?gotoplus="+delta+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics);
            }
            function goToMinus(delta){
                dur = document.getElementById("duration").value.toString();
                noPics = document.getElementById("noPics").value.toString();
                window.location.replace("?gotominus="+delta+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics);
            }

            function refresh() {
                    var xhr = new XMLHttpRequest();
                    xhr.onreadystatechange = function() {
                        if (this.readyState == 4 && this.status == 200) {
                            document.getElementById("slider-3").value= parseFloat(this.responseText); 
                            document.getElementById("slider-value-3").innerHTML = parseFloat(this.responseText);
                        }
                    }; 
                    xhr.open("GET", "?currentpos", true);
                    xhr.send();
            }
            refresh();
        </script>
    </body>
</html>

)rawliteral";

const char status_html[] PROGMEM = R"rawliteral(<!doctype html>
<html lang="en">
    <head>
        <script type="text/javascript">
        var width = screen.width;
        var meta = document.createElement('meta');
            meta.name = "viewport";
            
        if (width < 600) { 
            meta.content = "width=device-width,initial-scale=" + width / 540;
            document.getElementsByTagName("head")[0].appendChild(meta);
        } else {
            meta.content = "width=device-width,initial-scale=1";
            document.getElementsByTagName("head")[0].appendChild(meta);
        }
        </script>
        
        <style>
            input{
                font-size: 18pt !important;
            }
            body{
                font-size: 16pt;
                font-family: sans-serif;
            }
            .start-stop{
                  width: 420px;
                  height: 40px;
                  border-radius: 20px;
                  background: white;
                  font-size: 24px;
                  border: 1px solid lightgrey;
                  cursor: pointer;
                  -webkit-appearance: none;
                  margin: 10px 10px;
            }
            .config{
                  width: 200px;
                  height: 40px;
                  border-radius: 20px;
                  background: white;
                  font-size: 24px;
                  border: 1px solid lightgrey;
                  cursor: pointer;
                  -webkit-appearance: none;
                  margin: 10px 10px;
            }
            button:hover{
                background-color: #0094e2;
                color: #fff;
            }
            .heading{
                text-align: center;
                font-size: 20pt;
                font-weight: bold;
            }
            td{
                text-align: center;
            }
            #container{
                width: 500px;
                margin: auto;
                text-align: center;
                padding: 20px 10px 0px 10px;
            }
            table{
                /*border: 1px solid black;*/
                margin: auto;
                padding-top: 10px;
            }
            html, body{
                margin: 0px;
                padding:0px;
                width: 100%;
                height: 100%;
                background-color: #f7f7f7;
            }
            button{
                touch-action: manipulation;
            }
            progress{
                min-width: 400px;
                margin-top: 30px;
                margin-bottom: 30px;
            }
            .start-stop{
                margin-top: 30px;
            }
            
        </style>
        
    </head>
    <body>
        <div id="container">
            <table>
                <tr class="heading">
                    <td colspan="2"><label for="file">Current Job:</label></td>
                </tr>
                <tr>
                <td colspan="2"><progress id="file" value="32" max="100"> </progress><td>
                </tr>
                <tr>
                    <td id="photosRemaining">Img: 20/600</td>
                    <td id="timeRemaining">Time remaining: 7000s</td>                </tr>            </table>            
            <a href="?stop"><button class="start-stop" id="start">Stop</button></a>
            
        </div>
        <script>
            window.setInterval('refresh()', 5000);   

            function refresh() {
                    var xhr = new XMLHttpRequest();
                    xhr.onreadystatechange = function() {
                        if (this.readyState == 4 && this.status == 200) {
                            document.getElementById("timeRemaining").innerHTML= this.responseText;
                        }
                    }; 
                    xhr.open("GET", "?timeRemaining", true);
                    xhr.send();

                    var xhr = new XMLHttpRequest();
                    xhr.onreadystatechange = function() {
                        if (this.readyState == 4 && this.status == 200) {
                            document.getElementById("photosRemaining").innerHTML= this.responseText;  
                        }
                    }; 

                    xhr.open("GET", "?photosRemaining", true);
                    xhr.send();
                    var xhr = new XMLHttpRequest();
                    xhr.onreadystatechange = function() {
                        if (this.readyState == 4 && this.status == 200) {
                            document.getElementById("file").value= parseFloat(this.responseText); 
                        }
                    }; 
                    xhr.open("GET", "?percentDone", true);
                    xhr.send();
            }
            refresh();
        </script>
    </body>
</html>

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
  currentPos = pos;

  stepper.move(STEPPER_DIR*steps);
  
  return 0;
}


// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  if(var == "inputString"){
    return "test";
  }
  return String();
}

void handleRequest(AsyncWebServerRequest *request){
  Serial.println("request for / received..");

  if(currentStatus == 'i' && request->hasParam("home")){
    Serial.println("homing.. ");
    request->send ( 200, "text/html", index_html);
    while(!digitalRead(ENDSTOP_PIN)){
      goToPos(currentPos-MOTOR_STEPS*MICROSTEPS*4,1);
    }
    currentPos=0;
    Serial.println("homed.");
    disableStepper();
  }else if(currentStatus == 'i' && request->hasParam("start") && request->hasParam("pos1") && request->hasParam("pos2") && request->hasParam("duration") && request->hasParam("nopics")){
    Serial.println("starting timelapse.");
    request->send ( 200, "text/html", status_html);

    currentPhoto=0;
    numberPhotos = (request->getParam("nopics")->value()).toFloat();
    photoInterval = (request->getParam("duration")->value()).toFloat()/(request->getParam("nopics")->value()).toFloat()*1000;
    goToPos((int)((float)(request->getParam("pos1")->value()).toFloat()/100*MAX_POS));
    photoSteps = (int)((float)(((request->getParam("pos2")->value()).toFloat()-(request->getParam("pos1")->value()).toFloat())/100*MAX_POS)/numberPhotos);
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
  else if(currentStatus == 'i' && request->hasParam("currentpos")){
    Serial.println("sending curretn position update");
    request->send ( 200, "text/html", String(100*(float)currentPos/MAX_POS));
  }
  else if(currentStatus == 'i' && request->hasParam("gotominus")){
    Serial.println("moving in negative direction");
    int delta = (request->getParam("gotominus")->value()).toInt();
    goToPos(currentPos-delta*MICROSTEPS*4);
    request->send_P ( 200, "text/html", index_html, processor);
  }
  else if(currentStatus == 'i' && request->hasParam("gotoplus")){
    Serial.println("moving in positive direction");
    int delta = (request->getParam("gotominus")->value()).toInt();
    goToPos(currentPos+delta*MICROSTEPS*4);
    request->send ( 200, "text/html", index_html);
  }
  else if(currentStatus == 's' && request->hasParam("percentDone")){
    Serial.println("sending status update");
    request->send ( 200, "text/html", String(100*(float)currentPhoto/numberPhotos));
  }
  else if(currentStatus == 's' && request->hasParam("photosRemaining")){
    request->send ( 200, "text/html", "Img: " + String(currentPhoto) + "/" + String(numberPhotos));
  }
  else if(currentStatus == 's' && request->hasParam("timeRemaining")){
    request->send ( 200, "text/html", "Time remaining: " + String((int)(photoInterval*(numberPhotos - currentPhoto))/1000) + "s");
  }

  else if(currentStatus == 's' && request->hasParam("stop")){
    Serial.println("stop current job");
    request->send ( 200, "text/html", index_html);
    currentStatus = 'i';
  }
  
  
  else if(currentStatus = 'i'){
    Serial.println("bad arguments. sending index.html.. ");
    request->send ( 200, "text/html", index_html);
  }
  else if(currentStatus = 's'){
    Serial.println("bad arguments. sending status.html.. ");
    request->send ( 200, "text/html", status_html);
  }
}


void setup() {
  Serial.begin(115200); //start the serial output
  Serial.println();
  Serial.println("Starting up");

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
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    handleRequest(request);
  });



  
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
}
