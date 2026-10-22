
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <Arduino_MKRIoTCarrier.h>

MKRIoTCarrier carrier;

bool hasData = false;

String characterName, characterSpecies, characterStatus;
String characterOrigin, characterImageUrl;

const char* characters[] = {
  "Rick Sanchez",
  "Morty Smith",
  "Summer Smith",
  "Toxic Rick",
  "Birdperson"
};

const int CHARACTER_COUNT = sizeof(characters) / sizeof(characters[0]);
int nextCharacterIndex = 0;

unsigned long lastRequestAt = 0;
bool requestMade = false;

const char SERVER[] = "rickandmortyapi.com";
const char BACKEND_SERVER[] = "rickymortyapp-vlads-projects-55e33c4b.vercel.app";

WiFiSSLClient secureClient;
HttpClient http(secureClient, SERVER, 443);


bool connectWifi() {
  if (WiFi.status() == WL_CONNECTED) return true;

  Serial.println("Forbinder til WiFi...");
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  unsigned long started = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - started < 20000UL) {
    delay(250);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi fejlede. Tjek navn og kode.");
    return false;
  }

  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
  return true;
}

bool fetchCharacter(const char* name, JsonDocument& doc) {
  if (!connectWifi()) return false;

  String encodedName = String(name);
  encodedName.replace(" ", "%20");

  String path = "/api/character/?name=";
  path += encodedName;

  http.stop();
  http.setHttpResponseTimeout(10000);

  int result = http.get(path);

  if (result != 0) {
    Serial.print("Forbindelsesfejl: ");
    Serial.println(result);
    http.stop();
    return false;
  }

  int status = http.responseStatusCode();

  Serial.print("HTTP-status: ");
  Serial.println(status);

  if (status != 200) {
    http.stop();
    return false;
  }

  if (http.skipResponseHeaders() != 0) {
    Serial.println("Header-fejl.");
    http.stop();
    return false;
  }

  JsonDocument filter;

  filter["results"][0]["name"] = true;
  filter["results"][0]["species"] = true;
  filter["results"][0]["status"] = true;
  filter["results"][0]["origin"]["name"] = true;
  filter["results"][0]["image"] = true;

  DeserializationError error = deserializeJson(
    doc,
    http,
    DeserializationOption::Filter(filter)
  );

  http.stop();

  if (error) {
    Serial.print("JSON-fejl: ");
    Serial.println(error.c_str());
    return false;
  }

  return true;
}

String textValue(JsonVariantConst value) {
  if (!value.is<const char*>()) return "Ikke oplyst";

  String text = value.as<String>();
  return text.length() ? text : "Ikke oplyst";
}

bool parseCharacterData(JsonDocument& doc, const char* requestedName) {

  JsonArrayConst rows = doc["results"].as<JsonArrayConst>();

  if (rows.isNull() || rows.size() == 0) {
    Serial.println("Ingen karakterer fundet.");
    return false;
  }

  for (JsonObjectConst obj : rows) {

    String name = textValue(obj["name"]);

    if (name != requestedName) {
      continue;
    }

    characterName = name;
    characterSpecies = textValue(obj["species"]);
    characterStatus = textValue(obj["status"]);
    characterOrigin = textValue(obj["origin"]["name"]);
    characterImageUrl = textValue(obj["image"]);

    Serial.println(characterName);
    Serial.println(characterSpecies);
    Serial.println(characterStatus);
    Serial.println(characterOrigin);
    Serial.println(characterImageUrl);

    hasData = true;
    return true;
  }

  Serial.println("Karakteren blev ikke fundet.");
  return false;
}

void printLine(String text, int y) {
  text = text.substring(0, 14);

  int16_t x1, y1;
  uint16_t w, h;

  carrier.display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  carrier.display.setCursor((240 - (int)w) / 2 - x1, y);
  carrier.display.print(text);
}

void showScreen(String title, String row1, String row2) {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextSize(2);
  carrier.display.setTextColor(ST77XX_WHITE);

  printLine(title, 50);
  printLine(row1, 100);
  printLine(row2, 135);

  carrier.display.setTextSize(2);
  carrier.display.setTextColor(ST77XX_CYAN);
  printLine("0:Next 1:Info", 186);
printLine("2:Orig 3:Save", 202);
}

void showPage(int page) {
  if (!hasData) {
    showScreen("Ingen data", "Tryk TOUCH0", "Se ogsaa Serial");
    return;
  }

  if (page == 1) {
    showScreen("", "Species:", "");
    carrier.display.setTextColor(ST77XX_GREEN);
    printLine(characterName, 50);
    printLine(characterSpecies, 135);
    printLine(characterStatus, 160);
  }

  else if (page == 2) {
    showScreen("", "Origin:", "");

    carrier.display.setTextColor(ST77XX_GREEN);
    printLine(characterName, 50);

    carrier.display.setTextSize(2);
    carrier.display.setTextWrap(true);
    carrier.display.setCursor(30, 125);
    carrier.display.println(characterOrigin);
    carrier.display.setTextWrap(false);
  }
}

const char* nextCharacterName() {
  const char* name = characters[nextCharacterIndex];

  nextCharacterIndex = (nextCharacterIndex + 1) % CHARACTER_COUNT;

  return name;
}

void loadObject() {
  if (requestMade && millis() - lastRequestAt < 5000UL) {
    showScreen("Vent lidt", "Mindst 5 sek.", "mellem kald");
    return;
  }

  lastRequestAt = millis();
  requestMade = true;
  hasData = false;

  showScreen("Henter data", "Vent...", "");

  const char* selected = nextCharacterName();

  Serial.print("Valgt karakter: ");
  Serial.println(selected);

  JsonDocument doc;

  if (fetchCharacter(selected, doc) &&
      parseCharacterData(doc, selected)) {
    showScreen("Hentet", characterName, "Vaelg TOUCH1-3");
  }
  else {
    showScreen("Hentning fejlet", "Se Serial", "Tryk 0 igen");
  }
}
//todo: add conn to backend
  
void saveCharacter() {
  if (!hasData) {
    showScreen("Ingen data", "Tryk TOUCH0", "Vaelg karakter");
    return;
  }

  if (!connectWifi()) {
    showScreen("WiFi fejl", "Ingen forbindelse", "");
    return;
  }

  JsonDocument saveDoc;

  saveDoc["characterName"] = characterName;
  saveDoc["characterSpecies"] = characterSpecies;
  saveDoc["characterStatus"] = characterStatus;
  saveDoc["characterOrigin"] = characterOrigin;
  saveDoc["characterImageUrl"] = characterImageUrl;

  String json;
  serializeJson(saveDoc, json);

  Serial.println("JSON til backend:");
  Serial.println(json);

  showScreen("Sender data", "Vent...", "");

  WiFiSSLClient backendClient;
  HttpClient backendHttp(backendClient, BACKEND_SERVER, 443);

  String path = "/api/Character";

  backendHttp.setHttpResponseTimeout(10000);

  int result = backendHttp.post( path, "application/json",json );

  if (result != 0) {
    Serial.print("Forbindelsesfejl: ");
    Serial.println(result);
    backendHttp.stop();
    showScreen("Fejl", "Forbindelse", "");
    return;
  }

  int status = backendHttp.responseStatusCode();

  Serial.print("Backend HTTP-status: ");
  Serial.println(status);

  backendHttp.stop();

  if (status == 200 || status == 201) {
    showScreen("Gemt", characterName, "");
  }
  else {
    showScreen("Ikke gemt", "Backend fejl", String(status));
  }
}



void setup() {
  Serial.begin(9600);

  unsigned long started = millis();

  while (!Serial && millis() - started < 3000UL) {}

  carrier.noCase();

  if (!carrier.begin()) {
    Serial.println("Carrier kunne ikke starte.");

    while (true) {
      delay(100);
    }
  }

  carrier.display.setRotation(0);
  carrier.display.setTextWrap(false);

  Serial.print("Carrier revision: ");
  Serial.println(carrier.getBoardRevision());

  showScreen("Rick Morty API", "Forbinder...", "");

  bool connected = connectWifi();

  showScreen(
    "Rick Morty API",
    connected ? "WiFi klar" : "WiFi fejlede",
    "Tryk TOUCH0"
  );
}

void loop() {
  carrier.Buttons.update();

  if (carrier.Buttons.onTouchDown(TOUCH0)) {
    loadObject();
  }

  if (carrier.Buttons.onTouchDown(TOUCH1)) {
    showPage(1);
  }

  if (carrier.Buttons.onTouchDown(TOUCH2)) {
    showPage(2);
  }

  if (carrier.Buttons.onTouchDown(TOUCH3)) {
    saveCharacter();
  }
  
}



