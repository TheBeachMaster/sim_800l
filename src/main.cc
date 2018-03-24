#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <TinyGsmClient.h>

#define SerialMon Serial
#define SerialAT Serial1
#define TINY_GSM_DEBUG SerialMon
#define SMS_TARGET "+254724587654"

// #define DUMP_AT_COMMANDS
// #ifdef DUMP_AT_COMMANDS
//   #include <StreamDebugger.h>
//   StreamDebugger debugger(SerialAT, SerialMon);
//   TinyGsm modem(debugger);
// #else
//   TinyGsm modem(SerialAT);
// #endif

TinyGsm modem(SerialAT);

const char apn[] = "internet";
const char user[] = "";
const char pass[] = "";

void setup() {
  SerialMon.begin(115200);
  delay(10);
  delay(3000);

  TinyGsmAutoBaud(SerialAT);
}

void loop() {

  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.restart()) {
    delay(10000);
    return;
  }

  String modemInfo = modem.getModemInfo();
  DBG("Modem:", modemInfo);

  //modem.simUnlock("1234");

  DBG("Waiting for network...");
  if (!modem.waitForNetwork()) {
    delay(10000);
    return;
  }

  if (modem.isNetworkConnected()) {
    DBG("Network connected");
  }

  DBG("Connecting to", apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    delay(10000);
    return;
  }

  bool res;

  String ccid = modem.getSimCCID();
  DBG("CCID:", ccid);

  String imei = modem.getIMEI();
  DBG("IMEI:", imei);

  String cop = modem.getOperator();
  DBG("Operator:", cop);
  
  IPAddress local = modem.localIP();
  DBG("Local IP:", local);

  int csq = modem.getSignalQuality();
  DBG("Signal quality:", csq);

#if defined(SMS_TARGET)
  res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);
  DBG("SMS:", res ? "OK" : "Failed");

  res = modem.sendSMS_UTF16(SMS_TARGET, u"SIM-800L", 8);
  DBG("UTF16 SMS:", res ? "OK" : "Failed");
#endif


  modem.gprsDisconnect();
  if (!modem.isGprsConnected()) {
    DBG("GPRS disconnected");
  } else {
    DBG("GPRS disconnect: Failed.");
  }

  // Try to power-off (modem may decide to restart automatically)
  // To turn off modem completely, please use Reset/Enable pins
//   modem.poweroff();
//   DBG("Poweroff.");

  // Do nothing forevermore
  while (true) {
    modem.maintain();
  }
}
