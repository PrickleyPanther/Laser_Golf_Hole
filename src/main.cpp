#include <Arduino.h>
#include <Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"

//------Function Declare ------

void CalculateAngle(double adjacent);
void SetLaser();
void HandleRoot();
void HandleNotFound();
void HandleLaser();
void ReadWebPage();
void HandleSettings();
void HandleToggleLaser();
void ToggleLaser();

//-----------------------------


//------WebServer Props --------

WebServer server(80);
MDNSResponder mdns;
#undef HOSTNAME
#define HOSTNAME "GolfSimHole"
const char *ssid = "***";
const char *password = "****";

String WebPageHtml;

//-----END WebServer Props -----

static const int LaserPin = 12;
static const int ServoPin = 14;

Servo servo;
double ServoAngle = 0.0;
double laserHeight = 7.0; // height in ft
int holeDistance = 0;
int MaxDistanceChars = 6;
int laserTimeoutMs = 5000; // 5 set
bool laserState = false;

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());

  // Setup DNS responder
  if (mdns.begin(HOSTNAME))
  {
    Serial.println("MDNS responder started");
  }

  ReadWebPage();

  // Setup WebServer Handlers
  server.on("/", HandleRoot);
  server.on("/ballDist", HandleLaser);
  server.on("/settings", HandleSettings);
  server.on("/laser", HandleToggleLaser);

  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });

  server.onNotFound(HandleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  Serial.println("Attaching Laser and Servo Pins...");
  pinMode(LaserPin, OUTPUT);
  servo.attach(
      ServoPin,
      Servo::CHANNEL_NOT_ATTACHED);

  Serial.println("Enter the distance to hole: ");
}

void loop()
{
  server.handleClient();

  // Uncomment if distances are sent through serial
  // if (Serial.available() > 0)
  // {
  //   // Read in the ball distance as a double
  //   String result = Serial.readStringUntil(':');
  //   Serial.println(result);
  //   double resultDouble = result.toDouble();
  //   Serial.print("Distance entered ");
  //   Serial.println(resultDouble);
  //   CalculateAngle(resultDouble);
  //   SetLaser();
  // }
}

void CalculateAngle(double adjacent)
{
  double ratio = laserHeight / adjacent;
  double tanAngle = atan(ratio) * 180 / PI;
  Serial.print("Tan Angle: ");
  Serial.println(tanAngle);
  double totalAngle = tanAngle + 90;

  ServoAngle = 180 - totalAngle;

  Serial.print("Laser Angle: ");
  Serial.println(ServoAngle);
}

void SetLaser()
{
  ToggleLaser();
  servo.write(ServoAngle);
  delay(laserTimeoutMs);
  ToggleLaser();
  ServoAngle = 0;
  servo.write(ServoAngle);
}

void ToggleLaser()
{
    if(laserState)
    {
      digitalWrite(LaserPin, LOW);
      laserState = false;
    }
    else
    {
      digitalWrite(LaserPin, HIGH);
      laserState = true;
    }
}

void HandleRoot()
{
  server.send(200, WebPageHtml.c_str());
}

void HandleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void HandleLaser()
{
  Serial.println("Handling Laser Triggered");
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "dist")
    {
      Serial.println("Dist found");
      double dist = strtod(server.arg(i).c_str(), NULL);

      Serial.print("Setting Laser ");
      Serial.println(dist);
      CalculateAngle(dist);
      SetLaser();
    }
  }
  HandleRoot();
}

void HandleSettings()
{
  Serial.println("Handling Settings");
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "lHeight")
    {
      double lHeight = strtod(server.arg(i).c_str(), NULL);
      laserHeight = lHeight;
      Serial.print("Height found: ");
      Serial.println(lHeight);
    }
    if (server.argName(i) == "lTimeout")
    {
      double timeout = strtod(server.arg(i).c_str(), NULL);
      laserTimeoutMs = timeout * 1000;
      Serial.print("Timeout found: ");
      Serial.println(timeout);
    }
    // TODO: Save settings to spiff
    HandleRoot();
  }
}

void HandleToggleLaser()
{
   Serial.println("Handling Laser Toggle");

   ToggleLaser();
}

// Read the contents of the webpage html to host
void ReadWebPage()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/GolfLaser.html");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("File Length:");
  while (file.available())
  {
    WebPageHtml += file.readString();
  }

  Serial.println(WebPageHtml.length());

  file.close();
}
