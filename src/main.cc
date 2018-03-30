#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <TinyGsmClient.h>

#define SerialMon Serial
#define SerialAT Serial1
#define TINY_GSM_DEBUG SerialMon
#define SMS_TARGET "+254724587654"

#define DUMP_AT_COMMANDS
#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

//TinyGsm modem(SerialAT);

const char apn[] = "internet";
const char user[] = "";
const char pass[] = "";

void setup() {
  SerialMon.begin(115200);
  delay(10);
  SerialAT.begin(115200);
  delay(3000);
  TinyGsmAutoBaud(SerialAT);
}

void loop() {

  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  if (!modem.restart()) {
    delay(10000);
    return;
  }

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem: ");
  SerialMon.println(modemInfo);

  //modem.simUnlock("1234");

  SerialMon.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
    delay(10000);
    return;
  }

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  SerialMon.print("Connecting to: ");
  SerialMon.println(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    delay(10000);
    return;
  }

  bool res;

  String ccid = modem.getSimCCID();
  SerialMon.print("CCID: ");
  SerialMon.println(ccid);

  String imei = modem.getIMEI();
  SerialMon.print("IMEI: ");
  SerialMon.println(imei);

  String cop = modem.getOperator();
  SerialMon.print("Operator: ");
  SerialMon.println(cop);
  
  IPAddress local = modem.localIP();
  SerialMon.print("Local IP:");
  SerialMon.println(local);

  int csq = modem.getSignalQuality();
  SerialMon.print("Signal quality: ");
  SerialMon.println(csq);

#if defined(SMS_TARGET)
  res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);
  SerialMon.print(res);
  res ? SerialMon.println("OK") :SerialMon.println("Failed");

  res = modem.sendSMS_UTF16(SMS_TARGET, u"SIM-800L", 8);
  SerialMon.print("UTF16 SMS:");
  res ? SerialMon.println("OK") : SerialMon.println("Failed");
#endif


  modem.gprsDisconnect();
  if (!modem.isGprsConnected()) {
    SerialMon.println("GPRS disconnected");
  } else {
    SerialMon.println("GPRS disconnect: Failed.");
  }

  // Try to power-off (modem may decide to restart automatically)
  // To turn off modem completely, please use Reset/Enable pins
//   modem.poweroff();
//   SerialMon.println("Poweroff.");

  // Do nothing forevermore
  while (true) {
    modem.maintain();
  }
}
