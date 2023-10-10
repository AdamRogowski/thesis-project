#include "WiFi.h"
#include "esp_camera.h"
#include <WebSocketsClient.h>

const char* ssid = "ssid";
const char* password = "pwd";

//const char* serverAddress = "192.168.0.163";
const char* serverAddress = "192.168.43.234";
const uint16_t serverPort = 2137;

// Define the pin for the button
const int BUTTON_PIN = 16;

WebSocketsClient webSocket;


#define STATE_WAIT_PRESS   0
#define STATE_INIT_PRESS   1
#define STATE_SENDING      2

int state = STATE_WAIT_PRESS;


// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", payload);
    }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
    case WStype_PING:
        // pong will be send automatically
        Serial.printf("[WSc] get ping\n");
        break;
    case WStype_PONG:
        // answer to a ping we send
        Serial.printf("[WSc] get pong\n");
        break;
    }
}

void setupCamera()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    
    config.frame_size = FRAMESIZE_VGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 1;
  
    // Init Camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }
}

void connectToWiFi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void setup(){
  Serial.begin(115200);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setupCamera();
  connectToWiFi();
  
  // server address, port and URL
  webSocket.begin(serverAddress, serverPort, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);
  
}

unsigned long photoCount = 0;

void loop() {


  if (digitalRead(BUTTON_PIN) == LOW){
    switch (state) {
      case STATE_WAIT_PRESS:
      Serial.println("Button pressed, init");
        state = STATE_INIT_PRESS;
        break;
      case STATE_INIT_PRESS:
        delay(100);
        break;
      case STATE_SENDING:
        webSocket.disconnect();
        Serial.println("Restarting in 10s");
        delay(10000);
        ESP.restart();
      default:
        break;
    }
  }

  switch (state) {
    case STATE_INIT_PRESS:
      if (digitalRead(BUTTON_PIN) != LOW) {
        state = STATE_SENDING;
        Serial.println("Start sending");
      }
      break;
    case STATE_SENDING:
      {
        webSocket.loop();
        camera_fb_t * fb = NULL;

        // Take Picture with Camera
        fb = esp_camera_fb_get();  
        if(!fb) {
          Serial.println("Camera capture failed");
        }
        
        webSocket.sendBIN(fb->buf,fb->len);
        photoCount++;
        Serial.println(photoCount);
        esp_camera_fb_return(fb);
      }
      break;
    default:
      Serial.println("Waiting for button press");
      break;
  }
}