#ifndef DCDARKNET_DISPLAY_DEVICE_H
#define DCDARKNET_DISPLAY_DEVICE_H

#include "../error_type.h"
#include "fonts.h"
#include "../utility/bitarray.h"
#include "../rgbcolor.h"
#include <assert.h>
#if !defined VIRTUAL_DEVICE
#include "../utility/pin.h"
#endif

namespace cmdc0de {

  ///enumeration of pixel formats for interface pixel command
  enum PIXEL_FORMAT {
    FORMAT_12_BIT = 0b011, FORMAT_16_BIT = 0b101, FORMAT_18_BIT = 0b110
  };

  struct DCImage {
    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    const char* pixel_data;
  };

  /*
   * @author cmdc0de
   * @date:  2/13/17
   *
   * Base class for display devices
   * 	not sure this is actually needed as you create an instance of the Display7735 class not this
   */
  class DisplayDevice {
  public:
    enum ROTATION {
      PORTAIT_TOP_LEFT = 0, LANDSCAPE_TOP_LEFT = 1
    };
  public:
    DisplayDevice(uint16_t w, uint16_t h, ROTATION r) :
      Width(w), Height(h), Rotation(r), RefreshTopToBot(0) {
    }
    virtual ~DisplayDevice() = default;
  public:
    constexpr uint16_t getWidth() {
      return Width;
    }
    constexpr uint16_t getHeight() {
      return Height;
    }
    constexpr ROTATION getRotation() {
      return Rotation == 0 ? PORTAIT_TOP_LEFT : LANDSCAPE_TOP_LEFT;
    }
    constexpr void setRotation(ROTATION r, bool swapHeightWidth) {
      Rotation = r;
      if (swapHeightWidth) {
        uint16_t tmp = Height;
        Height = Width;
        Width = tmp;
      }
    }
    constexpr bool isTopToBotRefresh() {
      return RefreshTopToBot;
    }
    constexpr void setTopToBotRefresh(bool b) {
      RefreshTopToBot = b;
    }

  public:
    virtual bool drawPixel(uint16_t x0, uint16_t y0, const RGBColor& color) = 0;
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) = 0;
    virtual void drawRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) = 0;
    virtual void fillScreen(const RGBColor& color) = 0;
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dcImage) = 0;
    virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt) = 0;
    virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor) = 0;
    virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor, const RGBColor& bgColor, uint8_t size, bool lineWrap) = 0;
    virtual uint32_t drawStringOnLine(uint8_t line, const char* msg) = 0;
    virtual const FontDef_t* getFont() = 0;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w) = 0;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, RGBColor color) = 0;
  protected:
  private:
    uint16_t Width;
    uint16_t Height;
    uint32_t Rotation : 3;
    uint32_t RefreshTopToBot : 1;
  };

  ///Used to convert from the RGB Color class to something the driver chip understands
  class PackedColor {
  public:
    PackedColor() : Color{ 0 }, SizeInBytes(2) {}
    constexpr uint8_t* getPackedColorData() { return &Color[0]; }
    constexpr uint8_t getSize() { return SizeInBytes; }
  public:
    static PackedColor create(uint8_t pixelFormat, const RGBColor& c) {
      PackedColor pc;
      switch (pixelFormat) {
      case FORMAT_12_BIT:
        pc.SizeInBytes = 2;
        break;
      case FORMAT_16_BIT: {
        uint16_t tmp;
        tmp = (c.getR() & 0b11111) << 11;
        tmp |= (c.getG() & 0b111111) << 5;
        tmp |= (c.getB() & 0b11111);
        pc.Color[0] = tmp >> 8;
        pc.Color[1] = tmp & 0xFF;
        pc.SizeInBytes = 2;
      }
                        break;
      case FORMAT_18_BIT:
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
  class FrameBuf {
  public:
    FrameBuf(DisplayDevice* d, uint16_t* SPIBuffer)
      : Display(d), SPIBuffer(SPIBuffer), PixelFormat(0), MemoryAccessControl(1) /*1 is not valid*/ {	}
    virtual ~FrameBuf() = default;
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h,
      const RGBColor& color) = 0;
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h,
      const RGBColor& color) = 0;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w,
      const RGBColor& color) = 0;
    virtual void swap() = 0;
    virtual bool drawPixel(uint16_t x0, uint16_t y0,
      const RGBColor& color) = 0;
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc) = 0;
  protected:
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    bool writeCmd(uint8_t c);
    bool writeNData(const uint8_t* data, int nbytes);
    bool write16Data(const uint16_t& data);
    bool writeN(char dc, const uint8_t* data, int nbytes);

    /////////////////////////////////////////
    // uint8_t to save space format is one of the value in the PIXEL_FORMAT ENUM
    constexpr void setPixelFormat(uint8_t pf) { PixelFormat = pf; }
    constexpr uint8_t getPixelFormat() const { return PixelFormat; }
    void setMemoryAccessControl();
    constexpr DisplayDevice* getDisplay() { return Display; }

  protected:
    uint16_t* SPIBuffer;
  private:
    DisplayDevice* Display;
    uint8_t PixelFormat;
    uint8_t MemoryAccessControl;

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
  class DrawBuffer2D16BitColor16BitPerPixel1Buffer : public FrameBuf {
  public:
    enum COLOR { // 2/2/2
      BLACK = 0,
      RED_MASK = 0x30,
      GREEN_MASK = 0xC,
      BLUE_MASK = 0x3,
      WHITE = 0xFFFF,
      BITS_PER_PIXEL = 5
    };
  public:
    DrawBuffer2D16BitColor16BitPerPixel1Buffer(uint8_t w, uint8_t h, uint16_t* spiBuffer, DisplayDevice* d) :
      FrameBuf(d, spiBuffer), Width(w), Height(h), BufferSize(w* h * sizeof(uint16_t)) {}
    virtual ~DrawBuffer2D16BitColor16BitPerPixel1Buffer() = default;

    virtual bool drawPixel(uint16_t x, uint16_t y, const RGBColor& color);
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h,
      const RGBColor& color);
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w,
      const RGBColor& color);
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h,
      const RGBColor& color);
    virtual void swap();
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc);
  protected:
    uint16_t calcLCDColor(const RGBColor& color);
  private:
    uint8_t Width;
    uint8_t Height;
    uint16_t BufferSize;
  };


  /*
   * @author cmdc0de
   * @date 6/2/17
   * Pass through frame buffer, basically no buffer every pixel goes
   * directly to SPI bus
   */
  class DrawBufferNoBuffer : public FrameBuf {
  public:
    DrawBufferNoBuffer(DisplayDevice* d, uint16_t* optimizedFillBuf, uint8_t rowsForDrawBuffer)
      : FrameBuf(d, optimizedFillBuf), RowsForDrawBuffer(rowsForDrawBuffer) { }
    virtual ~DrawBufferNoBuffer() = default;
    virtual bool drawPixel(uint16_t x0, uint16_t y0, const RGBColor& color);
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h,
      const RGBColor& color);
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w,
      const RGBColor& color);
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h,
      const RGBColor& color);
    virtual void swap();
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc);
  private:
    uint8_t RowsForDrawBuffer;
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
  class DrawBuffer2D16BitColor : public FrameBuf {
  public:
    enum COLOR { // 2/2/2
      BLACK = 0,
      RED_MASK = 0x30,
      GREEN_MASK = 0xC,
      BLUE_MASK = 0x3,
      WHITE = 0x3F,
      BITS_PER_PIXEL = 2
    };
  public:
    DrawBuffer2D16BitColor(
      uint8_t w,
      uint8_t h,
      uint8_t* backBuffer,
      uint16_t* spiBuffer,
      uint8_t rowsForDrawBuffer,
      uint8_t* drawBlocksBuffer,
      DisplayDevice* d)
      : FrameBuf(d, spiBuffer),
      Width(w),
      Height(h),
      BufferSize(w* h),
      BackBuffer(backBuffer, w* h, 6),
      RowsForDrawBuffer(rowsForDrawBuffer),
      DrawBlocksChanged(drawBlocksBuffer, h / rowsForDrawBuffer, 1) { }

    virtual ~DrawBuffer2D16BitColor() = default;
    virtual bool drawPixel(uint16_t x, uint16_t y, const RGBColor& color);
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h,
      const RGBColor& color);
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w,
      const RGBColor& color);
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h,
      const RGBColor& color);
    virtual void swap();
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc);
  protected:
    uint16_t calcLCDColor(uint8_t packedColor);
    uint8_t deresColor(const RGBColor& color);
  private:
    uint8_t Width;
    uint8_t Height;
    uint16_t BufferSize;
    BitArray BackBuffer;
    uint8_t RowsForDrawBuffer;
    BitArray DrawBlocksChanged;
  };
}
#endif
