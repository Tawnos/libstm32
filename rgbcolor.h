#pragma once
#ifndef _LIBSTM32_CMDC0DE_RGB_COLOR_H
#define _LIBSTM32_CMDC0DE_RGB_COLOR_H

#include <stdint.h>

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
    RGBColor(const RGBColor& r)
    {
       (*this) = r;
    }
    RGBColor(const uint16_t c)
    {
       (*this) = c;
    }
    constexpr uint8_t getR() const {
      return R;
    }
    constexpr uint8_t getG() const {
      return G;
    }
    constexpr uint8_t getB() const {
      return B;
    }
    RGBColor& operator=(const RGBColor& r) {
      R = r.getR();
      G = r.getG();
      B = r.getB();
      return *this;
    }

    RGBColor& operator=(const uint16_t c)
    {
       R = (c & 0b1111100000000000) >> 11 << 3;
       G = (c & 0b0000011111100000) >> 5 << 2;
       B = (c & 0b0000000000011111) >> 0 << 3 ;
       return *this;
    }
    bool operator==(const RGBColor& r) const;
    bool operator!=(const RGBColor& r) const;
  private:
    uint8_t R, G, B;
  };

#undef RGB
#pragma pack(push, 1)
  class RGB {
  public:
    static const RGB BLACK;
    static const RGB RED;
    static const RGB GREEN;
    static const RGB BLUE;
    static const RGB WHITE;
  public:
    RGB() {}
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
    uint8_t B{ 0 };
    uint8_t G{ 0 };
    uint8_t R{ 0 };
    uint8_t Brightness{ 100 };
  };
#pragma pack(pop)

}

#endif
