#include <Clock.hpp>

Clock::Clock() {
  //strcat(_ntpServerName, "us.pool.ntp.org");

}

Clock* Clock::s_clock = 0;

Clock* Clock::getInstance() {
  if (!s_clock) {
    s_clock = new Clock();
  }
  return s_clock;
}

void Clock::begin() {
  _udp.begin(_port);
  setSyncProvider(s_getNTPTime);
  setSyncInterval(300);
}

time_t Clock::s_getNTPTime() {
  return s_clock->getNTPTime();
}

time_t Clock::getNTPTime() {
  IPAddress ntpServerIP; // NTP server's ip address
    while (_udp.parsePacket() > 0) ; // discard any previously received packets
    Serial.println("Transmit NTP Request");
    // get a random server from the pool
    WiFi.hostByName(_ntpServerName, ntpServerIP);
    Serial.print(_ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
      int size = _udp.parsePacket();
      if (size >= NTP_PACKET_SIZE) {
        Serial.println("Receive NTP Response");
        _udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
        unsigned long secsSince1900;
        // convert four bytes starting at location 40 to a long integer
        secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long)packetBuffer[43];
        return secsSince1900 - 2208988800UL + _timeZone * SECS_PER_HOUR;
      }
    }
    Serial.println("No NTP Response :-(");
    return 0; // return 0 if unable to get the time
}

void Clock::sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  _udp.beginPacket(address, 123); //NTP requests are to port 123
  _udp.write(packetBuffer, NTP_PACKET_SIZE);
  _udp.endPacket();
}

void Clock::getTimeNowChr(char* out) {
  return getTimeChr(out, now());
}

bool Clock::isATOrPastTime(const char* timeChr)
{
  time_t timeNow = now();
  int secsNow = getSecondsSinceMidnight(timeNow);
  int secsDesired = getSecondsSinceMidnight(timeChr);
  return secsNow >= secsDesired;
}

unsigned int Clock::getSecondsSinceMidnight(time_t moment) {
  return (hour(moment) * 3600) + (minute(moment) * 60) + (second(moment));
}

unsigned int Clock::getSecondsSinceMidnight(const char* timeChr)
{
  char test[9];
  strcpy(test, timeChr);
  char * pt;
  pt = strtok(test, ":");
  int hours = atoi(pt);
  pt = strtok(NULL, ":");
  int minutes = atoi(pt);
  pt = strtok(NULL, ":");
  int seconds = atoi(pt);
  return (hours * 3600) + (minutes * 60) + seconds;
}

void Clock::getTimeChr(char* out, time_t moment) {
  char hr[3] = "00";
  printDigits(hr, hour(moment));
  char min[3] = "00";
  printDigits(min, minute(moment));
  char sec[3] = "00";
  printDigits(sec, second(moment));
  snprintf(out, 9, "%s:%s:%s", hr, min, sec);
}

void Clock::printDigits(char* out, int digits) {
  if (digits < 10) {
    snprintf(out, 3, "%d%d", 0, digits);
  }
  else {
    snprintf(out, 3, "%d", digits);
  }
}