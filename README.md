# Project for Jewish Clock

Based on the works of:

* zmanin library: https://github.com/yparitcher/libzmanim
(in the hopes modify it so it would be re-use it and not branch out of it )
* https://werner.rothschopf.net/ - the LCD library used for hebrew writing 

current BOM: (i'm not affiliated with any of those )
* Arduino NANO
* LCD with Hebrew Fonts: https://www.aliexpress.com/item/32860270883.html
* I2C adapter for LCD: https://www.aliexpress.com/item/32848549625.html
* Mini DS3231 with CR1225 battery : https://www.aliexpress.com/item/1005003708922776.html - 
  * note: that while there are other DS3231 modules like this one: https://www.aliexpress.com/item/4000004876793.html      
    with what LOOKS like CR2032 , it's infact for LIR2032 rechargable battery, which can be dangerous , head on here for more details: https://forum.arduino.cc/t/zs-042-ds3231-rtc-module/268862?fbclid=IwAR3IvX4NHmxLW5qne8AV6rtS4bM5e0jD46ApXR-NxlOgQx33-mPqDwqZDRE
* 3 push buttons
