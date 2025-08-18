#include <WiFi.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define MAX_DEVICES 20
#define BUZZER_PIN 2
#define MAX_NAME_LEN 31

const char* ssid = "ESP32_AP";
const char* password = "12345678";
const char* adminPassword = "admin123";  // Web admin password

// -------- LCD SETUP --------
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 cols, 2 rows

// -------- LCD STATE --------
int lastDeviceCount = -1;  // keep track of last shown value
bool lastWasIP = false;

struct Device {
  String mac;
  bool whitelisted;
};

Device devices[MAX_DEVICES];
int deviceCount = 0;
bool isLoggedIn = false;  // session state

// -------- HTML PAGES --------
String styleCSS() {
  return R"rawliteral(
    <style>
      body { font-family: Arial; background: #f0f0f0; color: #333; text-align: center; }
      h1 { background: #2196F3; color: white; padding: 10px; }
      table { border-collapse: collapse; width: 80%; margin: auto; background: white; }
      th, td { border: 1px solid #ddd; padding: 8px; }
      tr:nth-child(even) { background-color: #f2f2f2; }
      tr:hover { background-color: #ddd; }
      th { background-color: #4CAF50; color: white; }
      a.button { padding: 6px 12px; background: #2196F3; color: white; text-decoration: none; border-radius: 4px; }
      a.button:hover { background: #0b7dda; }
      .login-box { background: white; padding: 20px; width: 300px; margin: auto; margin-top: 100px; border-radius: 8px; box-shadow: 0px 0px 10px gray; }
      input[type=password], input[type=text] { width: 80%; padding: 8px; margin: 8px 0; }
      input[type=submit] { padding: 8px 16px; background: #4CAF50; color: white; border: none; cursor: pointer; }
      input[type=submit]:hover { background: #45a049; }
    </style>
  )rawliteral";
}

String loginPage() {
  String page = "<html><head>" + styleCSS() + "</head><body>";
  page += "<div class='login-box'><h2>Admin Login</h2>";
  page += "<form action='/login' method='GET'>";
  page += "<input type='password' name='pass' placeholder='Enter Password'><br>";
  page += "<input type='submit' value='Login'>";
  page += "</form></div></body></html>";
  return page;
}

String htmlPage() {
  String page = "<html><head>" + styleCSS() + "</head><body>";
  page += "<h1>ESP32 Admin Panel</h1>";
  page += "<h2>Connected Devices</h2>";
  page += "<table id='deviceTable'><tr><th>MAC</th><th>Name</th><th>Status</th><th>Action</th></tr>";

  for (int i = 0; i < deviceCount; i++) {
    page += "<tr>";
    page += "<td>" + devices[i].mac + "</td>";
    page += "<td><input type='text' data-mac='" + devices[i].mac + "' maxlength='" + String(MAX_NAME_LEN) + "'></td>";
    page += "<td>" + String(devices[i].whitelisted ? "Whitelisted" : "Blacklisted") + "</td>";
    page += "<td><a class='button' href='/toggle?mac=" + devices[i].mac + "'>Toggle</a></td>";
    page += "</tr>";
  }
  page += "</table>";

  // JavaScript for localStorage
  page += R"rawliteral(
    <script>
      document.querySelectorAll('input[type=text]').forEach(input => {
        const mac = input.dataset.mac;
        const savedName = localStorage.getItem('name_' + mac);
        if (savedName) input.value = savedName;
        input.addEventListener('change', () => {
          localStorage.setItem('name_' + mac, input.value);
        });
      });
    </script>
  )rawliteral";

  page += "</body></html>";
  return page;
}

// -------- DEVICE FUNCTIONS --------
void addOrUpdateDevice(String mac) {
  for (int i = 0; i < deviceCount; i++)
    if (devices[i].mac == mac) return;
  if (deviceCount < MAX_DEVICES) {
    devices[deviceCount].mac = mac;
    devices[deviceCount].whitelisted = false;
    deviceCount++;
  }
}

void scanConnectedDevices() {
  wifi_sta_list_t wifi_sta_list;
  esp_wifi_ap_get_sta_list(&wifi_sta_list);
  for (int i = 0; i < wifi_sta_list.num; i++) {
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
            wifi_sta_list.sta[i].mac[0], wifi_sta_list.sta[i].mac[1], wifi_sta_list.sta[i].mac[2],
            wifi_sta_list.sta[i].mac[3], wifi_sta_list.sta[i].mac[4], wifi_sta_list.sta[i].mac[5]);
    addOrUpdateDevice(String(macStr));
  }
}

// -------- WEB SERVER --------
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Started");
}

void loop() {
  scanConnectedDevices();

  // -------- LCD DISPLAY --------
  if (deviceCount == 0) {
    if (!lastWasIP) {  // only update when switching back to IP display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("IP Address:");
      lcd.setCursor(0, 1);
      lcd.print(WiFi.softAPIP());
      lastWasIP = true;
    }
  } else {
    if (lastDeviceCount != deviceCount || lastWasIP) {  // update only if count changed or switched from IP view
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connected Dev:");
      lcd.setCursor(0, 1);
      lcd.print(deviceCount);
      lastDeviceCount = deviceCount;
      lastWasIP = false;
    }
  }

  for (int i = 0; i < deviceCount; i++) {
    if (!devices[i].whitelisted) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }

  WiFiClient client = server.available();
  if (!client) return;

  String request = client.readStringUntil('\r');
  client.flush();

  // Login handling
  if (request.indexOf("/login?pass=") != -1) {
    String pass = request.substring(request.indexOf("=") + 1);
    pass.trim();
    if (pass.indexOf(adminPassword) != -1) isLoggedIn = true;
  }

  if (!isLoggedIn) {
    client.println("HTTP/1.1 200 OK\r\nContent-type:text/html\r\n\r\n" + loginPage());
    return;
  }

  // Toggle MAC whitelist
  if (request.indexOf("/toggle?mac=") != -1) {
    int start = request.indexOf("=") + 1;
    int end = request.indexOf(" ", start);
    String mac = request.substring(start, end);
    mac.trim();
    for (int i = 0; i < deviceCount; i++)
      if (devices[i].mac == mac) {
        devices[i].whitelisted = !devices[i].whitelisted;
      }
    client.println("HTTP/1.1 302 Found\r\nLocation: /\r\n\r\n");
    return;
  }

  // Serve admin panel
  client.println("HTTP/1.1 200 OK\r\nContent-type:text/html\r\n\r\n" + htmlPage());
}
