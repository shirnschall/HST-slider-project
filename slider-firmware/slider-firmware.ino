#include <EEPROM.h>
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
//#define motorRpm 250
#define stepperDir_PIN 12
#define STEPPER_STEP_PIN 13
#define MICROSTEPS 8
#define MS1_PIN 9
#define MS2_PIN 4
#define MS3_PIN 2
#define STEPPER_EN_PIN 16
BasicStepperDriver stepper(MOTOR_STEPS, stepperDir_PIN, STEPPER_STEP_PIN);

//#define shutterDelay 100 //time for the ralay to open
#define RELAY_PIN 5
//#define vibrationDelay 300 //in ms

#define ENDSTOP_PIN 14
//#define stepperDir -1 //-1 or 1 to change direction
//#define maxPos 440000  //in steps
#define HOMING_SPEED 5


//minified with https://kangax.github.io/html-minifier/
//use option "Case sensitive" and disable "remove optional tags", and "remove tag whitespace"
const char index_html[] PROGMEM = R"rawliteral(<!doctype html><html lang="en"><head><link rel="manifest" href="/manifest.webmanifest"><script type="text/javascript">var width=screen.width,meta=document.createElement("meta");meta.name="viewport",meta.content=width<600?"width=device-width,initial-scale="+width/540:"width=device-width,initial-scale=1",document.getElementsByTagName("head")[0].appendChild(meta)</script><style>.slidecontainer{margin:auto;transition:color .2s ease-in-out}.slidecontainer input{position:relative;width:200px;margin:0 10px 0 10px}input{font-size:18pt!important}.slidecontainer button{width:60px;height:40px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:0}body{font-size:16pt;font-family:sans-serif}.start-stop{width:420px;height:40px;border-radius:20px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:10px 10px}.config{width:200px;height:40px;border-radius:20px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:10px 10px}button:hover:enabled{background-color:#0094e2;color:#fff}.heading{text-align:center;font-size:20pt;font-weight:700}td{text-align:center}.slider-l{border-bottom-left-radius:20px;border-top-left-radius:20px}.slider-r{border-bottom-right-radius:20px;border-top-right-radius:20px}#container{width:500px;margin:auto;text-align:center;padding:20px 10px 0 10px}table{margin:auto;padding-top:10px}body,html{margin:0;padding:0;width:100%;height:100%;background-color:#f7f7f7}button{touch-action:manipulation}.slider{width:200px;margin:0 10px}.tab{margin-top:20.667px;padding:0;border-radius:0 0 15px 15px;overflow:hidden;border:1px solid #ccc;background-color:#f1f1f1}.tab button{margin:0;font-size:16pt;background-color:inherit;float:left;border:none;outline:0;cursor:pointer;padding:14px 16px;transition:.3s;font-weight:700}.tab button:hover{background-color:#ddd;color:#000}.tab button.active{background-color:#0094e2;color:#fff}.tabcontent{display:none;padding:6px 0;border-top:none;position:relative}.buttonTableContainer{position:absolute;bottom:-20px;width:100%}.buttonTable{width:450px;margin:25px;margin-top:0}.alignL{text-align:left}hr{border:1px solid #ccc}</style></head><body><div id="container"><div id="setup" class="tabcontent"><div class="slidecontainer"><table><tr class="heading"><td colspan="3">Start Position</td></tr><tr><td><button class="slider-l" id="slider-minus-big-1">-5</button><button class="slider-r" id="slider-minus-1">-1</button></td><td><input type="range" min="0" max="100" value="0" class="slider" id="slider-1"></td><td><button class="slider-l" id="slider-plus-1">+1</button><button class="slider-r" id="slider-plus-big-1">+5</button></td></tr><tr><td></td><td><center><div class="slider-value" id="slider-value-1">16</div></center></td><td></td></tr></table></div><hr><div class="slidecontainer"><table><tr class="heading"><td colspan="3">End Position</td></tr><tr><td><button class="slider-l" id="slider-minus-big-2">-5</button><button class="slider-r" id="slider-minus-2">-1</button></td><td><input type="range" min="0" max="100" value="100" class="slider" id="slider-2"></td><td><button class="slider-l" id="slider-plus-2">+1</button><button class="slider-r" id="slider-plus-big-2">+5</button></td></tr><tr><td></td><td><center><div class="slider-value" id="slider-value-2">16</div></center></td><td></td></tr></table></div><hr><div class="slidecontainer"><table><tr class="heading"><td colspan="3">Current Position</td></tr><tr><td><button class="slider-l" id="slider-minus-big-3" onclick="goToMinus(10)">-10</button><button class="slider-r" id="slider-minus-3" onclick="goToMinus(1)">-1</button></td><td><progress id="slider-3" value="32" max="100" class="slider"></progress></td><td><button class="slider-l" id="slider-plus-3" onclick="goToPlus(1)">+1</button><button class="slider-r" id="slider-plus-big-3" onclick="goToPlus(10)">+10</button></td></tr><tr><td></td><td><center><div class="slider-value" id="slider-value-3">16</div></center></td><td></td></tr></table></div><hr><table><tr><td><button class="config" id="gotostart" onclick="goToStart()">Go to Start</button></td><td><button class="config" id="gotoend" onclick="goToEnd()">Go to End</button></td></tr><tr><td><button class="config" id="home" onclick="home()">Home</button></td><td><button class="config" id="takephoto" onclick="takePhoto()">Take Photo</button></td></tr></table></div><div id="timelapse" class="tabcontent"><div class="buttonTableContainer"><table><tr><td><label for="duration">Duration (s):</label></td><td><input type="number" inputmode="numeric" pattern="[0-9]*" id="duration" name="duration"></td></tr><tr><td><label for="noPics">No. Pictures:</label></td><td><input type="number" inputmode="numeric" pattern="[0-9]*" id="noPics" name="noPics"></td></tr></table><hr><table class="buttonTable"><tr><td colspan="2"><button class="start-stop" id="start" onclick="startJob()">Start</button></td></tr></table></div></div><div id="misc" class="tabcontent" style="color:rgba(16,16,16,.3)"><table><tr class="heading"><td colspan="3">Motion Type</td></tr><tr><td class="alignL"><input type="radio" name="motionType" checked value="linear" id="linear" onchange="updateAuxiliarySliders(-1)" disabled><label for="linear">Constant</label></td><td></td><td></td></tr><tr><td class="alignL"><input type="radio" name="motionType" value="easeInOut" id="easeInOut" onchange="updateAuxiliarySliders(2)" disabled><label for="easeInOut">Ease in/out</label></td><td class="alignL"><input type="radio" name="motionType" value="easeIn" id="easeIn" onchange="updateAuxiliarySliders(0)" disabled><label for="easeIn">Ease in</label></td><td class="alignL"><input type="radio" name="motionType" value="easeOut" id="easeOut" onchange="updateAuxiliarySliders(1)" disabled><label for="easeOut">Ease out</label></td></tr><tr><td class="alignL"><input type="radio" name="motionType" value="rampInOut" id="rampInOut" onchange="updateAuxiliarySliders(2)" disabled><label for="rampInOut">Ramp in/out</label></td><td class="alignL"><input type="radio" name="motionType" value="rampIn" id="rampIn" onchange="updateAuxiliarySliders(0)" disabled><label for="rampIn">Ramp in</label></td><td class="alignL"><input type="radio" name="motionType" value="rampOut" id="rampOut" onchange="updateAuxiliarySliders(1)" disabled><label for="rampOut">Ramp out</label></td></tr></table><hr><div class="slidecontainer" id="auxiliarySliderContainer0"><table><tr class="heading"><td colspan="3">Auxiliary Point (in)</td></tr><tr><td><button class="slider-l" id="slider-minus-big-4">-5</button><button class="slider-r" id="slider-minus-4">-1</button></td><td><input type="range" min="0" max="100" value="20" class="slider" id="slider-4"></td><td><button class="slider-l" id="slider-plus-4">+1</button><button class="slider-r" id="slider-plus-big-4">+5</button></td></tr><tr><td></td><td><center><div class="slider-value" id="slider-value-4">20</div></center></td><td></td></tr></table></div><hr><div class="slidecontainer" id="auxiliarySliderContainer1"><table><tr class="heading"><td colspan="3">Auxiliary Point (out)</td></tr><tr><td><button class="slider-l" id="slider-minus-big-5">-5</button><button class="slider-r" id="slider-minus-5">-1</button></td><td><input type="range" min="0" max="100" value="80" class="slider" id="slider-5"></td><td><button class="slider-l" id="slider-plus-5">+1</button><button class="slider-r" id="slider-plus-big-5">+5</button></td></tr><tr><td></td><td><center><div class="slider-value" id="slider-value-5">80</div></center></td><td></td></tr></table></div><div class="buttonTableContainer"><hr><table class="buttonTable"><tr><td colspan="2"><button class="start-stop" id="start" onclick="startJob()" disabled>Start</button></td></tr></table></div></div><div id="config" class="tabcontent"><table><tr class="heading"><td colspan="2">Machine</td></tr><tr><td class="alignL"><label for="maxPos">Max Position:</label></td><td><input type="number" inputmode="numeric" pattern="[0-9]*" id="maxPos" name="maxPos"></td></tr><tr><td class="alignL"><label for="rpm">Motor RPM:</label></td><td><input type="number" inputmode="numeric" pattern="[0-9]*" id="rpm" name="rpm"></td></tr><tr><td class="alignL"><label for="invertMotor">Invert Motor:</label></td><td class="alignL"><input type="checkbox" id="invertMotor" name="invertMotor"></td><td></td></tr></table><hr><table><tr class="heading"><td colspan="2">Photos</td></tr><tr><td class="alignL"><label for="shutter">Shutter Time (ms):</label></td><td><input type="number" inputmode="numeric" pattern="[0-9]*" id="shutter" name="shutter"></td></tr><tr><td class="alignL"><label for="vibration">Vibration Delay (ms):</label></td><td><input type="number" inputmode="numeric" pattern="[0-9]*" id="vibration" name="vibration"></td></tr></table><div class="buttonTableContainer"><hr><table class="buttonTable"><tr><td colspan="2"><button class="start-stop" id="save" onclick="saveConfig()">Save</button></td></tr></table></div></div><div class="tab"><button id="setupButton" class="tablinks active" onclick="openTab(&#34;setup&#34;)">Camera Setup</button> <button id="timelapseButton" class="tablinks" onclick="openTab(&#34;timelapse&#34;)">Timelapse</button> <button id="miscButton" class="tablinks" onclick="openTab(&#34;misc&#34;)">Misc.</button> <button id="configButton" class="tablinks" onclick="openTab(&#34;config&#34;)">Config</button></div></div><script>function startJob(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString(),""==dur||""==noPics||"0"==dur||"0"==noPics?alert("Duration and Number of Pictures cannot be none!"):window.location.replace("?start&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics)}function home(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var e=new XMLHttpRequest;e.open("GET","?home&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),e.send(),refresh()}function goToStart(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var e=new XMLHttpRequest;e.open("GET","?gotopos="+slider[0].value+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),e.send(),refresh()}function goToEnd(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var e=new XMLHttpRequest;e.open("GET","?gotopos="+slider[1].value+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),e.send(),refresh()}function goToPlus(e){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var t=new XMLHttpRequest;t.open("GET","?gotoplus="+e+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),t.send(),refresh()}function goToMinus(e){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var t=new XMLHttpRequest;t.open("GET","?gotominus="+e+"&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),t.send(),refresh()}function takePhoto(){dur=document.getElementById("duration").value.toString(),noPics=document.getElementById("noPics").value.toString();var e=new XMLHttpRequest;e.open("GET","?takephoto&pos1="+slider[0].value+"&pos2="+slider[1].value+"&duration="+dur+"&nopics="+noPics,!0),e.send(),refresh()}function refreshCurrentPos(){var e=new XMLHttpRequest;e.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("slider-3").value=parseFloat(this.responseText),document.getElementById("slider-value-3").innerHTML=parseFloat(this.responseText))},e.open("GET","?currentpos",!0),e.send()}function getInitialUIValues(){var e=new XMLHttpRequest;e.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("slider-1").value=parseFloat(this.responseText),document.getElementById("slider-value-1").innerHTML=parseFloat(this.responseText))},e.open("GET","?pos1",!0),e.send();var t=new XMLHttpRequest;t.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("slider-2").value=parseFloat(this.responseText),document.getElementById("slider-value-2").innerHTML=parseFloat(this.responseText))},t.open("GET","?pos2",!0),t.send();var n=new XMLHttpRequest;n.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("noPics").value=parseFloat(this.responseText))},n.open("GET","?nopics",!0),n.send();var s=new XMLHttpRequest;s.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("duration").value=parseFloat(this.responseText))},s.open("GET","?duration",!0),s.send()}function refresh(){var e=new XMLHttpRequest;e.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("slider-3").value=parseFloat(this.responseText),document.getElementById("slider-value-3").innerHTML=parseFloat(this.responseText))},e.open("GET","?currentpos",!0),e.send();var t=new XMLHttpRequest;t.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("maxPos").value=parseFloat(this.responseText))},t.open("GET","?maxpos",!0),t.send();var n=new XMLHttpRequest;n.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("rpm").value=parseFloat(this.responseText))},n.open("GET","?rpm",!0),n.send();var s=new XMLHttpRequest;s.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("invertMotor").checked=!(0<parseFloat(this.responseText)))},s.open("GET","?invertmotor",!0),s.send();var i=new XMLHttpRequest;i.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("shutter").value=parseFloat(this.responseText))},i.open("GET","?shutter",!0),i.send();var d=new XMLHttpRequest;d.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("vibration").value=parseFloat(this.responseText))},d.open("GET","?vibration",!0),d.send()}function saveConfig(){var e=document.getElementById("invertMotor").checked?-1:1,t=new XMLHttpRequest;t.open("GET","?setmaxpos="+document.getElementById("maxPos").value+"&setrpm="+document.getElementById("rpm").value+"&setinvertmotor="+e+"&setshutter="+document.getElementById("shutter").value+"&setvibration="+document.getElementById("vibration").value,!0),t.send()}function openTab(e){refresh();var t=["setup","timelapse","misc","config"],n=4;document.getElementById(e).style.display="block",document.getElementById(e+"Button").className+=" active",document.getElementById(e+"Button").className=document.getElementById(e+"Button").className.replace(" active active"," active");for(var s=0;s<n;++s)t[s]!=e&&(document.getElementById(t[s]).style.display="none",document.getElementById(t[s]+"Button").className=document.getElementById(t[s]+"Button").className.replace(" active",""))}function setDivHeight(){for(var e=["setup","timelapse","misc","config"],t=4,n=document.getElementById("setup").offsetHeight-12-1,s=0;s<t;++s)document.getElementById(e[s]).style.height=n+"px"}function updateAuxiliarySliders(e){-1==e?(disableAuxiliarySlider(0),disableAuxiliarySlider(1)):0==e?(enableAuxiliarySlider(0),disableAuxiliarySlider(1)):1==e?(disableAuxiliarySlider(0),enableAuxiliarySlider(1)):2==e&&(enableAuxiliarySlider(0),enableAuxiliarySlider(1))}function disableAuxiliarySlider(e){0==e?(document.getElementById("slider-4").setAttribute("disabled",!0),document.getElementById("slider-minus-big-4").setAttribute("disabled",!0),document.getElementById("slider-minus-4").setAttribute("disabled",!0),document.getElementById("slider-plus-big-4").setAttribute("disabled",!0),document.getElementById("slider-plus-4").setAttribute("disabled",!0),document.getElementById("auxiliarySliderContainer0").style.color="rgba(16, 16, 16, 0.3)"):1==e&&(document.getElementById("slider-5").setAttribute("disabled",!0),document.getElementById("slider-minus-big-5").setAttribute("disabled",!0),document.getElementById("slider-minus-5").setAttribute("disabled",!0),document.getElementById("slider-plus-big-5").setAttribute("disabled",!0),document.getElementById("slider-plus-5").setAttribute("disabled",!0),document.getElementById("auxiliarySliderContainer1").style.color="rgba(16, 16, 16, 0.3)")}function enableAuxiliarySlider(e){0==e?(document.getElementById("slider-4").removeAttribute("disabled"),document.getElementById("slider-minus-big-4").removeAttribute("disabled"),document.getElementById("slider-minus-4").removeAttribute("disabled"),document.getElementById("slider-plus-big-4").removeAttribute("disabled"),document.getElementById("slider-plus-4").removeAttribute("disabled"),document.getElementById("auxiliarySliderContainer0").style.color="#000"):1==e&&(document.getElementById("slider-5").removeAttribute("disabled"),document.getElementById("slider-minus-big-5").removeAttribute("disabled"),document.getElementById("slider-minus-5").removeAttribute("disabled"),document.getElementById("slider-plus-big-5").removeAttribute("disabled"),document.getElementById("slider-plus-5").removeAttribute("disabled"),document.getElementById("auxiliarySliderContainer1").style.color="#000")}for(var slider=[],sliderValues=[],sliderC=2,smallIncrement=1,bigIncrement=5,i=0;i<sliderC;++i)slider.push(document.getElementById("slider-"+(i+1))),sliderValues.push(document.getElementById("slider-value-"+(i+1))),sliderValues[i].innerHTML=slider[i].value,function(t){slider[t].oninput=function(e){sliderValues[t].innerHTML=slider[t].value}}(i),function(t){document.getElementById("slider-minus-"+(t+1)).onclick=function(e){slider[t].value=parseFloat(slider[t].value)-smallIncrement,sliderValues[t].innerHTML=slider[t].value}}(i),function(t){document.getElementById("slider-minus-big-"+(t+1)).onclick=function(e){slider[t].value=parseFloat(slider[t].value)-bigIncrement,sliderValues[t].innerHTML=slider[t].value}}(i),function(t){document.getElementById("slider-plus-"+(t+1)).onclick=function(e){slider[t].value=parseFloat(slider[t].value)+smallIncrement,sliderValues[t].innerHTML=slider[t].value}}(i),function(t){document.getElementById("slider-plus-big-"+(t+1)).onclick=function(e){slider[t].value=parseFloat(slider[t].value)+bigIncrement,sliderValues[t].innerHTML=slider[t].value}}(i);window.setInterval("refreshCurrentPos()",1e3),getInitialUIValues(),refresh(),document.getElementById("setup").style.display="block",setDivHeight(),updateAuxiliarySliders(-1)</script></body></html>
)rawliteral";

const char status_html[] PROGMEM = R"rawliteral(<!doctype html><html lang="en"><head><link rel="manifest" href="/manifest.webmanifest"><script type="text/javascript">var width=screen.width,meta=document.createElement("meta");meta.name="viewport",meta.content=width<600?"width=device-width,initial-scale="+width/540:"width=device-width,initial-scale=1",document.getElementsByTagName("head")[0].appendChild(meta)</script><style>input{font-size:18pt!important}body{font-size:16pt;font-family:sans-serif}.start-stop{width:420px;height:40px;border-radius:20px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:10px 10px}.config{width:200px;height:40px;border-radius:20px;background:#fff;font-size:24px;border:1px solid #d3d3d3;cursor:pointer;-webkit-appearance:none;margin:10px 10px}button:hover{background-color:#0094e2;color:#fff}.heading{text-align:center;font-size:20pt;font-weight:700}td{text-align:center}#container{width:500px;margin:auto;text-align:center;padding:20px 10px 0 10px}table{margin:auto;padding-top:10px}body,html{margin:0;padding:0;width:100%;height:100%;background-color:#f7f7f7}button{touch-action:manipulation}progress{min-width:400px;margin-top:30px;margin-bottom:30px}.start-stop{margin-top:30px}</style></head><body><div id="container"><table><tr class="heading"><td colspan="2"><label for="file">Current Job:</label></td></tr><tr><td colspan="2"><progress id="file" value="0" max="100"></progress></td><td></td></tr><tr><td id="photosRemaining">Img: NaN</td><td id="timeRemaining">Time remaining: NaN</td></tr></table><a href="?stop"><button class="start-stop" id="start">Stop</button></a></div><script>function refresh(){var e=new XMLHttpRequest;e.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("timeRemaining").innerHTML=this.responseText)},e.open("GET","?timeRemaining",!0),e.send();var t=new XMLHttpRequest;t.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById("photosRemaining").innerHTML=this.responseText)},t.open("GET","?photosRemaining",!0),t.send();var n=new XMLHttpRequest;n.onreadystatechange=function(){4==this.readyState&&200==this.status&&(parseFloat(this.responseText)<-1&&window.location.replace("/"),document.getElementById("file").value=parseFloat(this.responseText))},n.open("GET","?percentDone",!0),n.send()}window.setInterval("refresh()",5e3),refresh()</script></body></html>
)rawliteral";

const char web_manifest[] PROGMEM = R"rawliteral({
    "name": "HS-Slider Controller",
    "short_name": "HS-Slider",
    "theme_color": "#f7f7f7",
    "background_color": "#f7f7f7",
    "display": "standalone",
    "orientation": "portrait",
    "scope": "/",
    "start_url": "/",
    "icons": [
      {
        "src": "/favicon-192.png",
        "type": "image/png",
        "sizes": "192x192"
      },
      {
        "src": "/favicon-512.png",
        "type": "image/png",
        "sizes": "512x512"
      }
    ]
  }
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

int pos1 = 20;
int pos2 = 80;
int maxPos = 440000;
int stepperDir = -1;
int shutterDelay = 100;
int vibrationDelay = 300;
int duration = 300;
int motorRpm = 250;

const int showDebugOutput = 1;


void saveToEEPROM(){
  unsigned long addr =0;
    EEPROM.put(addr, currentPos);
    addr += sizeof(currentPos);
    EEPROM.put(addr, pos1);
    addr += sizeof(pos1);
    EEPROM.put(addr, pos2);
    addr += sizeof(pos2);
    EEPROM.put(addr, maxPos);
    addr += sizeof(maxPos);
    EEPROM.put(addr, stepperDir);
    addr += sizeof(stepperDir);
    EEPROM.put(addr, shutterDelay);
    addr += sizeof(shutterDelay);
    EEPROM.put(addr, vibrationDelay);
    addr += sizeof(vibrationDelay);
    EEPROM.put(addr, duration);
    addr += sizeof(duration);
    EEPROM.put(addr, motorRpm);
    addr += sizeof(motorRpm);
    EEPROM.commit();
}
void readFromEEPROM(){
  unsigned long addr =0;
    EEPROM.get(addr, currentPos);
    addr += sizeof(currentPos);
    EEPROM.get(addr, pos1);
    addr += sizeof(pos1);
    EEPROM.get(addr, pos2);
    addr += sizeof(pos2);
    EEPROM.get(addr, maxPos);
    addr += sizeof(maxPos);
    EEPROM.get(addr, stepperDir);
    addr += sizeof(stepperDir);
    EEPROM.get(addr, shutterDelay);
    addr += sizeof(shutterDelay);
    EEPROM.get(addr, vibrationDelay);
    addr += sizeof(vibrationDelay);
    EEPROM.get(addr, duration);
    addr += sizeof(duration);
    EEPROM.get(addr, motorRpm);
    addr += sizeof(motorRpm);
}

void enableStepper(){
  digitalWrite(STEPPER_EN_PIN, LOW);
}
void disableStepper(){
  digitalWrite(STEPPER_EN_PIN, HIGH);
}
void takePhoto(){
  digitalWrite(RELAY_PIN,HIGH);
  delay(shutterDelay);
  digitalWrite(RELAY_PIN,LOW);
}
char goToPos(int pos,char homing = 0){
  if((pos>maxPos+100 || pos<0)&& !homing)
  {
  if(showDebugOutput)
      Serial.println("Invalid move");
    return 1;
  }
  if(showDebugOutput)
    Serial.println("OK");
  enableStepper(); //enalbe stepper
  int steps = pos-currentPos;
  stepper.move(stepperDir*steps);
  
  currentPos = pos;
  if(!homing){
    disableStepper();
  }
  
  return 0;
}

void handleWebManifest(){
  server.send ( 200, "text/json", web_manifest);
}

void handleRequest(){
  if(showDebugOutput)
    Serial.println("request for / received..");

  if(currentStatus == 'i' && server.hasArg("home")){
    if(showDebugOutput)
      Serial.println("homing.. ");
    server.send ( 200, "text/html", index_html);
    while(!digitalRead(ENDSTOP_PIN)){
      goToPos(currentPos-MOTOR_STEPS*MICROSTEPS*4,1);
    }
    currentPos=0;
    EEPROM.put(0, currentPos);
    EEPROM.commit();
    if(showDebugOutput)
      Serial.println("homed.");
    disableStepper();
  }else if(currentStatus == 'i' && server.hasArg("start") && server.hasArg("pos1") && server.hasArg("pos2") && server.hasArg("duration") && server.hasArg("nopics")){
    if(showDebugOutput)   Serial.println("starting timelapse.");
    server.send ( 200, "text/html", status_html);

    duration = server.arg("duration").toInt();
    pos1=server.arg("pos1").toInt();
    pos2=server.arg("pos2").toInt();
    
    currentPhoto=0;
    numberPhotos = (server.arg("nopics")).toFloat();
    photoInterval = (server.arg("duration")).toFloat()/(server.arg("nopics")).toFloat()*1000;
    goToPos((int)((float)(server.arg("pos1")).toFloat()/100*maxPos));
    photoSteps = (int)((float)(((server.arg("pos2")).toFloat()-(server.arg("pos1")).toFloat())/100*maxPos)/numberPhotos);
    currentStatus ='s';

    if(showDebugOutput) {  
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

  }
  else if(currentStatus == 'i' && server.hasArg("currentpos")){
    if(showDebugOutput)
      Serial.println("sending curretn position update");
    server.send ( 200, "text/html", String(100*(float)currentPos/maxPos));
  }
  else if(currentStatus == 'i' && server.hasArg("gotominus")){
    if(showDebugOutput)
      Serial.println("moving in negative direction");
    goToPos(currentPos-(server.arg("gotominus")).toFloat()/100*maxPos);
    if(showDebugOutput)
      Serial.println("saving currentPos to EEPROM");
    saveToEEPROM();
    server.send ( 200, "text/html", String(currentPos));
  }
  else if(currentStatus == 'i' && server.hasArg("gotoplus")){
    if(showDebugOutput)
      Serial.println("moving in positive direction");
    goToPos(currentPos+(server.arg("gotoplus")).toFloat()/100*maxPos);
    if(showDebugOutput)
      Serial.println("saving currentPos to EEPROM");
    saveToEEPROM();
    server.send ( 200, "text/html", String(currentPos));
  }
  else if(currentStatus == 'i' && server.hasArg("gotopos")){
    if(showDebugOutput)
      Serial.println("moving into position");
    goToPos((server.arg("gotopos")).toFloat()/100*maxPos);
    server.send ( 200, "text/html", String(currentPos));
  }
  else if(currentStatus == 'i' && server.hasArg("takephoto")){
    if(showDebugOutput)
      Serial.println("taking photo");
    takePhoto();
    server.send ( 200, "text/html", String(currentPos));
  }

  
  else if(currentStatus == 'i' && server.hasArg("pos1")){
    if(showDebugOutput)
      Serial.println("sending pos1");
    server.send ( 200, "text/html", String(pos1));
  }
  else if(currentStatus == 'i' && server.hasArg("pos2")){
    if(showDebugOutput)
      Serial.println("sending pos2");
    server.send ( 200, "text/html", String(pos2));
  }
  else if(currentStatus == 'i' && server.hasArg("nopics")){
    if(showDebugOutput)
      Serial.println("sending numberPhotosDelay");
    server.send ( 200, "text/html", String(numberPhotos));
  }
  else if(currentStatus == 'i' && server.hasArg("duration")){
    if(showDebugOutput)
      Serial.println("sending duration");
    server.send ( 200, "text/html", String(duration));
  }
  else if(currentStatus == 'i' && server.hasArg("maxpos")){
    if(showDebugOutput)
      Serial.println("sending maxPos");
    server.send ( 200, "text/html", String(maxPos));
  }
  else if(currentStatus == 'i' && server.hasArg("rpm")){
    if(showDebugOutput)
      Serial.println("sending motorRpm");
    server.send ( 200, "text/html", String(motorRpm));
  }
  else if(currentStatus == 'i' && server.hasArg("invertmotor")){
    if(showDebugOutput)
      Serial.println("sending stepperDir");
    server.send ( 200, "text/html", String(stepperDir));
  }
  else if(currentStatus == 'i' && server.hasArg("shutter")){
    if(showDebugOutput)
      Serial.println("sending shutterDelay");
    server.send ( 200, "text/html", String(shutterDelay));
  }
  else if(currentStatus == 'i' && server.hasArg("vibration")){
    if(showDebugOutput)
      Serial.println("sending vibrationDelay");
    server.send ( 200, "text/html", String(vibrationDelay));
  }
  else if(currentStatus == 'i' && server.hasArg("setmaxpos") && server.hasArg("setrpm") && server.hasArg("setinvertmotor") && server.hasArg("setshutter") && server.hasArg("setvibration")){
    if(showDebugOutput)   Serial.println("updating config");
    maxPos = server.arg("setmaxpos").toInt();
    motorRpm = server.arg("setrpm").toInt();
    stepperDir = server.arg("setinvertmotor").toInt();
    shutterDelay = server.arg("setshutter").toInt();
    vibrationDelay = server.arg("setvibration").toInt();
    
    disableStepper();
    stepper.begin(motorRpm, MICROSTEPS);
    saveToEEPROM();
    server.send ( 200, "text/html", String(currentPos));
  }


  
  else if(currentStatus == 's' && server.hasArg("percentDone")){
    if(showDebugOutput)   Serial.println("sending status update");
    server.send ( 200, "text/html", String(100*(float)currentPhoto/numberPhotos));
  }
  else if(currentStatus == 'i' && server.hasArg("percentDone")){
    if(showDebugOutput)   Serial.println("sending status update, done!");
    server.send ( 200, "text/html", "-2");
  }
  else if(currentStatus == 's' && server.hasArg("photosRemaining")){
    server.send ( 200, "text/html", "Img: " + String(currentPhoto) + "/" + String(numberPhotos));
  }
  else if(currentStatus == 's' && server.hasArg("timeRemaining")){
    server.send ( 200, "text/html", "Time remaining: " + String((int)(photoInterval*(numberPhotos - currentPhoto))/1000) + "s");
  }

  else if(currentStatus == 's' && server.hasArg("stop")){
    if(showDebugOutput)
      Serial.println("stop current job");
    if(showDebugOutput)
      Serial.println("saving surrentPos to EEPROM");
    saveToEEPROM();
    server.send ( 200, "text/html", index_html);
    currentStatus = 'i';
  }
  
  
  else if(currentStatus = 'i'){
    if(showDebugOutput)   Serial.println("bad arguments. sending index.html.. ");
    server.send ( 200, "text/html", index_html);
  }
  else if(currentStatus = 's'){
    if(showDebugOutput)   Serial.println("bad arguments. sending status.html.. ");
    server.send ( 200, "text/html", status_html);
  }
}


void setup() {
  if(showDebugOutput)
    Serial.begin(115200); //start the serial output
  if(showDebugOutput)
    Serial.println();
  if(showDebugOutput) 
    Serial.println("Starting up");
  EEPROM.begin(sizeof(currentPos)+sizeof(pos1)+sizeof(pos2)+sizeof(maxPos)+sizeof(stepperDir)+sizeof(shutterDelay)+sizeof(vibrationDelay)+sizeof(duration)+sizeof(motorRpm));
  readFromEEPROM();
  if(showDebugOutput)
    Serial.print("Curretn Pos: ");
  if(showDebugOutput)
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
  stepper.begin(motorRpm, MICROSTEPS);

  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN,LOW);
  
  
  if(showDebugOutput)
    Serial.print("Setting Access Point configuration... ");
  WiFi.mode(WIFI_AP_STA);
  
  if(showDebugOutput)
    Serial.println(WiFi.softAPConfig(localIp, gateway, subnet) ? "done" : "failed");
  
  if(showDebugOutput)
    Serial.print("Starting Access Point... ");
  if(showDebugOutput)
    Serial.println(WiFi.softAP(ssid) ? "done" : "failed");

  IPAddress myip = WiFi.softAPIP();
  if(showDebugOutput)
    Serial.print("IP: ");
  if(showDebugOutput)
    Serial.println(myip);

  if(showDebugOutput)
    Serial.println("------------------------------");
  server.on ( "/", handleRequest );
  server.on ( "/manifest.webmanifest", handleWebManifest );
  server.begin(); //start the webserver
  if(showDebugOutput)
    Serial.println("Webserver started");
  
}

void loop() {
  //code to find maxPos value
  //int maxsteps=0;
  //while(!digitalRead(ENDSTOP_PIN)){
  //  enableStepper(); //enalbe stepper
  //  maxsteps += 10000;
  //  stepper.move(stepperDir*-10000);
  //  Serial.println(maxsteps);
  //  delay(500);
  //}
  
  //check if job is running, if so handle timer, movement, and shutter
  if(currentStatus=='s' && currentPhoto>= numberPhotos){
    if(showDebugOutput)
      Serial.println("Job done");
    currentStatus='i';
  }
  else if(currentStatus=='s'){ //job running
    //check if time since last photo is larger than photoInterval
    if(millis() - lastPhotoTime + timeToMove  > photoInterval){
      if(showDebugOutput)
        Serial.println("moving to new location");
      lastPhotoTime=millis();
      //move to position
      timeToMove = millis();
      goToPos(currentPos + photoSteps);
      timeToMove = millis()-timeToMove;
      //delay to avoid vibrations
      if(showDebugOutput)
        Serial.println("vibration delay");
      delay(vibrationDelay);
      if(showDebugOutput)
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
