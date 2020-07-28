// AP mode password
const char WiFiAPPSK[] = "";

const char *ssid;
const char *password;

const bool apMode = false;

void wifi_setup()
{
  if (apMode)
  {
    WiFi.mode(WIFI_AP);

    // Do a little work to get a unique-ish name. Append the
    // last two bytes of the MAC (HEX'd) to "Thing-":
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.softAPmacAddress(mac);
    String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                   String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
    //    String AP_NameString = "Thousand Petal Lotus " + macID;
    //    String AP_NameString = "Bed " + macID;
    //    String AP_NameString = "First Floor " + macID;
    String AP_NameString = String(MY_NAME) + " " + macID;

    char AP_NameChar[AP_NameString.length() + 1];
    memset(AP_NameChar, 0, AP_NameString.length() + 1);

    for (int i = 0; i < AP_NameString.length(); i++)
      AP_NameChar[i] = AP_NameString.charAt(i);

    WiFi.softAP(AP_NameChar, WiFiAPPSK);

    Serial.printf("Connect to Wi-Fi access point: %s\n", AP_NameChar);
    Serial.println("and open http://192.168.4.1 in your browser");
  }
  else
  {
    Serial.begin(115200);
    WiFiManager wifiManager;
    Serial.println("Connecting.....");
    wifiManager.autoConnect("AutoConnectAP");
    Serial.println("Connected");
    ssid = WiFi.SSID().c_str();
    password = WiFi.psk().c_str();
    WiFi.mode(WIFI_STA);
    Serial.printf("Connecting to %s\n", ssid);
    if (String(WiFi.SSID()) != String(ssid))
    {
      WiFi.begin(ssid, password);
    }
  }
}
