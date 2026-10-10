#include "weather.h"

#include <Arduino.h>
#include <string>

hourly_weather_t* hourlyEntries[168];
daily_weather_t* dailyEntries[7];

int hourlyIndex = 0;
int dailyIndex = 0;


hourly_weather_t parseHourlyWeatherInfo(std::string hwi) {
// "TIME%d~~TEMP%f~~CLOUD%f~~RELHUM%f~~APPTEMP%f~~PREC%f~~PRECPROB%f~~UV%f~~WINDVEL%f~~WINDDIR%d~~END_HOUR~~",
//                                 hTimes.getLong(i),
//                                 hTemps.getDouble(i),
//                                 hCloud.getDouble(i),
//                                 hRelHum.getDouble(i),
//                                 hAppTemp.getDouble(i),
//                                 hPrec.getDouble(i),
//                                 hPrecProb.getDouble(i),
//                                 hUVInd.getDouble(i),
//                                 hWindVel.getDouble(i),
//                                 hWindDir.getInt(i)));

    hourly_weather_t* hw = hourlyEntries[hourlyIndex++];
    int timeIdx = hwi.find("TIME") + 4;
    int timeEnd = hwi.find("~~");
    hw->timestamp = std::stol(hwi.substr(timeIdx, timeEnd));
    hwi = hwi.substr(timeEnd+2);
    // int tempEnd = hwi.find("~~", timeEnd+2);
    // int cloudEnd = hwi.find("~~", tempEnd+2);
    // int humidEnd = hwi.find("~~", cloudEnd+2);
    // int appTempEnd = hwi.find("~~", humidEnd+2);
    // int precEnd = hwi.find("~~", appTempEnd+2);
    // int precProbEnd = hwi.find("~~", precEnd+2);
    // int uvIndEnd = hwi.find("~~", precProbEnd+2);
    // int windVelEnd = hwi.find("~~", uvIndEnd+2);
    // int windDirEnd = hwi.find("~~", windVelEnd+2);

    
    
}

daily_weather_t parseDailyWeatherInfo(std::string dwi) {

}

void printAllWeatherInfo() {

}
