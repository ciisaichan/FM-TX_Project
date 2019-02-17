#include <EEPROM.h>
#include <FMTX.h>
union data
{
	float a;
	byte b[4];
};
data rom_freq;
float fm_freq = 0;
void setup(void)
{

	Serial.begin(9600);
	Serial.print("FM-TX Demo (Edit by MaxChen_CX)\r\n");
	for (int i = 0; i < 4; i++) {
		rom_freq.b[i] = EEPROM.read(i);
	}
	if (rom_freq.a == 0.00) {
		fm_freq = 105;  //如果未设置过FM频率，就自动设置为105 Mhz
	}
	else {
		fm_freq = rom_freq.a;
	}
	//Serial.println(rom_freq.a);
	fmtx_init(fm_freq, CHINA);
	Serial.print("Channel:");
	Serial.print(fm_freq, 1);
	Serial.println("MHz");
}

void loop(void)
{
	/*
	更改FM频率请从串口监视器发送信息，频率数据以'&'开头，后面接上4位数字（例子：&8000）
	数据前3位为小数点前，最后一位为小数点后
	例如&1050 设置的FM频率为 105.0Mhz
	可设置的FM频率范围为0700~1080（70~108Mhz）
	*/
	if (Serial.available()) {
		switch (Serial.read()) {
		case '&':
			u8 i, buf[4];
			float ch;
			i = 0;
			delay(30);
			while (Serial.available() && i<4) {
				buf[i] = Serial.read();
				if (buf[i] <= '9' && buf[i] >= '0') {
					i++;
				}
				else {
					i = 0;
					break;
				}
			}
			if (i == 4) {
				ch = (buf[0] - '0') * 100 + (buf[1] - '0') * 10 + (buf[2] - '0') * 1 + 0.1*(buf[3] - '0');
				if (ch >= 70 && ch <= 108) {
					Serial.print("New Channel:");
					Serial.print(ch, 1);
					Serial.println("MHz");
					fmtx_set_freq(ch);

					rom_freq.a = ch;
					for (int i = 0; i < 4; i++) {
						EEPROM.write(i, rom_freq.b[i]);
					}
					Serial.println("Saved");
				}
				else {
					Serial.println("ERROR:Channel must be range from 70Mhz to 108Mhz.");
				}
			}
			else {
				Serial.println("ERROR:Input Format Error.");
			}

			while (Serial.available()) {
				Serial.read();
			}
			break;
		}
	}
}