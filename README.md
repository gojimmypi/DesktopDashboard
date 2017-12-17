# DesktopDashboard

What's a Desktop Dashboard? Well, I work as a Senior Software Engineer. The management staff wanted a "better, easier" method for viewing key business metrics. Although I've created SQL SSAS OLAP cubes using XLS as a front end, many web-based Crystal Reports, and an entire internal web site... well, there's still a desire for an even simpler dashboard. This solution? No login. No computer. No fussing with a phone. Just something that sits on a desk and displays dashboard business metrics.

I was inspired by the weather station:

https://github.com/squix78/esp8266-weather-station

and decided to take the idea further than just weather. My first prototype was met with "wow, that's cool - but the display is so small". So I moved on to the ILI9341 display.

The plan is to add staff productivity, budget status, staff counts, even IT security metrics. All of this will be sent via user-specific JSON to the Desktop Dashboard device. Think of it as a desktop clock, with business-relevant news!

The one issue with the ESP8266 - is that with the low price tag, it has limited resources. It appears that even something as simple as jpg decompression is beyond the scope of possibilities. So I wrote an image conversion utility to take a variety  of image formats and convert them to the simplest BMP:

https://github.com/gojimmypi/imageConvert2BMP

This version incorprates my "Visitor WiFi" library, that programmatically presses the "I accept the terms and conditions" button on a redirect page, before actually granting access to the internet:

https://github.com/gojimmypi/VisitorWiFi-ESP8266

Included in that library are a variety of useful functions for getting HTML tag values, parsing headers, reading query strings, and more.

Note that if you download the entire project and compile this with Visual Micro add-in for Visual Studio, you'll likely need to edit the DesktopDashboard.vcxproj file, replacing all the instances of C:\Users\gojimmypi... with C:\Users\YourLoginName  (does anyone know of a more graceful way to share Visual Studio projects on github?). Hopefully your Arduino libs are also installed by default at a location like:

C:\Users\YourLoginName\AppData\Local\arduino15\packages\esp8266\
More coming soon...

