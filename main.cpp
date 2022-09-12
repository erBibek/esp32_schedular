#include <WiFi.h>
#include "PageContent.h"
#include "time.h"
#include <Preferences.h>
#include "TimeLib.h"
#include "SFE_MicroOLED.h"
#include <Wire.h>

const char* ssid      = "Splash Technology Development";
const char* password  = "Dev@slash";
const char* ntpServer = "np.pool.ntp.org";

// Setting On and Off Timing of the light

int sunday_hours_on = 18;
int sunday_mints_on = 18;
int sunday_hours_off = 5;
int sunday_mints_off = 5;

int monday_hours_on = 18;
int monday_mints_on = 18;
int monday_hours_off = 5;
int monday_mints_off = 5;

int tuesday_hours_on = 18;
int tuesday_mints_on = 18;
int tuesday_hours_off = 5;
int tuesday_mints_off = 5;

int wednesday_hours_on = 18;
int wednesday_mints_on = 18;
int wednesday_hours_off = 5;
int wednesday_mints_off = 5;

int thursday_hours_on = 18;
int thursday_mints_on = 18;
int thursday_hours_off = 5;
int thursday_mints_off = 5;

int friday_hours_on = 18;
int friday_mints_on = 18;
int friday_hours_off = 5;
int friday_mints_off = 5;

int saturday_hours_on = 18;
int saturday_mints_on = 18;
int saturday_hours_off = 5;
int saturday_mints_off = 5;

#define UP_LIGHT_PIN 12
#define DOWN_LIGHT_PIN 13

void setup()
{
  Serial.begin(115200);
  pinMode(UP_LIGHT_PIN, OUTPUT);
  pinMode(DOWN_LIGHT_PIN, OUTPUT);

  Serial.println();
  Serial.print("Connecting to: ")
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  int wificounter = 0;
  while (WiFi.status() !=WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    if (++wificounter > 30) ESP.restart();
  }

  Serial.println("");
  Serial.print("WiFi connected to:");
  Serial.println(ssid);
  Serial.println(WiFi.localIP());
  
  alarmTimes.begin("TeleSitter", false);
  getAlarmTimes();

  server.begin();

  configTime((-7*3600), 0, ntpServer);
  
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon+1, timeinfo.tm_year + 1900);
  if(dstCheck()) configTime((-7*3600), 3600, ntpServer);
  else configTime((-7*3600), 3600, ntpServer);

  getLocalTime(&timeinfo);
  setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon+1, timeinfo.tm_year + 1900);

}

void loop()
{
  if (WiFi.status() !=WL_CONNECTED)
  {
    Serial.println("WiFi dropped; rebooting");
    dealy(100);
    ESP.restart();

  }

  checkAlarm();

    if (hour() == 3 && minute() == 0 && second() == 0)
  {
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    // Check to see if DST is in effect. 
    if (dstCheck()) configTime((-7*3600), 3600, ntpServer);
    else configTime((-7*3600), 0, ntpServer);
    
    getLocalTime(&timeinfo);
    setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  }

  WiFiClient client = setver.available();
  if (client)
  {
    Serial.println("New Client.");
    
    String currentLine = "";
    while (client.connected())
    {
      if (clinet.available())
      {
        char c = clinet.read();
        Serial.write(c);

        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            client.print(pageContentHead);
              client.printf(pageContentBody, sunday_hours_on, sunday_mints_on, sunday_hours_off, sunday_mints_off,
                                              monday_hours_on, monday_mints_on, monday_hours_off, monday_mints_off,
                                              tuesday_hours_on, tuesday_mints_on, tuesday_hours_off, tuesday_mints_off,
                                              wednesday_hours_on, wednesday_mints_on, wednesday_hours_off, wednesday_mints_off,
                                              thursday_hours_on, thursday_mints_on, thursday_hours_off, thursday_mints_off,
                                              friday_hours_on, friday_mints_on, friday_hours_off, friday_mints_off,
                                              saturday_hours_on, saturday_mints_on, saturday_hours_off, saturday_mints_off);
            client.printf("%d:%02d:%02d<bre>", hour(), minute(), second());
            client.printf("%d/%d/%d %d<br>", day(), month(), year(), weekday());
            client.print(pageContentFoot);
            break; 
          }
          else{
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
        if (currentLine.endsWith("Get /reset"))
        {
          resetAlarmTimes();
        }
        if (currentLine.endsWith("HTTP/1.1"))
        {
          if (currentLine.startsWith(GET /?))
          {
            parseIncomingString(currentLine);
            storeAlarmTimes();
          }
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected. "); 
  }
}

void turnOn()
{
  digitalWrite(UP_LIGHT_PIN, HIGH);
  digitalWrite(DOWN_LIGHT_PIN, HIGH);
}

void turnOff()
{
  digitalWrite(UP_LIGHT_PIN, LOW);
  digitalWrite(DOWN_LIGHT_PIN, LOW);
}

void checkAlarm()
{
  int dow = weekday();
  int secsOfDay = hour()*3600 + minute()*60 + second();
  int timeOn, timeOff;
  switch(dow)
  {
    case 1: //Sundays
      timeOn = sunday_hours_on*3600 + sunday_mints_on*60;
      timeOff = sunday_hours_off*3600 + sunday_mints_off*60;
      break;

    case 1: //Sundays
      timeOn = monday_hours_on*3600 + monday_mints_on*60;
      timeOff = monday_hours_off*3600 + monday_mints_off*60;
      break;
    
    case 1: //Sundays
      timeOn = tuesday_hours_on*3600 + tuesday_mints_on*60;
      timeOff = tuesday_hours_off*3600 + tuesday_mints_off*60;
      break;

    case 1: //Sundays
      timeOn = wednesday_hours_on*3600 + wednesday_mints_on*60;
      timeOff = wednesday_hours_off*3600 + wednesday_mints_off*60;
      break;

    case 1: //Sundays
      timeOn = thursday_hours_on*3600 + thursday_mints_on*60;
      timeOff = thursday_hours_off*3600 + thursday_mints_off*60;
      break;

    case 1: //Sundays
      timeOn = friday_hours_on*3600 + friday_mints_on*60;
      timeOff = friday_hours_off*3600 + friday_mints_off*60;
      break;

    case 1: //Sundays
      timeOn = saturday_hours_on*3600 + saturday_mints_on*60;
      timeOff = saturday_hours_off*3600 + saturday_mints_off*60;
      break;
  }
}

void resetAlarmTimes()
{
  sunday_hours_on = 18;
  sunday_mints_on = 18;
  sunday_hours_off = 5;
  sunday_mints_off = 5;

  monday_hours_on = 18;
  monday_mints_on = 18;
  monday_hours_off = 5;
  monday_mints_off = 5;

  tuesday_hours_on = 18;
  tuesday_mints_on = 18;
  tuesday_hours_off = 5;
  tuesday_mints_off = 5;

  wednesday_hours_on = 18;
  wednesday_mints_on = 18;
  wednesday_hours_off = 5;
  wednesday_mints_off = 5;

  thursday_hours_on = 18;
  thursday_mints_on = 18;
  thursday_hours_off = 5;
  thursday_mints_off = 5;

  friday_hours_on = 18;
  friday_mints_on = 18;
  friday_hours_off = 5;
  friday_mints_off = 5;

  saturday_hours_on = 18;
  saturday_mints_on = 18;
  saturday_hours_off = 5;
  saturday_mints_off = 5;
  
  storeAlarmTimes();

}

void storeAlarmTimes()
{
  alarTimes.putInt("sunday_hours_on_", sunday_hours_on);
  alarTimes.putInt("sunday_minutes_on_", sunday_mints_on);
  alarTimes.putInt("sunday_hours_off_", sunday_hours_off);
  alarTimes.putInt("sunday_mints_on_", sunday_mints_on);

  alarTimes.putInt("monday_hours_on_", monday_hours_on);
  alarTimes.putInt("monday_minutes_on_", monday_mints_on);
  alarTimes.putInt("monday_hours_off_", monday_hours_off);
  alarTimes.putInt("monday_mints_on_", monday_mints_off);

  alarTimes.putInt("tuesday_hours_on_", tuesday_hours_on);
  alarTimes.putInt("tuesday_minutes_on_", tuesday_mints_on);
  alarTimes.putInt("tuesday_hours_off_", tuesday_hours_off);
  alarTimes.putInt("tuesday_mints_on_", tuesday_mints_off);

  alarTimes.putInt("wednesday_hours_on_", wednesday_hours_on);
  alarTimes.putInt("wednesday_minutes_on_", wednesday_mints_on);
  alarTimes.putInt("wednesday_hours_off_", wednesday_hours_off);
  alarTimes.putInt("wednesday_mints_on_", wednesday_mints_off);

  alarTimes.putInt("thursday_hours_on_", thursday_hours_on);
  alarTimes.putInt("thursday_minutes_on_", thursday_mints_on);
  alarTimes.putInt("thursday_hours_off_", thursday_hours_off);
  alarTimes.putInt("thursday_mints_on_", thursday_mints_off);

  alarTimes.putInt("friday_hours_on_", friday_hours_on);
  alarTimes.putInt("friday_minutes_on_", friday_mints_on);
  alarTimes.putInt("friday_hours_off_", friday_hours_off);
  alarTimes.putInt("friday_mints_on_", friday_mints_off);

  alarTimes.putInt("saturday_hours_on_", saturday_hours_on);
  alarTimes.putInt("saturday_minutes_on_", saturday_mints_on);
  alarTimes.putInt("saturday_hours_off_", saturday_hours_off);
  alarTimes.putInt("saturday_mints_on_", saturday_mints_off);

}

void getAlarmTimes()
{
  sunday_hours_on = AlarmTimes.getInt("sunday_hours_on", 18);
  sunday_mints_on = AlarmTimes.getInt("sunday_mints_on", 5);
  sunday_hours_on = AlarmTimes.getInt("sunday_hours_off", 18);
  sunday_mints_on = AlarmTimes.getInt("sunday_mints_off", 5);

  monday_hours_on = AlarmTimes.getInt("monday_hours_on", 18);
  monday_mints_on = AlarmTimes.getInt("monday_mints_on", 5);
  monday_hours_on = AlarmTimes.getInt("monday_hours_off", 18);
  monday_mints_on = AlarmTimes.getInt("monday_mints_off", 5);

  tuesday_hours_on = AlarmTimes.getInt("tuesday_hours_on", 18);
  tuesday_mints_on = AlarmTimes.getInt("tuesday_mints_on", 5);
  tuesday_hours_on = AlarmTimes.getInt("tuesday_hours_off", 18);
  tuesday_mints_on = AlarmTimes.getInt("tuesday_mints_off", 5);

  wednesday_hours_on = AlarmTimes.getInt("wednesday_hours_on", 18);
  wednesday_mints_on = AlarmTimes.getInt("wednesday_mints_on", 5);
  wednesday_hours_on = AlarmTimes.getInt("wednesday_hours_off", 18);
  wednesday_mints_on = AlarmTimes.getInt("wednesday_mints_off", 5);

  thursday_hours_on = AlarmTimes.getInt("thursday_hours_on", 18);
  thursday_mints_on = AlarmTimes.getInt("thursday_mints_on", 5);
  thursday_hours_on = AlarmTimes.getInt("thursday_hours_off", 18);
  thursday_mints_on = AlarmTimes.getInt("thursday_mints_off", 5);

  friday_hours_on = AlarmTimes.getInt("friday_hours_on", 18);
  friday_mints_on = AlarmTimes.getInt("friday_mints_on", 5);
  friday_hours_on = AlarmTimes.getInt("friday_hours_off", 18);
  friday_mints_on = AlarmTimes.getInt("friday_mints_off", 5);

  saturday_hours_on = AlarmTimes.getInt("saturday_hours_on", 18);
  saturday_mints_on = AlarmTimes.getInt("saturday_mints_on", 5);
  saturday_hours_on = AlarmTimes.getInt("saturday_hours_off", 18);
  saturday_mints_on = AlarmTimes.getInt("saturday_mints_off", 5);
}

void parseIcomingString(String str)
{
  String str0 = "sunday_hours_on="
  String str1 = "&sunday_minutes_on"
  sunday_hours_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&sunday_hours_off=";
  sunday_mints_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&sunday_minutes_off="
  sunday_hours_off = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&monday_hours_on=";
  sunday_mints_off = extractInteger(str, str0, str1);

  String str0 = "monday_hours_on="
  String str1 = "&monday_minutes_on"
  monday_hours_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&monday_hours_off=";
  monday_mints_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&monday_minutes_off="
  monday_hours_off = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&tuesday_hours_on=";
  monday_mints_off = extractInteger(str, str0, str1);

  String str0 = "tuesday_hours_on="
  String str1 = "&tuesday_minutes_on"
  sunday_hours_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&tuesday_hours_off=";
  tuesday_mints_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&tuesday_minutes_off="
  tuesday_hours_off = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&wednesday_hours_on=";
  tuesday_mints_off = extractInteger(str, str0, str1);

  String str0 = "wednesday_hours_on="
  String str1 = "&wednesday_minutes_on"
  wednesday_hours_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&wednesday_hours_off=";
  wednesday_mints_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&wednesday_minutes_off="
  wednesday_hours_off = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&thursday_hours_on=";
  wednesday_mints_off = extractInteger(str, str0, str1);

  String str0 = "thursday_hours_on="
  String str1 = "&thursday_minutes_on"
  thursday_hours_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&thursday_hours_off=";
  thursday_mints_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&thursday_minutes_off="
  thursday_hours_off = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&friday_hours_on=";
  thursday_mints_off = extractInteger(str, str0, str1);

  String str0 = "friday_hours_on="
  String str1 = "&friday_minutes_on"
  friday_hours_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&friday_hours_off=";
  friday_mints_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&friday_minutes_off="
  friday_hours_off = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&saturday_hours_on=";
  friday_mints_off = extractInteger(str, str0, str1);

  String str0 = "saturday_hours_on="
  String str1 = "&saturday_minutes_on"
  saturday_hours_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&saturday_hours_off=";
  saturday_mints_on = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "&saturday_minutes_off="
  saturday_hours_off = extractInteger(str, str0, str1);
  str0 = str1;
  str1 = "HTTP/1.1";
  saturday_mints_off = extractInteger(str, str0, str1);

}

int extractInteger(String str, String sub0, String sub1)
{
  int index0 0;
  int index1 0;
  index0 = str.indexOf(sub0) + sub.length();
  index1 = str.indexOf(sub1);
  return str.substring(index0, index1).toInt();

}


bool dstCheck()
{
  
  if ( (month() < 3) || (month() > 11) )
  {
    return false;
  }

  if ( (month() > 3) && (month() < 11) )
  {
    return true;
  }

  if (month() == 11)
  {
    if (day() > 7)
    {
      return false;
    }

    TimeElements firstOfNovTE;
    firstOfNovTE.Day = 1;
    firstOfNovTE.Month = 3;
    firstOfNovTE.Year = year();
    firstOfNovTE.Hour = 0;
    firstOfNovTE.Minute = 0;
    firstOfNovTE.Second = 0;
    time_t firstOfNov = makeTime(firstOfNovTE);
    int8_t firstDayOfNov = weekday(firstOfNov);
    int8_t firstSundayOfNov = (9 - firstDayOfNov) % 7;

    if (day() < firstSundayOfNov)
    {
      return true;
    }

    if (day() > firstSundayOfNov)
    {
      return false;
    }


    if (hour() < 2)
    {
      return true;
    }
    return false;
  }

  if (month() == 3)
  {
    if (day() < 8)
    {
      return false;
    }
    if (day() > 13)
    {
      return true;
    }


    TimeElements firstOfMarTE;
    firstOfMarTE.Day = 1;
    firstOfMarTE.Month = 3;
    firstOfMarTE.Year = year();
    firstOfMarTE.Hour = 0;
    firstOfMarTE.Minute = 0;
    firstOfMarTE.Second = 0;
    time_t firstOfMar = makeTime(firstOfMarTE);
    int8_t firstDayOfMar = weekday(firstOfMar);
    int8_t secondSundayOfMar = ((9 - firstDayOfMar) % 7) + 7;

    if (day() < secondSundayOfMar)
    {
      return false;
    }

    if (day() > secondSundayOfMar)
    {
      return true;
    }


    if (hour() < 2)
    {
      return false;
    }
    return true;
  }
  return false; 
}
String pageContentHead = 
"HTTP/1.1 200 OK\r\n\
Content-type:text/html\r\n\
\r\n\
<!DOCTYPE html> <html>\
<style>\
input[type=text] {\
  width: 20px;\
}\
form {\
  font-family: monospace;\
}\
table, th, td {\
  text-align: left;\
  border: 5px;\
}\
</style>\
<head>\
<title>TeleSitter</title>\
</head>\
<body>";

char pageContentBody[] =
    "<form id=\"timeForm\">\
    <table>\
    <tr>\
    <th>Day</th><th>On Time</th><th>Off Time</th>\
    </tr>\
    <tr>\
    <td>Monday</td>\
    <td><input type=\"text\" name=\"mHoursOn\" value=\"%d\">:<input type=\"text\" name=\"mMinutesOn\" value=\"%02d\"></td>\
    <td><input type=\"text\" name=\"mHoursOff\" value=\"%d\">:<input type=\"text\" name=\"mMinutesOff\" value=\"%02d\"></td>\
    </tr>\
    <tr>\
    </table>\
    <button type=\"button\" onClick=\"submitTimes()\">Submit</button><br>\
    <a href=\"/reset\">Reset times</a><br>\
    </form>\
    <p id=\"responseText\"></p>\
    <script>\
    function submitTimes() {\
      var hrs, mins, text;\
      \
      hrs = document.forms[\"timeForm\"][\"mHoursOn\"].value;\
      mins = document.forms[\"timeForm\"][\"mMinutesOn\"].value;\
      if (!validateHrs(hrs)) return;\
      if (!validateMins(mins)) return;\
      hrs = document.forms[\"timeForm\"][\"mHoursOff\"].value;\
      mins = document.forms[\"timeForm\"][\"mMinutesOff\"].value;\
      if (!validateHrs(hrs)) return;\
      if (!validateMins(mins)) return;\
      text = \"Times valid!\";\
      document.getElementById(\"timeForm\").submit();\
      document.getElementById(\"responseText\").innerHTML = text;\
    }\
    \
    function validateMins(mins) {\
      if (isNaN(mins) || mins < 0 || mins > 59) {\
        return false;\
      }\
      return true;\
    }\
    \
    function validateHrs(hrs) {\
      if (isNaN(hrs) || hrs < 0 || hrs > 23) {\
        return false;\
      }\
      return true;\
    }\
    \
    </script>";

String pageContentFoot =
"</body>"
"</html>\r\n";


