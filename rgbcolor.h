#ifndef _LIBSTM32_CMDC0DE_RGB_COLOR_H
#define _LIBSTM32_CMDC0DE_RGB_COLOR_H

namespace cmdc0de {

/*
 * @Author cmdc0de
 * @date:  2/13/17
 *
 * convenient RGB color class
 */
class RGBColor {
public:
	static const RGBColor BLACK;
	static const RGBColor RED;
	static const RGBColor GREEN;
	static const RGBColor BLUE;
	static const RGBColor WHITE;
public:
	RGBColor(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {
	}
	RGBColor(const RGBColor &r) {
		(*this) = r;
	}
	uint16_t getR() const {
		return R;
	}
	uint16_t getG() const {
		return G;
	}
	uint16_t getB() const {
		return B;
	}
	RGBColor &operator=(const RGBColor &r) {
		R = r.getR();
		G = r.getG();
		B = r.getB();
		return *this;
	}
	bool operator==(const RGBColor& r) const {
		return (r.R == R && r.G == G && r.B == B);
	}

	bool operator!=(const RGBColor& r) const {
		return !((*this) == r);
	}
private:
	uint8_t R, G, B;
};

#undef RGB
#pragma pack(push, 1)
class RGB {
public:
	static const RGB WHITE;
	static const RGB BLUE;
	static const RGB GREEN;
	static const RGB RED;
public:
	RGB() : B(0), G(0), R(0), Brightness(100) {}
	RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) : B(b), G(g), R(r), Brightness(brightness) {}
	RGB(const RGB& r) : B(r.B), G(r.G), R(r.R), Brightness(r.Brightness) {}
	RGB& operator=(const RGB& r) { B = r.B; G = r.G; R = r.R; Brightness = r.Brightness; return (*this); }
	uint8_t getBlue() { return B; }
	uint8_t getRed() { return R; }
	uint8_t getGreen() { return G; }
	uint8_t getBrightness() { return Brightness; }
	void setBlue(uint8_t v) { B = v; }
	void setRed(uint8_t v) { R = v; }
	void setGreen(uint8_t v) { G = v; }
	void setBrightness(uint8_t v) { Brightness = ((v > 100) ? 100 : v); }
private:
	uint8_t B;
	uint8_t G;
	uint8_t R;
	uint8_t Brightness;
}; 
#pragma pack(pop)

}

#endif
