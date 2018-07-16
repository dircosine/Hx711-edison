#include <Arduino.h>
#include <HX711.h>

#if ARDUINO_VERSION <= 106
    void yield(void) {};
#endif

HX711::HX711(byte dout, byte pd_sck, byte gain) {
	//begin(dout, pd_sck, gain);
}

HX711::HX711() {
}

HX711::~HX711() {
}

void HX711::begin(byte dout, byte pd_sck, byte gain) {
	PD_SCK = pd_sck;
	DOUT = dout;

	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);

	set_gain(gain);
}

bool HX711::is_ready() {
	return digitalRead(DOUT) == LOW;
}

void HX711::set_gain(byte gain) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 24;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 26;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 25;
			break;
	}

	digitalWrite(PD_SCK, LOW);
	read();
}

long HX711::read() {
	unsigned long Count = 0;

	while (digitalRead(DOUT));

	for (int i = 0; i<GAIN; i++)
	{
		digitalWrite(PD_SCK, HIGH);
		Count = Count << 1;
		digitalWrite(PD_SCK, LOW);
		if (digitalRead(DOUT))
		{
			Count++;
		}
	}

	digitalWrite(PD_SCK, HIGH);
	Count = Count ^ 0x800000;
	digitalWrite(PD_SCK, LOW);

	return static_cast<long>(Count);
}

long HX711::read_average(byte times) {
	long sum = 0;
	for (byte i = 0; i < times; i++) {
		sum += read();
		yield();
	}
	return sum / times;
}

double HX711::get_value(byte times) {
	return read_average(times) - OFFSET;
}

float HX711::get_units(byte times) {
	return get_value(times) / SCALE;
}

void HX711::tare(byte times) {
	double sum = read_average(times);
	set_offset(sum);
}

void HX711::set_scale(float scale) {
	SCALE = scale;
}

float HX711::get_scale() {
	return SCALE;
}

void HX711::set_offset(long offset) {
	OFFSET = offset;
}

long HX711::get_offset() {
	return OFFSET;
}

void HX711::power_down() {
	digitalWrite(PD_SCK, LOW);
	digitalWrite(PD_SCK, HIGH);
}

void HX711::power_up() {
	digitalWrite(PD_SCK, LOW);
}
