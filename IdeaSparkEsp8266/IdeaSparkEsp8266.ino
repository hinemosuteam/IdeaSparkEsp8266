#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ========================================
// DHT11設定
// ========================================
#define DHT_PIN D2
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// ========================================
// OLED設定
// ========================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ========================================
// 前回の正常値
// ========================================
float lastTemperature = NAN;
float lastHumidity = NAN;

// ========================================
// エラー回数
// ========================================
int errorCount = 0;

// ========================================
// 初期設定
// ========================================
void setup() {

  Serial.begin(115200);

  // DHT11開始
  dht.begin();

  // ESP8266 OLED I2C
  // SDA = D6(GPIO12)
  // SCL = D5(GPIO14)
  Wire.begin(D6, D5);

  // OLED開始
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {

    Serial.println("OLEDが見つかりません");

    // OLEDがない場合は停止
    while (true) {
      delay(1000);
    }
  }

  // OLED初期表示
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(20, 10);
  display.println("DHT11");

  display.setTextSize(1);
  display.setCursor(20, 40);
  display.println("Starting...");

  display.display();

  delay(2000);
}

// ========================================
// メインループ
// ========================================
void loop() {

  // ========================================
  // DHT11から読み取り
  // ========================================

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // ========================================
  // 読み取り結果を確認
  // ========================================

  if (isnan(temperature) || isnan(humidity)) {

    // 読み取り失敗
    errorCount++;

    Serial.println("DHT11 ERROR");
    Serial.print("Error count: ");
    Serial.println(errorCount);

    // ========================================
    // OLEDにERROR表示
    // ========================================

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("DHT11 Temperature");

    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("ERROR");

    display.setTextSize(1);
    display.setCursor(0, 48);
    display.print("Error count: ");
    display.println(errorCount);

    // ここで必ずOLEDを更新
    display.display();

  }
  else {

    // ========================================
    // 読み取り成功
    // ========================================

    lastTemperature = temperature;
    lastHumidity = humidity;

    // エラー回数をリセット
    errorCount = 0;

    // シリアル表示
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity, 1);
    Serial.println(" %");

    // ========================================
    // OLED表示
    // ========================================

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // タイトル
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("DHT11 Temperature");

    // ========================================
    // 温度
    // ========================================

    display.setTextSize(2);
    display.setCursor(0, 18);

    display.print(temperature, 1);

    // Cの位置
    int x = display.getCursorX();

    display.setCursor(x + 6, 18);
    display.print("C");

    // 度記号
    display.fillCircle(
      x + 2,
      21,
      2,
      SSD1306_WHITE
    );

    // ========================================
    // 湿度
    // ========================================

    display.setTextSize(2);
    display.setCursor(0, 42);

    display.print(humidity, 1);
    display.println("%");

    // OLED更新
    display.display();
  }

  // ========================================
  // DHT11は最低でも約2秒間隔
  // ========================================

  delay(2000);
}
