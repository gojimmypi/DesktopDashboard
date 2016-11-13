# DesktopDashboard

Desktop Dashboard using ESP8266 and ILI9341 display.

What's a Desktop Dashboard? Well, I work as a Senior Software Engineer. The management staff wanted a "better, easier" method for viewing key business metrics. Although I've created SQL SSAS OLAP cubes using XLS as a front end, many web=based Crystal Reports, an entire internal web site... well, there's still a desire for an even simpler dashboard. This solution? No login. No computer. No fussing with a phone. 

I was inspired by the weather station:

https://github.com/squix78/esp8266-weather-station

and decided to take the idea further than just weather. My first prototype was met with "wow, that's cool - but the display is so small". So I moved on to the ILI9341 display.

The plan is to add staff productivity, budget status, staff counts, even IT security metrics. All of this will be sent via user-specific JSON to the Desktop Dashboard device. Think of it as a desktop clock, with business-relevant news!

The one issue with the ESP8266 - is that with the low price tag, it has limited resources. It appears that even something as simple as jpg decompression is beyond the scope of possibilities. So I wrote an image conversion utility to take a variety  of image formats and convert them to the simplest BMP:

https://github.com/gojimmypi/imageConvert2BMP

More coming soon...

