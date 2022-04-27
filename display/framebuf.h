#pragma once

#include <rgbcolor.h>
#include <cassert>
#include <stdint.h>

namespace cmdc0de
{
  class DisplayDevice;
  /// enumeration of pixel formats for interface pixel command
  enum class PixelFormat
  {
    TwelveBit = 0b011,
    SixteenBit = 0b101,
    EighteenBit = 0b110
  };

  /// Used to convert from the RGB Color class to something the driver chip understands
  class PackedColor
  {
  public:
    PackedColor() : Color{ 0 }, SizeInBytes(2) {}
    constexpr uint8_t* getPackedColorData() { return &Color[0]; }
    constexpr uint8_t getSize() { return SizeInBytes; }

  public:
    static PackedColor create(PixelFormat pixelFormat, const RGBColor& c)
    {
      PackedColor pc;
      switch (pixelFormat)
      {
      case PixelFormat::TwelveBit:
        pc.SizeInBytes = 2;
        break;
      case PixelFormat::SixteenBit:
      {
        uint16_t tmp;
        tmp = (c.getR() & 0b11111) << 11;
        tmp |= (c.getG() & 0b111111) << 5;
        tmp |= (c.getB() & 0b11111);
        pc.Color[0] = tmp >> 8;
        pc.Color[1] = tmp & 0xFF;
        pc.SizeInBytes = 2;
      }
      break;
      case PixelFormat::EighteenBit:
        pc.Color[0] = c.getR() << 2;
        pc.Color[1] = c.getG() << 2;
        pc.Color[2] = c.getB() << 2;
        pc.SizeInBytes = 3;
        break;
      default:
        assert(false);
      }
      return pc;
    }

  private:
    uint8_t Color[3];
    uint8_t SizeInBytes;
  };

  /*
   * Allows us to have different frame buffer types to make optimal use of memory
   * Current implemented (see below)
   * pass though
   * 2d buffer
   * 3d buffer
   */
  class FrameBuf
  {
  public:
    virtual ~FrameBuf() = default;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) = 0;
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc) = 0;
    virtual bool drawPixel(uint16_t x0, uint16_t y0, const RGBColor& color) = 0;
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) = 0;
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) = 0;
    virtual void swap() = 0;

  public:
    void drawRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color);

  protected:
    FrameBuf(DisplayDevice* d, uint16_t* SPIBuffer, uint16_t bufferSize, uint8_t w, uint8_t h, PixelFormat pf)
      : Display(d),
      SPIBuffer(SPIBuffer),
      BufferSize(bufferSize),
      PixelFormat(pf),
      MemoryAccessControl(1), /*1 is not valid*/
      Width(w),
      Height(h) {}

    uint16_t* SPIBuffer{ nullptr };
    uint16_t BufferSize{ 0 };
    DisplayDevice* Display{ nullptr };
    const PixelFormat PixelFormat{ PixelFormat::TwelveBit };
    uint8_t MemoryAccessControl{ 0 };
    uint8_t Width{ cmdc0de::DISPLAY_WIDTH };
    uint8_t Height{ cmdc0de::DISPLAY_HEIGHT };

    friend class DisplayDevice;
  };

  /*
   * @author cmdc0de
   * @date 6/2/17
   *
   * 2 byte per pixel for backbuffer
   * 5 bits Red, 6 Bits Green, 5 bits blue -
   * for a 128x160 lcd you'll need 40,960 bytes for the buffer
   */
  class DrawBuffer2D16BitColor16BitPerPixel1Buffer : public FrameBuf
  {
  public:
    enum COLOR
    { // 2/2/2
      BLACK = 0,
      RED_MASK = 0x30,
      GREEN_MASK = 0xC,
      BLUE_MASK = 0x3,
      WHITE = 0xFFFF,
      BITS_PER_PIXEL = 5
    };

  public:
    DrawBuffer2D16BitColor16BitPerPixel1Buffer(DisplayDevice* d, uint16_t* spiBuffer)
      : FrameBuf(d, spiBuffer, cmdc0de::DISPLAY_WIDTH*cmdc0de::DISPLAY_HEIGHT, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT, PixelFormat::SixteenBit) {}
    virtual ~DrawBuffer2D16BitColor16BitPerPixel1Buffer() = default;

    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc) override;
    virtual bool drawPixel(uint16_t x, uint16_t y, const RGBColor& color) override;
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) override;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) override;
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) override;
    virtual void swap();

  protected:
    uint16_t calcLCDColor(const RGBColor& color);
  };
}
