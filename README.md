# False Beacon

> **Stealth Wi-Fi Credential Capture Tool for ESP32**

---

**False Beacon** is a rogue Wi-Fi access point project built using the ESP32 and a 0.96" OLED display.  
It creates a fake wireless network that lures devices into connecting and prompts users with a fake login page, capturing their credentials for educational and research purposes.

---

## 🚀 Features

-   Creates a **fake Wi-Fi access point** (AP) mimicking public networks.
-   Hosts a **captive portal** (login phishing page).
-   Captures and displays **entered usernames and passwords** on an OLED screen.
-   Lightweight, fast, and fully self-contained on ESP32 hardware.
-   Useful for **security awareness training** and **red team simulations**.

---

## 🛠️ Hardware Requirements

-   ESP32 Development Board
-   0.96" I2C OLED Display (SSD1306 Driver)

---

## 👔 Software Requirements

-   Arduino IDE
-   ESP32 Board Manager installed
-   Libraries:
    -   **WiFi.h**
    -   **WebServer.h**
    -   **Adafruit_GFX.h**
    -   **Adafruit_SSD1306.h**

---

## ⚙️ How It Works

1. ESP32 sets up a fake Wi-Fi network (example: `Free_Public_WiFi`).
2. Victim connects and is redirected to a fake login page.
3. Submitted credentials are captured and displayed in real-time on the OLED screen.

---

## 🤠 Why False Beacon?

Wi-Fi phishing (also called "evil twin" attacks) is a **real-world risk**.  
**False Beacon** provides a **safe, isolated environment** to study these attack vectors and build better defensive strategies.

---

## ⚠️ Disclaimer

> This project is for **educational purposes only**.  
> Use only in controlled, isolated environments (e.g., your personal homelab).  
> **Unauthorized use against networks you do not own is illegal.**  
> The creator is not responsible for misuse of this tool.

---

# ✨ Project Status

✅ Basic functionality: Fake AP, captive portal, credential capture  
🛠️ In progress: Auto-reset, SPIFFS logging, improved UI
