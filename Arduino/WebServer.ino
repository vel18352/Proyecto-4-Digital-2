/*************************************************************************************************
  ESP32 Web Server
  Ejemplo de creación de Web server 
  Basándose en los ejemplos de: 
  https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/
  https://electropeak.com/learn
**************************************************************************************************/
//************************************************************************************************
// Librerías
//************************************************************************************************
#include <WiFi.h>
#include <WebServer.h>
//************************************************************************************************
// Variables globales
//************************************************************************************************
// SSID & Password
const char* ssid = "Nexxt_528930";  // Enter your SSID here
const char* password = "7Jjn17AJ";  //Enter your Password here

uint8_t Sensores[4];
uint8_t n_Parqueos = 0;
WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

uint8_t LED1pin = 2;
bool LED1status = LOW;

//************************************************************************************************
// Configuración
//************************************************************************************************
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.println("Try Connecting to ");
  Serial.println(ssid);

  pinMode(LED1pin, OUTPUT);

  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  server.on("/", handle_OnConnect); // Directamente desde e.g. 192.168.0.8
  server.on("/leer", handle_Data); // handler para enviar datos

  
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  delay(100);
}
//************************************************************************************************
// loop principal
//************************************************************************************************
void loop() {
  server.handleClient();          //Inicia servidor
  if (Serial2.available())        //Inicia lectura del UART
    {   
      digitalWrite(2, 1);         //Enciende led siempre que se reciba datos por UART
        for (int i = 0; i<=3; i++)//se lee 4 bytes y se almacenan en un array
        {
          Sensores[i] = Serial2.read(); 
          //Serial.print(Sensores[i],BIN); 
          //Serial.println(' ');
        }
        n_Parqueos = (((4 - Sensores[0])- Sensores[1])- Sensores[2])- Sensores[3];  //Se realiza el calculo de cuantos parqueos hay
        //Serial.println(n_Parqueos);
        //Serial.println(' ');
    }
    digitalWrite(2, 0);         //apaga led de lectura UART
    //Serial.println(data_json());

}
//************************************************************************************************
// Handler de Inicio página
//************************************************************************************************
void handle_OnConnect() {
  server.send(200, "text/html", SendHTML());
}
//************************************************************************************************
// Handlers
//************************************************************************************************
void handle_Data() {                          //se envia la data y se concatena los datos a enviar
   String dato_json = "{\"parqueo1\":";
          dato_json += Sensores[0];
          dato_json += ",";
          dato_json += "\"parqueo2\":";
          dato_json += Sensores[1];
          dato_json += ",";
          dato_json += "\"parqueo3\":";
          dato_json += Sensores[2];
          dato_json += ",";
          dato_json += "\"parqueo4\":";
          dato_json += Sensores[3];
          dato_json += ",";
          dato_json += "\"lugares\":";
          dato_json += n_Parqueos;
          dato_json += "}";
  server.send(200, "application/json", dato_json);
}

//************************************************************************************************
// Procesador de HTML
//************************************************************************************************
String SendHTML(void) {
  String ptr = "<!DOCTYPE html>\n";
  ptr = "<html>\n";
  ptr += "<head>\n";
  ptr += "<style>\n";
  ptr += "body {font-family: Helvetica, sans-serif;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Proyecto 4: Parqueos</h1>\n";
  ptr += "<h3>Emilio Velasquez 18352</h3>\n";
  ptr += "<canvas id=\"Cantidad\" width=\"825\" height=\"40\" style=\"border:0px solid #000000;\">\n";
  ptr += "</canvas>\n";
  ptr += "<canvas id=\"Parqueo 1\" width=\"200\" height=\"100\" style=\"border:2px solid #000000;\">\n";
  ptr += "</canvas>\n";
  ptr += "<canvas id=\"Parqueo 2\" width=\"200\" height=\"100\" style=\"border:2px solid #000000;\">\n";
  ptr += "</canvas>\n";
  ptr += "<canvas id=\"Parqueo 3\" width=\"200\" height=\"100\" style=\"border:2px solid #000000;\">\n";
  ptr += "</canvas>\n";
  ptr += "<canvas id=\"Parqueo 4\" width=\"200\" height=\"100\" style=\"border:2px solid #000000;\">\n";
  ptr += "</canvas>\n";
  ptr += "<script>\n";
  ptr += "function Sensor(n_parqueo, valor){\n";
  ptr += "var canvas = document.getElementById(n_parqueo);\n";
  ptr += "var ctx = canvas.getContext(\"2d\");\n";
  ptr += "if (valor == 0){\n";
  ptr += "ctx.fillStyle = \"#2cfc03\";\n";
  ptr += "};\n";
  ptr += "if (valor == 1){\n";
  ptr += "ctx.fillStyle = \"#fc0303\";\n";
  ptr += "};\n";
  ptr += "ctx.fillRect(0,0,200,100);\n";
  ptr += "ctx.fillStyle = \"#000000\";\n";
  ptr += "ctx.font = \"30px Arial\";\n";
  ptr += "ctx.fillText(n_parqueo, 28, 60);\n";
  ptr += "};\n";
  ptr += "function Parqueos(cantidad){\n";
  ptr += "var canvas = document.getElementById(\"Cantidad\");\n";
  ptr += "var ctx = canvas.getContext(\"2d\");\n";
  ptr += "ctx.fillStyle = \"#FFFFFF\";\n";
  ptr += "ctx.fillRect(0,0,825,40);\n";
  ptr += "ctx.fillStyle = \"#000000\";\n";
  ptr += "ctx.font = \"20px Arial\";\n";
  ptr += "ctx.fillText(\"Cantidad de parqueos disponibles: \" + cantidad.toString(), 0,30);\n";
  ptr += "};\n";
  ptr += "Parqueos(4);\n";
  ptr += "Sensor(\"Parqueo 1\", 0);\n";
  ptr += "Sensor(\"Parqueo 2\", 0);\n";
  ptr += "Sensor(\"Parqueo 3\", 0);\n";
  ptr += "Sensor(\"Parqueo 4\", 0);\n";
  ptr += "var sendHttpRequest = function(){\n";
  ptr += "var xhr = new XMLHttpRequest();\n";
  ptr += "xhr.open(\"GET\", \"http://192.168.1.173/leer\");\n";
  ptr += "xhr.responseType = \'json\';\n";
  //ptr += "var receive = xhr.responseText;\n";
  ptr += "xhr.onload = function() {\n";
  ptr += "  console.log(xhr.response);\n";
  ptr += "Parqueos(xhr.response.lugares);\n";
  ptr += "Sensor(\"Parqueo 1\", xhr.response.parqueo1);\n";
  ptr += "Sensor(\"Parqueo 2\", xhr.response.parqueo2);\n";
  ptr += "Sensor(\"Parqueo 3\", xhr.response.parqueo3);\n";
  ptr += "Sensor(\"Parqueo 4\", xhr.response.parqueo4);\n"; 
  ptr += "};\n";
  ptr += "xhr.send();\n"; 
  ptr += "return xhr.response;\n";
  ptr += "};\n"; 
  ptr += "setInterval(function(){\n";
  ptr += "sendHttpRequest();\n";
  ptr += "},1);\n";
  ptr += "</script>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
//************************************************************************************************
// Handler de not found
//************************************************************************************************
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}


String data_json(void){
  String dato_json = "{\"parqueo1\":";
          dato_json += Sensores[0];
          dato_json += ",";
          dato_json += "\"parqueo2\":";
          dato_json += Sensores[1];
          dato_json += ",";
          dato_json += "\"parqueo3\":";
          dato_json += Sensores[2];
          dato_json += ",";
          dato_json += "\"parqueo4\":";
          dato_json += Sensores[3];
          dato_json += ",";
          dato_json += "\"lugares\":";
          dato_json += n_Parqueos;
          dato_json += "}";
  return dato_json;
}
