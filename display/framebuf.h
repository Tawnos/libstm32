#ifndef DCDARKNET_FRAMEBUF_H
#define DCDARKNET_FRAMEBUF_H

#include "display_device.h"

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
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h,
      const RGBColor& color) = 0;
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h,
      const RGBColor& color) = 0;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w,
      const RGBColor& color) = 0;
    virtual bool drawPixel(uint16_t x0, uint16_t y0,
      const RGBColor& color) = 0;
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc) = 0;
    virtual void swap() = 0;

  protected:
    FrameBuf(DisplayDevice* d, uint16_t* SPIBuffer, uint16_t bufferSize, PixelFormat pf)
      : Display(d),
      SPIBuffer(SPIBuffer),
      BufferSize(bufferSize),
      PixelFormat(pf),
      MemoryAccessControl(1) /*1 is not valid*/ {}
    uint16_t* SPIBuffer{ nullptr };
    uint16_t BufferSize{ 0 };
    DisplayDevice* Display{ nullptr };
    const PixelFormat PixelFormat{ PixelFormat::TwelveBit };
    uint8_t MemoryAccessControl{ 0 };

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
      : FrameBuf(d, spiBuffer, d->getWidth()* d->getHeight(), PixelFormat::SixteenBit) {}
    virtual ~DrawBuffer2D16BitColor16BitPerPixel1Buffer() = default;

    virtual bool drawPixel(uint16_t x, uint16_t y, const RGBColor& color) override;
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) override;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) override;
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) override;
    virtual void swap();
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc);

  protected:
    uint16_t calcLCDColor(const RGBColor& color);
  };

  /*
   * @author cmdc0de
   * @date 6/2/17
   *
   * 1 byte per pixel for backbuffer
   * 3 bits Red, 2 Bits Green, 3 bits blue - color passed in is deresed from 565 to 323
   * On each pixel write we track if we changed any pixel in rowsForDrawBuffer,
   * if not on swap we skip those rows, otherwise write 2 bytes (565) to drawBuffer
   * for the width of the screen for 'rowsForDrawBuffer' rows. flush to SPI bus then repeat until
   * entire screen is rendered
   */
  class DrawBuffer2D16BitColor : public FrameBuf
  {
  public:
    enum COLOR
    { // 2/2/2
      BLACK = 0,
      RED_MASK = 0x30,
      GREEN_MASK = 0xC,
      BLUE_MASK = 0x3,
      WHITE = 0x3F,
      BITS_PER_PIXEL = 2
    };

  public:
    DrawBuffer2D16BitColor(
      DisplayDevice* d,
      uint8_t* backBuffer,
      uint16_t* spiBuffer,
      uint8_t rowsForDrawBuffer,
      uint8_t* drawBlocksBuffer
    ) : FrameBuf(d, spiBuffer, d->getWidth()* d->getHeight(), PixelFormat::SixteenBit),
      BackBuffer{ backBuffer, d->getWidth() * d->getHeight(), 6 },
      RowsForDrawBuffer(rowsForDrawBuffer),
      DrawBlocksChanged{ drawBlocksBuffer, d->getHeight() / rowsForDrawBuffer, 1 } {}

    virtual ~DrawBuffer2D16BitColor() = default;
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc) override;
    virtual bool drawPixel(uint16_t x, uint16_t y, const RGBColor& color) override;
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) override;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) override;
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) override;
    virtual void swap() override;

  protected:
    uint16_t calcLCDColor(uint8_t packedColor);
    uint8_t deresColor(const RGBColor& color);

  private:
    BitArray BackBuffer{ nullptr,0,0 };
    uint8_t RowsForDrawBuffer = 0;
    BitArray DrawBlocksChanged{ nullptr,0,0 };
  };
}
#endif
