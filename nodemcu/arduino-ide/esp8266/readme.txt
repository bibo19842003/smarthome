一. 刷新固件
1.连线
ESP8266-01    usbTOttl
GND           GND
GPIO0         GND
RXD           TXD
VCC(3.3V)     VCC(3.3V)
CH_PD(EN)     VCC(3.3V)
TXD           RXD

2. 刷新固件后，断开 GPIO0 与 CH_PD(EN) 后，可以使用新固件

二. nodemcu 扩展板
有两种扩展板，CP2102 和 CH340, CH340的扩展板要宽一些

ESP8266 Motor shield 适用于 CP2102
NODEMCU ESP8266 V3 适用于 CH340


