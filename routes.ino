void cors_set_access_control_headers()
{
  Serial.println("cors_set_access_control_headers");
  webServer.sendHeader("Access-Control-Allow-Origin", "*");
  webServer.sendHeader("Access-Control-Max-Age", "10000");
  webServer.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
  webServer.sendHeader("Access-Control-Allow-Headers", "*");
  // webServer.send(204);
}

void run_server()
{
  httpUpdateServer.setup(&webServer);

  webServer.on("/all", HTTP_GET, []() {
    cors_set_access_control_headers();
    String json = getFieldsJson(fields, fieldCount);
    webServer.send(200, "text/json", json);
  });

  webServer.on("/whoami", HTTP_GET, []() {
    cors_set_access_control_headers();
    String json = "{\"name\":\"" + String(MY_NAME) + "\"}";
    webServer.send(200, "text/json", json);
  });

  webServer.on("/fieldValue", HTTP_GET, []() {
    cors_set_access_control_headers();
    String name = webServer.arg("name");
    String value = getFieldValue(name, fields, fieldCount);
    webServer.send(200, "text/json", value);
  });

  webServer.on("/fieldValue", HTTP_POST, []() {
    cors_set_access_control_headers();
    String name = webServer.arg("name");
    String value = webServer.arg("value");
    String newValue = setFieldValue(name, value, fields, fieldCount);
    webServer.send(200, "text/json", newValue);
  });

  webServer.on("/reset", HTTP_POST, []() {
    cors_set_access_control_headers();
    Serial.print("RESETTING");
    resetFunc();
    Serial.print("RESETTING SHOULD NOT GET HERE");
  });

  webServer.on("/power", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    setPower(value.toInt());
    sendInt(power);
  });

  webServer.on("/cooling", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    cooling = value.toInt();
    broadcastInt("cooling", cooling);
    sendInt(cooling);
  });

  webServer.on("/sparking", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    sparking = value.toInt();
    broadcastInt("sparking", sparking);
    sendInt(sparking);
  });

  webServer.on("/speed", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    speed = value.toInt();
    broadcastInt("speed", speed);
    sendInt(speed);
  });

  webServer.on("/twinkleSpeed", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    twinkleSpeed = value.toInt();
    if (twinkleSpeed < 0)
      twinkleSpeed = 0;
    else if (twinkleSpeed > 8)
      twinkleSpeed = 8;
    broadcastInt("twinkleSpeed", twinkleSpeed);
    sendInt(twinkleSpeed);
  });

  webServer.on("/twinkleDensity", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    twinkleDensity = value.toInt();
    if (twinkleDensity < 0)
      twinkleDensity = 0;
    else if (twinkleDensity > 8)
      twinkleDensity = 8;
    broadcastInt("twinkleDensity", twinkleDensity);
    sendInt(twinkleDensity);
  });

  webServer.on("/solidColor", HTTP_POST, []() {
    cors_set_access_control_headers();
    String r = webServer.arg("r");
    String g = webServer.arg("g");
    String b = webServer.arg("b");
    setSolidColor(r.toInt(), g.toInt(), b.toInt());
    sendString(String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
  });

  webServer.on("/pattern", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    setPattern(value.toInt());
    sendInt(currentPatternIndex);
  });

  webServer.on("/patternName", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    setPatternName(value);
    sendInt(currentPatternIndex);
  });

  webServer.on("/palette", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    setPalette(value.toInt());
    sendInt(currentPaletteIndex);
  });

  webServer.on("/paletteName", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    setPaletteName(value);
    sendInt(currentPaletteIndex);
  });

  webServer.on("/brightness", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    setBrightness(value.toInt());
    sendInt(brightness);
  });

  webServer.on("/autoplay", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    setAutoplay(value.toInt());
    sendInt(autoplay);
  });

  webServer.on("/autoplayDuration", HTTP_POST, []() {
    cors_set_access_control_headers();
    String value = webServer.arg("value");
    setAutoplayDuration(value.toInt());
    sendInt(autoplayDuration);
  });

  //list directory
  webServer.on("/list", HTTP_GET, handleFileList);
  //load editor
  webServer.on("/edit", HTTP_GET, []() {
    cors_set_access_control_headers();
    if (!handleFileRead("/edit.htm"))
      webServer.send(404, "text/plain", "FileNotFound");
  });
  //create file
  webServer.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  webServer.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  webServer.on(
      "/edit", HTTP_POST, []() {
        webServer.send(200, "text/plain", "");
      },
      handleFileUpload);

  webServer.serveStatic("/", SPIFFS, "/", "max-age=86400");
  webServer.begin();

  Serial.println("HTTP web server started");
}
