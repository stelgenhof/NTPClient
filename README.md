## *** Archived ***
Now that version 2.4.0 (and above) of the [ESP8266 core for Arduino](https://github.com/esp8266/Arduino) library provides proper support for timezones and [DST](https://en.wikipedia.org/wiki/Daylight_saving_time), this library has served it's purpose and has been archived. 

This library becoming obsolete is a good thing: developers write their own libraries, API's, etc simply because features in existing projects are lacking or bugs are present. These efforts are an important part of [OSS](https://en.wikipedia.org/wiki/Open-source_software): helping each other to make software better.

If you're using version 2.4.0 (or above) of the ESP8266 core for Arduino library, and want to know how to implement SNTP functions in your project, checkout the [NTP-TZ-DST example](https://github.com/esp8266/Arduino/tree/master/libraries/esp8266/examples/NTP-TZ-DST).

Please fork it, use it as an archive, a reference or as an example to learn developing ESP8266/Arduino sketches/libraries :)


# NTPClient

A simple and small NTP Client library to have date and time on your ESP8266 device synchronized periodically, supporting timezones.

For Internet connected devices like the ESP8266, Arduino, etc. having a correct time and date for your project can be essential. For these devices plenty of good NTP libraries are available, however most of these contain too many features. In addition, supporting timezones (i.e. UTC offsets) are often implemented poorly.

This NTPClient library makes use of the internal Espressif SDK routines for the built in SNTP API, allowing this client to be small and simple. Unfortunately the timezone implementation of the SNTP API is limited in that only UTC offsets between -11 and 13 hour offsets can be used. Currently UTC offsets range from -12 to 14 hours including some timezones that are not a full hour (e.g. 05:30, 05:45). This NTPClient library overcomes that and fully supports all known timezones.

Using the NTPClient library is very straightforward and simple. Include this NTPClient library in your project and start the synchronization by calling the `NTP.init` function. You can use it without any parameters (defaults will be used) or adjust to your liking. It is recommended to use a NTP server closer to your location and the appropriate timezone (i.e. UTC offset). Please refer to the header file for the full list of UTC offsets.

By default a polling interval of 30 minutes is used for synchronizing with the NTP server. You may adjust this by using the `NTP.setPollingInterval(interval)` function, where `interval` is the desired interval in seconds.

To keep the NTPClient library small, no functions other than `NTP.getTimeDate(time_t _time)` are implemented that output time and date in a human readable format. The `NTP.getTimeDate(time_t _time)` will render the given time in [ISO8601](https://en.wikipedia.org/wiki/ISO_8601) format.

For more advanced use, a callback function may be used to handle some events emitted by the NTPClient library. The following events are currently implemented:

- NTP_EVENT_INIT: Issued upon start of the synchronization.
- NTP_EVENT_STOP: Issued when the synchronization is stopped.
- NTP_EVENT_NO_RESPONSE: Issued when no response is received from the NTP server.
- NTP_EVENT_SYNCHRONIZED: Issued when time has been successfully synchronized.

A full working example is included in the examples folder.

Making this library was heavily inspired by the wonderful [NTPClient Library](https://github.com/gmag11/NtpClient) of [Germán Martín](https://github.com/gmag11).

# Disclaimer

The [SNTP](https://tools.ietf.org/html/rfc4330) protocol is a simple implementation of the [NTP](https://tools.ietf.org/html/rfc5905) protocol, therefore disregarding time drift values and using simplified ways of system clock adjustment methods. Hence, complete accuracy and reliability can not be guaranteed however time synchronization in the range of 1 to 2 seconds is achievable.

This NTPClient library is designed for the ESP8266 device only. Other [MCU](https://en.wikipedia.org/wiki/Microcontroller) like Arduino, etc. are not supported.

[DST](https://en.wikipedia.org/wiki/Daylight_saving_time) is currently not supported. The use of DST varies by region or country and can also change over time.

# Requirements/Dependencies

You will need version 2.3.0 of the [ESP8266 core for Arduino](https://github.com/esp8266/Arduino) library. Any newer versions are known to work however with some issues.

This library makes use of the [Time](https://github.com/PaulStoffregen/Time.git) library. You need to add it to use this NTPClient.

# Contributing

Contributions are encouraged and welcome; I am always happy to get feedback or pull requests on Github :)

# License

The NTPClient library is open-sourced software licensed under the [MIT license](http://opensource.org/licenses/MIT). For the full copyright and license information, please see the [LICENSE](LICENSE) file that was distributed with this source code.
