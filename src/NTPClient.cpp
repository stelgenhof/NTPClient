/*
  This file is part of the NTPClient library.

  Copyright 2016 Sacha Telgenhof (stelgenhof@gmail.com). All rights reserved.

  For the full copyright and license information, please view the LICENSE.md
  file that was distributed with this source code.
 */

#include "NTPClient.h"

#ifdef DEBUG_NTPCLIENT
#define DEBUGLOG(...) os_printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

NTPClient NTP;

NTPClient::NTPClient() {
  // Initialize class members
  utc_offset = UTC;
  last_poll = 0;
  first_poll = 0;
}

bool NTPClient::setNTPServer(char *server, uint8_t idx) {
  // Supports 3 SNTP servers at most (0 ~ 2)
  if (idx > (NTP_SERVERS_MAXIMUM - 1)) {
    return false;
  }
  sntp_stop();
  sntp_setservername(idx, server);
  DEBUGLOG("[NTP] NTP Server #%d set to: %s.\n", idx, server);
  sntp_init();

  return true;
}

char *NTPClient::getNTPServer(uint8_t idx) {
  // Supports 3 SNTP servers at most (0 ~ 2)
  if (idx > (NTP_SERVERS_MAXIMUM - 1)) {
    return {};
  }

  return sntp_getservername(idx);
}

bool NTPClient::init(char *server, tz_utc_offsets_t utcOffset) {
  if (!setNTPServer(server)) {
    return false;
  }

  // Adjust for UTC Offset. Need to set timezone to 0 (UTC) since by default the
  // timezone is set to UTC+0800
  sntp_stop();
  if (sntp_set_timezone(0)) {
    sntp_init();
    utc_offset = utcOffset;
  }

  last_poll = 0;

  setPollingInterval(DEFAULT_POLLING_INTERVAL);
  setSyncProvider(s_getTime);

  DEBUGLOG("[NTP] Time synchronization started.\n");

  if (onNTPSyncEvent != NULL) {
    onNTPSyncEvent(NTP_EVENT_INIT);
  }

  return true;
}

bool NTPClient::stop() {
  setSyncProvider(NULL);
  sntp_stop();

  DEBUGLOG("[NTP] Time synchronization stopped.\n");

  if (onNTPSyncEvent != NULL) {
    onNTPSyncEvent(NTP_EVENT_STOP);
  }

  return true;
}

void NTPClient::onSyncEvent(onSyncEvent_t cb) { onNTPSyncEvent = cb; }

void NTPClient::setPollingInterval(unsigned int interval) {
  // Don't do anything if new interval is not different from current
  if (long_interval == interval) {
    return;
  }

  long_interval =
      (interval < NTP_MINIMUM_INTERVAL) ? NTP_MINIMUM_INTERVAL : interval;

  if (timeStatus() != timeSet) {
    setSyncInterval(NTP_SHORT_INTERVAL); // Get synchronized immediately
  } else {
    setSyncInterval(interval);
  }
  DEBUGLOG("[NTP] Polling interval set to: %d seconds.\n", long_interval);
}

unsigned int NTPClient::getPollingInterval() { return long_interval; }

time_t NTPClient::getLastSync() { return last_poll; }

time_t NTPClient::getFirstSync() { return first_poll; }

const char *NTPClient::getTimeDate(time_t tm) {
  char *dt = new char[20];
  char buf[20];

  snprintf(buf, sizeof(buf), "%d-%02d-%02dT%02d:%02d:%02d", year(tm), month(tm),
           day(tm), hour(tm), minute(tm), second(tm));
  strcpy(dt, buf);

  return dt;
}

time_t NTPClient::s_getTime() { return NTP.getTime(); }

time_t NTPClient::getTime() {
  if (!WiFi.isConnected()) {
    DEBUGLOG("[NTP] Error. WiFi not connected.\n");
    return 0;
  }

  DEBUGLOG("[NTP] Requesting time from: %s.\n", sntp_getservername(0));
  uint32_t timestamp = sntp_get_current_timestamp();

  if (timestamp) {
    setSyncInterval(NTP.getPollingInterval()); // Regular polling interval
    timestamp += utc_offset;                   // Adjust time with UTC Offset

    last_poll = timestamp;

    if (0 == first_poll) {
      first_poll = timestamp;
      DEBUGLOG("[NTP] First synchronization: %s.\n",
               getTimeDate(getFirstSync()));
    }

    if (onNTPSyncEvent != NULL) {
      onNTPSyncEvent(NTP_EVENT_SYNCHRONIZED);
    }

    DEBUGLOG("[NTP] Time successfully synchronized to: %s.\n",
             getTimeDate(getLastSync()));
  } else {
    // Received no response from the NTP Server
    DEBUGLOG("[NTP] Error.\n");
    if (onNTPSyncEvent != NULL) {
      onNTPSyncEvent(NTP_EVENT_NO_RESPONSE);
    }

    setSyncInterval(NTP_SHORT_INTERVAL); // Get synchronized immediately
  }

  return timestamp;
}
