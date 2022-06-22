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

    operator uint16_t() { return (uint16_t)R << 11 | (G & 0b111111) << 5 | (B & 0b11111); }
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
    RGB(uint8_t r, uint8_t g, uint8_t b) : B(b), G(g), R(r) {}
    RGB(const RGB& r) : B(r.B), G(r.G), R(r.R) {}
    RGB& operator=(const RGB& r) { B = r.B; G = r.G; R = r.R;  return (*this); }
  private:
    uint8_t B{ 0 };
    uint8_t G{ 0 };
    uint8_t R{ 0 };
  };
#pragma pack(pop)

}

#endif
