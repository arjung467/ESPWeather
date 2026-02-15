#include "math.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <memory>
#include <time.h>
#include "weather.hpp"

const char *root_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "63ZTGI0RmLo=\n"
    "-----END CERTIFICATE-----\n";

Weather::Weather(float latitude, float longitude)
{
  this->num_hours = ESPWeatherNumHours;
  this->user_agent = ESPWeatherUserAgent;
  this->longitude = longitude;
  this->latitude = latitude;
  this->altitude = 0;
  this->local_time = new tm;
  this->expired_time = new tm;
  this->temperature = new WeatherData(this->num_hours);
  this->dew_point = new WeatherData(this->num_hours);
  this->precipitation = new WeatherData(this->num_hours);
  this->wind_speeds = new WeatherData(this->num_hours);
  this->wind_direction = new WeatherData(this->num_hours);
  this->air_pressure = new WeatherData(this->num_hours);
  this->cloudiness = new WeatherData(this->num_hours);
  this->relative_humidity = new WeatherData(this->num_hours);
  this->last_modified = "";
  this->symbol_code_next_1h = "";
  this->symbol_code_next_12h = "";
  this->symbol_code_next_6h = "";
  this->utc_offset = 0;
  this->daylight_saving = false;
  this->expired_time->tm_year = 0; // Years since 1900
  this->expired_time->tm_mon = 0;  // Months since January (0-11)
  this->expired_time->tm_mday = 0; // Day of the month
  this->expired_time->tm_hour = 0;
  this->expired_time->tm_min = 0;
  this->expired_time->tm_sec = 0;
}
Weather::Weather(uint8_t num_hours, float latitude, float longitude)
{
  this->num_hours = num_hours;
  this->user_agent = ESPWeatherUserAgent;
  this->longitude = longitude;
  this->latitude = latitude;
  this->altitude = 0;
  this->local_time = new tm;
  this->expired_time = new tm;
  this->temperature = new WeatherData(this->num_hours);
  this->dew_point = new WeatherData(this->num_hours);
  this->precipitation = new WeatherData(this->num_hours);
  this->wind_speeds = new WeatherData(this->num_hours);
  this->wind_direction = new WeatherData(this->num_hours);
  this->air_pressure = new WeatherData(this->num_hours);
  this->cloudiness = new WeatherData(this->num_hours);
  this->relative_humidity = new WeatherData(this->num_hours);
  this->last_modified = "";
  this->symbol_code_next_1h = "";
  this->symbol_code_next_12h = "";
  this->symbol_code_next_6h = "";
  this->utc_offset = 0;
  this->daylight_saving = false;
  this->expired_time->tm_year = 0; // Years since 1900
  this->expired_time->tm_mon = 0;  // Months since January (0-11)
  this->expired_time->tm_mday = 0; // Day of the month
  this->expired_time->tm_hour = 0;
  this->expired_time->tm_min = 0;
  this->expired_time->tm_sec = 0;
}
Weather::Weather(float latitude, float longitude, uint16_t altitude)
{
  this->num_hours = ESPWeatherNumHours;
  this->user_agent = ESPWeatherUserAgent;
  this->longitude = longitude;
  this->latitude = latitude;
  this->altitude = altitude;
  this->local_time = new tm;
  this->expired_time = new tm;
  this->temperature = new WeatherData(this->num_hours);
  this->dew_point = new WeatherData(this->num_hours);
  this->precipitation = new WeatherData(this->num_hours);
  this->wind_speeds = new WeatherData(this->num_hours);
  this->wind_direction = new WeatherData(this->num_hours);
  this->air_pressure = new WeatherData(this->num_hours);
  this->cloudiness = new WeatherData(this->num_hours);
  this->relative_humidity = new WeatherData(this->num_hours);
  this->last_modified = "";
  this->symbol_code_next_1h = "";
  this->symbol_code_next_12h = "";
  this->symbol_code_next_6h = "";
  this->daylight_saving = false;
  this->utc_offset = 0;
  this->expired_time->tm_year = 0; // Years since 1900
  this->expired_time->tm_mon = 0;  // Months since January (0-11)
  this->expired_time->tm_mday = 0; // Day of the month
  this->expired_time->tm_hour = 0;
  this->expired_time->tm_min = 0;
  this->expired_time->tm_sec = 0;
}
Weather::Weather(uint8_t num_hours, float latitude, float longitude, uint16_t altitude)
{
  this->num_hours = num_hours;
  this->user_agent = ESPWeatherUserAgent;
  this->longitude = longitude;
  this->latitude = latitude;
  this->altitude = altitude;
  this->local_time = new tm;
  this->expired_time = new tm;
  this->temperature = new WeatherData(this->num_hours);
  this->dew_point = new WeatherData(this->num_hours);
  this->precipitation = new WeatherData(this->num_hours);
  this->wind_speeds = new WeatherData(this->num_hours);
  this->wind_direction = new WeatherData(this->num_hours);
  this->air_pressure = new WeatherData(this->num_hours);
  this->cloudiness = new WeatherData(this->num_hours);
  this->relative_humidity = new WeatherData(this->num_hours);
  this->last_modified = "";
  this->symbol_code_next_1h = "";
  this->symbol_code_next_12h = "";
  this->symbol_code_next_6h = "";
  this->daylight_saving = false;
  this->utc_offset = 0;
  this->expired_time->tm_year = 0; // Years since 1900
  this->expired_time->tm_mon = 0;  // Months since January (0-11)
  this->expired_time->tm_mday = 0; // Day of the month
  this->expired_time->tm_hour = 0;
  this->expired_time->tm_min = 0;
  this->expired_time->tm_sec = 0;
}

void Weather::set_daylight_saving(bool daylight_saving)
{
  this->daylight_saving = daylight_saving;
}

void Weather::set_utc_offset(int8_t utc_offset)
{
  this->utc_offset = utc_offset;
}

Weather::~Weather()
{
  delete this->local_time;
  delete this->expired_time;
  delete this->temperature;
  delete this->dew_point;
  delete this->precipitation;
  delete this->wind_speeds;
  delete this->wind_direction;
  delete this->air_pressure;
  delete this->cloudiness;
  delete this->relative_humidity;
}

void Weather::update_location(float latitude, float longitude)
{
  this->longitude = longitude;
  this->latitude = latitude;
}
void Weather::update_location(float latitude, float longitude,
                              uint16_t altitude)
{
  this->longitude = longitude;
  this->latitude = latitude;
  this->altitude = altitude;
}

void Weather::update_data(void)
{
  std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);
  std::unique_ptr<char[]> buffer(new char[512]);
  client->setCACert(root_cert);
  HTTPClient https;
  sprintf(buffer.get(), "?lat=%.2f&lon=%.2f&altitude=%d", this->latitude,
          this->longitude, this->altitude);
  String url = (String)this->url + (String)buffer.get();
  https.begin(*client, url.c_str());
  https.setUserAgent(this->user_agent);
  if (!this->last_modified.isEmpty())
  {
    https.addHeader("If-Modifed-Since", this->last_modified);
  }
  const char *headerKeys[] = {"last-modified", "expires"};
  const size_t headerKeysCount = sizeof(headerKeys) / sizeof(headerKeys[0]);
  https.collectHeaders(headerKeys, headerKeysCount);

  JsonDocument filter;

  for (uint8_t i = 0; i < this->num_hours + 1; ++i)
  {
    filter["properties"]["timeseries"][i] = true;
    filter["properties"]["timeseries"][i]["data"]["instant"]["details"]
          ["air_temperature"] = true;
    filter["properties"]["timeseries"][i]["data"]["next_1_hours"]["details"]
          ["precipitation_amount"] = true;
    filter["properties"]["timeseries"][i]["data"]["instant"]["details"]
          ["wind_speed"] = true;
    filter["properties"]["timeseries"][i]["data"]["instant"]["details"]
          ["wind_from_direction"] = true;
    filter["properties"]["timeseries"][i]["data"]["instant"]["details"]
          ["air_pressure_at_sea_level"] = true;
    filter["properties"]["timeseries"][i]["data"]["instant"]["details"]
          ["cloud_area_fraction"] = true;
    filter["properties"]["timeseries"][i]["data"]["instant"]["details"]
          ["relative_humidity"] = true;
    filter["properties"]["timeseries"][i]["data"]["instant"]["details"]
          ["dew_point_temperature"] = true;
    if (i == 0)
    {
      filter["properties"]["timeseries"][i]["data"]["next_1_hours"]["summary"]
            ["symbol_code"] = true;
    }
  }

  int httpResponseCode = https.GET();
  String payload = "{}";
  if (httpResponseCode > 0)
  {
#ifdef DEBUG_WEATHER
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
#endif
    if ((!this->last_modified.isEmpty()) && httpResponseCode == 304)
    {
      int header_collected = https.headers();
#ifdef DEBUG_WEATHER
      Serial.println("Data unchanged. Nothing todo");
      Serial.print("Collected ");
      Serial.print(header_collected);
      Serial.println(" headers:");
#endif
      if (header_collected == 2)
      {
        this->last_modified = https.header("last-modified");
        String expires = https.header("expires");
        const char *expires_c = expires.c_str();
        char *end = strptime(expires_c, "%a, %d %b %Y %H:%M:%S GMT",
                             this->expired_time);
#ifdef DEBUG_WEATHER
        if ((end == NULL) || end != "\0")
        {
          Serial.print("Found remaining char: ");
          Serial.println(end);
        }
#endif
#ifdef DEBUG_WEATHER

        Serial.print("last-modified: ");
        Serial.println(this->last_modified);

        Serial.print("expires: ");
        Serial.println(expires);
#endif
      }
      return;
    }
  }
  else
  {
#ifdef DEBUG_WEATHER
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
#endif
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, https.getStream(), DeserializationOption::Filter(filter));

#ifdef DEBUG_WEATHER
  Serial.println("Starting deserialization");
#endif
  if (error)
  {

#ifdef DEBUG_WEATHER
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
#endif
    return;
  }
  if (!(doc.containsKey("properties")))
  {
    return;
  }
  if (!(doc["properties"].containsKey("timeseries")))
  {
    return;
  }
  JsonArray timeseries = doc["properties"]["timeseries"];
  float *temps = new float[this->num_hours + 1];
  float *precipitation = new float[this->num_hours + 1];
  float *wind_speeds = new float[this->num_hours + 1];
  float *wind_directions = new float[this->num_hours + 1];
  float *air_pressure = new float[this->num_hours + 1];
  float *cloudiness = new float[this->num_hours + 1];
  float *relative_humidity = new float[this->num_hours + 1];
  float *dew_point = new float[this->num_hours + 1];
  JsonObject current_timeseries_data;
  JsonObject current_timeseries_details;
  JsonObject current_timeseries_next_hour_details;
  if (timeseries[0]["data"].containsKey("next_1_hours"))
  {
    if (timeseries[0]["data"]["next_1_hours"].containsKey("summary"))
    {
      if (timeseries[0]["data"]["next_1_hours"]["summary"].containsKey(
              "symbol_code"))
      {
        this->symbol_code_next_1h =
            String((const char *)timeseries[0]["data"]["next_1_hours"]
                                           ["summary"]["symbol_code"]);
      }
    }
  }
  if (timeseries[0]["data"].containsKey("next_6_hours"))
  {
    if (timeseries[0]["data"]["next_6_hours"].containsKey("summary"))
    {
      if (timeseries[0]["data"]["next_6_hours"]["summary"].containsKey(
              "symbol_code"))
      {
        this->symbol_code_next_6h =
            String((const char *)timeseries[0]["data"]["next_6_hours"]
                                           ["summary"]["symbol_code"]);
      }
    }
  }
  if (timeseries[0]["data"].containsKey("next_12_hours"))
  {
    if (timeseries[0]["data"]["next_12_hours"].containsKey("summary"))
    {
      if (timeseries[0]["data"]["next_12_hours"]["summary"].containsKey(
              "symbol_code"))
      {
        this->symbol_code_next_12h =
            String((const char *)timeseries[0]["data"]["next_12_hours"]
                                           ["summary"]["symbol_code"]);
      }
    }
  }
  for (uint8_t i = 0; i < this->num_hours + 1; ++i)
  {
    current_timeseries_data = timeseries[i]["data"];
    current_timeseries_details = current_timeseries_data["instant"]["details"];
    current_timeseries_next_hour_details =
        current_timeseries_data["next_1_hours"]["details"];

    temps[i] = current_timeseries_details["air_temperature"];
    precipitation[i] =
        current_timeseries_next_hour_details["precipitation_amount"];
    wind_speeds[i] = current_timeseries_details["wind_speed"];
    wind_directions[i] = current_timeseries_details["wind_from_direction"];
    air_pressure[i] = current_timeseries_details["air_pressure_at_sea_level"];
    cloudiness[i] = current_timeseries_details["cloud_area_fraction"];
    relative_humidity[i] = current_timeseries_details["relative_humidity"];
    dew_point[i] = current_timeseries_details["dew_point_temperature"];
  }
  // Free resources
  this->temperature->update_vals(temps);
  this->precipitation->update_vals(precipitation);
  this->wind_speeds->update_vals(wind_speeds);
  this->wind_direction->update_vals(wind_directions);
  this->air_pressure->update_vals(air_pressure);
  this->cloudiness->update_vals(cloudiness);
  this->relative_humidity->update_vals(relative_humidity);
  this->dew_point->update_vals(dew_point);
  int header_collected = https.headers();
#ifdef DEBUG_WEATHER
  Serial.print("Collected ");
  Serial.print(header_collected);
  Serial.println(" headers:");
#endif
  if (header_collected == 2)
  {
    this->last_modified = https.header("last-modified");
    String expires = https.header("expires");
    const char *expires_c = expires.c_str();
    char *end =
        strptime(expires_c, "%a, %d %b %Y %H:%M:%S GMT", this->expired_time);
#ifdef DEBUG_WEATHER
    if ((end == NULL) || end != "\0")
    {
      Serial.print("Found remaining char: ");
      Serial.println(end);
    }
#endif

#ifdef DEBUG_WEATHER
    Serial.print("last-modified: ");
    Serial.println(this->last_modified);

    Serial.print("expires: ");
    Serial.println(expires);
    Serial.println(this->expired_time->tm_hour);
    Serial.println(this->expired_time->tm_min);
#endif
  }
  https.end();
  delete[] temps;
  delete[] precipitation;
  delete[] wind_speeds;
  delete[] wind_directions;
  delete[] air_pressure;
  delete[] cloudiness;
  delete[] relative_humidity;
  delete[] dew_point;
}

bool is_leap_year(int year)
{
  if (year % 4 != 0)
  {
    return false;
  }
  else if (year % 100 != 0)
  {
    return true;
  }
  else if (year % 400 != 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

bool Weather::is_expired(void)
{
  getLocalTime(this->local_time);
  int8_t day_add = 0;
  int8_t hour = this->expired_time->tm_hour + this->utc_offset +
                (int8_t)this->daylight_saving;
  int year = this->expired_time->tm_year;
  if (hour > 23)
  {
    day_add = 1;
    hour = (hour - 24);
  }
  else if (hour < 0)
  {
    day_add = -1;
    hour = 24 + hour;
  }
  int days_in_year = is_leap_year(year) ? 366 : 365;
  if (this->expired_time->tm_yday + day_add > days_in_year)
  {
    year += 1;
  }

  // Check if the expiration year is less than the current year or if the
  // expiration day of the year (with day adjustment) is less than the current
  // day of the year
  if ((year < this->local_time->tm_year) ||
      ((this->expired_time->tm_yday + day_add) < this->local_time->tm_yday))
  {
    return true;
  }

  // Check if the expiration hour is less than the current hour
  if (hour < this->local_time->tm_hour)
  {
    return true;
  }
  if ((hour == this->local_time->tm_hour) &&
      (this->expired_time->tm_min < this->local_time->tm_min))
  {
    return true;
  }
  return false;
}

WeatherData *Weather::get_temperature() { return this->temperature; }
WeatherData *Weather::get_precipitation() { return this->precipitation; }
WeatherData *Weather::get_air_pressure() { return this->air_pressure; }

WeatherData *Weather::get_relative_humidity()
{
  return this->relative_humidity;
}

WeatherData *Weather::get_wind_speeds() { return this->wind_speeds; }

WeatherData *Weather::get_wind_direction() { return this->wind_direction; }

WeatherData *Weather::get_cloudiness() { return this->cloudiness; }

WeatherData *Weather::get_dew_point() { return this->dew_point; }

tm *Weather::getExpiredTime() { return this->expired_time; }
String Weather::get_symbol_code_next_1h() { return this->symbol_code_next_1h; }
String Weather::get_symbol_code_next_12h()
{
  return this->symbol_code_next_12h;
}
String Weather::get_symbol_code_next_6h() { return this->symbol_code_next_6h; }
