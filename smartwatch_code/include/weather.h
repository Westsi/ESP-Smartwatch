#ifndef WEATHER_H_
#define WEATHER_H_

#include <Arduino.h>
#include <string>

extern hourly_weather_t* hourlyEntries[168];
extern daily_weather_t* dailyEntries[7];

typedef struct hourlyitem {
    long timestamp;             // seconds from epoch
    float temp;                 // ºC
    float apparentTemp;         // ºC
    float relativeHumidity;     // %
    float cloudCover;           // %
    float precAmount;           // mm
    float precChance;           // %
    float uvIndex;
    float windVel;              // kmh
    int windDir;                // degrees from north (compass)
} hourly_weather_t;

typedef struct dailyitem {
    long timestamp;     // seconds from epoch
    long sunrise;       // seconds from epoch
    long sunset;        // seconds from epoch
    float maxTemp;      // ºC
    float minTemp;      // ºC
} daily_weather_t;

hourly_weather_t parseHourlyWeatherInfo(std::string hwi);
daily_weather_t parseDailyWeatherInfo(std::string dwi);
void printAllWeatherInfo();

#endif