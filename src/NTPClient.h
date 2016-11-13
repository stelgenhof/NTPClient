/*
   This file is part of the NTPClient library.

   Copyright 2016 Sacha Telgenhof (stelgenhof@gmail.com). All rights reserved.

   For the full copyright and license information, please view the LICENSE
   file that was distributed with this source code.
 */

#ifndef _NTPClient_h
#define _NTPClient_h

#ifndef ESP8266
#error “This library only supports boards with the ESP8266 MCU.”
#endif

#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <functional>

extern "C" {
    #include "sntp.h"
}

using namespace std;

#define DEFAULT_NTP_SERVER "pool.ntp.org"  // Default NTP server. Recommended to use a NTP server closer to your location.
#define DEFAULT_POLLING_INTERVAL 1800  // Default polling interval (seconds)
#define NTP_MINIMUM_INTERVAL 15  // Minimum polling interval (seconds)
#define NTP_SHORT_INTERVAL 5 // Polling interval when synchronization has not been achieved yet
#define NTP_SERVERS_MAXIMUM 3 // Maximum number of supported NTP servers

// Events used for the callback function
typedef enum {
        NTP_EVENT_INIT, // Start of NTP synchronization
        NTP_EVENT_STOP, // End of NTP synchronization
        NTP_EVENT_NO_RESPONSE, // No response from NTP server
        NTP_EVENT_SYNCHRONIZED, // Time successfully received from NTP server
} NTPSyncEvent_t;

typedef std::function<void (NTPSyncEvent_t)> onSyncEvent_t;

// Timezones by UTC-offset (in seconds). The underscore represents negative
// mark, as hyphens can not be used.
typedef enum __attribute__((packed)) {
        UTC_1200 = -43200,
        UTC_1100 = -39600,
        UTC_1000 = -36000,
        UTC_0930 = -34200,
        UTC_0900 = -32400,
        UTC_0800 = -28800,
        UTC_0700 = -25200,
        UTC_0600 = -21600,
        UTC_0500 = -18000,
        UTC_0400 = -14400,
        UTC_0330 = -12600,
        UTC_0300 = -10800,
        UTC_0200 = -7200,
        UTC_0100 = -3600,
        UTC      = 0,
        UTC0100  = 3600,
        UTC0200  = 7200,
        UTC0300  = 10800,
        UTC0330  = 12600,
        UTC0400  = 14400,
        UTC0430  = 16200,
        UTC0500  = 18000,
        UTC0530  = 19800,
        UTC0545  = 20700,
        UTC0600  = 21600,
        UTC0630  = 23400,
        UTC0700  = 25200,
        UTC0800  = 28800,
        UTC0830  = 30600,
        UTC0845  = 31500,
        UTC0900  = 32400,
        UTC0930  = 34200,
        UTC1000  = 36000,
        UTC1030  = 37800,
        UTC1100  = 39600,
        UTC1200  = 43200,
        UTC1245  = 45900,
        UTC1300  = 46800,
        UTC1400  = 50400,
} tz_utc_offsets_t;

class NTPClient {

public:
        NTPClient();

        /**
         * @brief Sets the name of a NTP server to use.
         *
         * This function sets the name of a NTP Server to use for time synchronization.
         * By default the first (0) server is set.  The ESP8266 supports 3 SNTP servers at most (0 ~ 2),
         * whereby 0 is the main server, and servers identified at index 1 and 2 are backup NTP servers.
         *
         * @param  server the host name of the NTP server to use for time synchronization
         * @param  idx    idx NTP server index (0 ~ 2). If no index is given, the first server is set.
         * @return        true if successful
         */
        bool setNTPServer(char *server, uint8_t idx = 0);

        /**
         * @brief Gets the name of the configured NTP server.
         *
         * This function gets the name of the configured NTP Server set at index 'idx'.
         * By default the first (0) server is returned.  The ESP8266 supports 3 SNTP servers at most (0 ~ 2),
         * whereby 0 is the main server, and servers identified at index 1 and 2 are backup NTP servers.
         *
         * @param  idx NTP server index (0 ~ 2). If no index is given, the first server name is returned.
         * @return the name of the configured NTP Server set at index 'idx'.
         */
        char * getNTPServer(uint8_t idx = 0);

        /**
         * @brief Starts the NTP time synchronization.
         *
         * @param  server    the host name of the NTP server to use for time synchronization
         * @param  utcOffset the UTC Offset to use (i.e. timezone). If empty, UTC will be used.
         * @return           true if successful
         */
        bool init(char *server = (char *)DEFAULT_NTP_SERVER, tz_utc_offsets_t utcOffset = UTC);

        /**
         * @brief Stops the time synchronization
         *
         * @return true if successful
         */
        bool stop();

        /**
         * @brief Set a callback function that triggers after synchronization request.
         *
         * @param cb handle to callback function
         */
        void onSyncEvent(onSyncEvent_t cb);

        /**
         * @brief Sets how often time needs to be synchronized.
         *
         * This function sets the polling interval of the synchronization
         * process (in seconds). The interval cannot be set lower than 15
         * seconds, as this is the advised minimum interval for NTP servers.
         *
         * In case time hasn't been synchronized yet, a faster polling
         * frequency will be used to retrieve a time update quicker.
         *
         * @param interval synchronization interval (in seconds)
         */
        void setPollingInterval(unsigned int interval);

        /**
         * @brief Gets the current configured polling interval (in seconds).
         *
         * @return the current configured polling interval (in seconds).
         */
        unsigned int getPollingInterval();

        /**
         * @brief Gets last succesful synchronization time.
         *
         * @return last succesful synchronization time in UNIX time format (seconds).
         *         0 means synchronization was not (yet) succesful.
         */
        time_t getLastSync();

        /**
         * @brief Gets first succesful synchronization time.
         *
         * @return first succesful synchronization time in UNIX time format (seconds).
         *         0 means synchronization hasn't taken place yet.
         */
        time_t getFirstSync();

        /**
         * @brief Helper function to display date/time in human readable format.
         *
         * This function is used to present a time in human readable format.
         * Time and date are rendered in ISO8601 format (yyyy/mm/ddThh:mm:ss)
         *
         * @param  _time the time element to be displayed in text format
         * @return       Time and date rendered in ISO8601 format (yyyy/mm/ddThh:mm:ss)
         */
        const char* getTimeDate(time_t _time);

private:
        unsigned int long_interval; // Polling interval for periodic time synchronization
        tz_utc_offsets_t utc_offset; // UTC Offset. Defaults to UTC
        time_t last_poll; // Time of last succesfull poll
        time_t first_poll; // Time of first successful poll after boot
        onSyncEvent_t onNTPSyncEvent; // Event handler callback function

        /**
         * @brief Sends a request to the NTP server.
         *
         * This function sends a request to the configured NTP Server. It will
         * return a time in UNIX time format.
         *
         * @return time in UNIX time format.
         */
        time_t getTime();

        /**
         * @brief Gets the time from an NTP server.
         *
         * This function sends a request to the configured NTP Server. It will
         * return a time in UNIX time format. It is set as the synchronization
         * handler for Time Library.
         *
         * @return time in UNIX time format.
         */
        static time_t s_getTime();
};

extern NTPClient NTP;

#endif
