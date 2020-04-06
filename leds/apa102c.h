#ifndef _LIBSTM32_APA102C_H
#define _LIBSTM32_APA102C_H

namespace cmdc0de {

class RGB;
class SPIDevice;

/*
*	Brightness is a percentage
*/
class APA102c {
public:
	//0xE0 because high 3 bits are always on
	static const uint8_t BRIGHTNESS_START_BITS = 0xE0;
	static const uint8_t MAX_BRIGHTNESS			 = 31;
	static const char *LOG;
public:
	APA102c(const SPIDevice *spiI);
	void init(uint16_t nleds, RGB *ledBuf);
	void send();
private:
	const SPIDevice *SPIInterface;
	uint16_t BufferSize;
	char *LedBuffer1;
};

}

#endif
