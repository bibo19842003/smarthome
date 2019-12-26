一、BOM
nodemcu(CH340)及针角扩展板 × 1套
nodemcu(cp2102)及电机扩展板 × 1套
舵机S90 × 1
超声波SR04 × 1
红外线4路避障 × 1套
18650 × 4
电池盒 18650 两节串联 × 2
5v输出电源 × 1
两轮地盘 × 1套

二、接线：

nodemcu(cp2102)扩展板：
A+：电机1正极
A-：电机1负极
B+：电机2正极
B-：电机2负极
VM：电池盒正极
GND：电池盒负极
Vin：nodemcu(CH340)扩展板 VUSB
G：nodemcu(CH340)扩展板 GND


nodemcu(CH340)扩展板：
VUSB：nodemcu(cp2102)扩展板 Vin
GND：nodemcu(cp2102)扩展板 G
VUSB：SR04 Vcc
GND：SR04 Gnd
D1：SR04 Trig
D2：SR04 Echo
3V：红外线4路避障 vcc
GND：红外线4路避障 gnd
D5：红外线4路避障 out1
D6：红外线4路避障 out2
D7：红外线4路避障 out3
D8：红外线4路避障 out4

