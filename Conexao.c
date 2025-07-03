#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// 1. CONFIGURAÇÕES DO WIFI
const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";

// 2. CONFIGURAÇÕES DO INTERSCITY
const char* INTERSCITY_URL = "http://SEU_SERVIDOR:8000";  // Altere aqui
const char* SENSOR_ID = "sensor-001";  // Este ID deve estar registrado previamente no InterSCity
const float NIVEL_CRITICO = 60.0;

// 3. SETUP INICIAL
void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[OK] Conectado ao Wi-Fi.");
}

// 4. LOOP DE ENVIO DE DADOS
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float nivel = random(30, 80);   // Simula o nível da água (cm)
    float chuva = random(0, 25);    // Simula a chuva (mm)
    bool alerta = nivel > NIVEL_CRITICO;

    Serial.printf("Nível: %.1f cm | Chuva: %.1f mm | Alerta: %s\n",
                  nivel, chuva, alerta ? "SIM" : "NÃO");

    HTTPClient http;
    String url = String(INTERSCITY_URL) + "/collector/data/" + SENSOR_ID;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // 5. CONSTRUIR O JSON PARA O INTERSCITY
    StaticJsonDocument<512> doc;
    JsonArray data = doc.createNestedArray("data");

    JsonObject obj1 = data.createNestedObject();
    obj1["capability"] = "nivel_agua";
    obj1["value"] = nivel;

    JsonObject obj2 = data.createNestedObject();
    obj2["capability"] = "chuva";
    obj2["value"] = chuva;

    JsonObject obj3 = data.createNestedObject();
    obj3["capability"] = "alerta_alagamento";
    obj3["value"] = alerta;

    String jsonStr;
    serializeJson(doc, jsonStr);

    // 6. ENVIO DOS DADOS
    int httpResponseCode = http.POST(jsonStr);
    if (httpResponseCode > 0) {
      Serial.printf("[OK] Dados enviados! Código: %d\n", httpResponseCode);
    } else {
      Serial.printf("[ERRO] Código: %d | %s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("[ERRO] Wi-Fi desconectado.");
  }

  delay(5000);  // Espera 5 segundos antes da próxima leitura
}
