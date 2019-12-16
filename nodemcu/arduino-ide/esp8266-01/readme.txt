1. 刷新固件
ESP8266-01    usbTOttl
GND           GND
GPIO0         GND
RXD           TXD
VCC(3.3V)     VCC(3.3V)
CH_PD(EN)     VCC(3.3V)
TXD           RXD

2. 刷新固件后，断开 GPIO0 与 CH_PD(EN) 后，可以使用新固件
