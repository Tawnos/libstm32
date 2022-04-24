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

namespace cmdc0de
{
  class FrameBuf;

  enum class RotationType
  {
    PortraitTopLeft = 0,
    LandscapeTopLeft = 1
  };

  struct DCImage
  {
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
  class DisplayDevice
  {
  public:
    DisplayDevice(uint16_t w, uint16_t h, RotationType r)
      : Width(w), Height(h), RotationType(r), RefreshTopToBot(0)
    {
    }
    virtual ~DisplayDevice() = default;

  public:
    ErrorType init(FrameBuf* fb, const FontDef_t* font)
    {
      FB = fb;
      CurrentFont = font;
      return onInit();
    }
    constexpr uint16_t getWidth()
    {
      return Width;
    }
    constexpr uint16_t getHeight()
    {
      return Height;
    }
    constexpr RotationType getRotation()
    {
      return this->RotationType;
    }
    constexpr void setRotation(RotationType r, bool swapHeightWidth)
    {
      RotationType = r;
      if (swapHeightWidth)
      {
        uint16_t tmp = Height;
        Height = Width;
        Width = tmp;
      }
    }
    constexpr bool isTopToBotRefresh()
    {
      return RefreshTopToBot;
    }
    constexpr void setTopToBotRefresh(bool b)
    {
      RefreshTopToBot = b;
    }

    void setFont(const FontDef_t* font)
    {
      CurrentFont = font;
    }
    const FontDef_t* getFont()
    {
      return CurrentFont;
    }
    FrameBuf* getFrameBuffer() const
    {
      return FB;
    }

    void setTextColor(const RGBColor& t)
    {
      CurrentTextColor = t;
    }

    constexpr const RGBColor& getTextColor()
    {
      return CurrentTextColor;
    }

    void setBackgroundColor(const RGBColor& t)
    {
      CurrentBGColor = t;
    }
    constexpr const RGBColor& getBackgroundColor()
    {
      return CurrentBGColor;
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
    virtual uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor, const RGBColor& bgColor, uint8_t size, bool lineWrap, uint8_t charsToRender) = 0;
    virtual uint32_t drawStringOnLine(uint8_t line, const char* msg) = 0;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w) = 0;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) = 0;
    virtual void update() = 0;
    virtual void reset() = 0;

  protected:
    virtual ErrorType onInit() { return ErrorType{}; };
    uint16_t Width;
    uint16_t Height;
    RotationType RotationType : 3;
    uint32_t RefreshTopToBot : 1;

    RGBColor CurrentTextColor{ cmdc0de::RGBColor::WHITE };
    RGBColor CurrentBGColor{ cmdc0de::RGBColor::BLACK };
    const FontDef_t* CurrentFont{ nullptr };
    FrameBuf* FB{ nullptr };
  };
}
#endif
