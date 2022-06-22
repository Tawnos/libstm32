#pragma once
#ifndef DCDARKNET_DISPLAY_DEVICE_H
#define DCDARKNET_DISPLAY_DEVICE_H

#include "common.h"
#include "art/images.h"
#include "framebuf.h"
#include "../error_type.h"
#include <fonts/fonts.h>
#include "../utility/bitarray.h"
#include <rgbcolor.h>
#include <assert.h>
#if !defined VIRTUAL_DEVICE
#include "../utility/pin.h"
#endif
#include <common.h>

namespace cmdc0de
{
   /*
    * @author cmdc0de
    * @date:  2/13/17
    *
    * Base class for display devices
    */
   class DisplayDevice
   {
   public:
      DisplayDevice(uint16_t w, uint16_t h, RotationType r, FrameBuf* fb)
         : Width(w), Height(h), RotationType(r), RefreshTopToBot(0), FB(fb)
      {}
      virtual ~DisplayDevice() = default;

   public:
      constexpr uint16_t getWidth() const { return Width; }
      constexpr uint16_t getHeight() const { return Height; }
      constexpr RotationType getRotation() const { return RotationType; }
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
      constexpr bool isTopToBotRefresh() { return RefreshTopToBot; }
      constexpr void setTopToBotRefresh(bool b) { RefreshTopToBot = b; }
      constexpr FrameBuf* getFrameBuffer() const { return FB; }

   public:
      // Input: x         horizontal position of the top left corner of the character, columns from the left edge
      //        y         vertical position of the top left corner of the character, rows from the top edge
      //        c         character to be printed
      //        textColor 16-bit color of the character
      //        bgColor   16-bit color of the background
      //        size      number of pixels per character pixepl (e.g. size==2 prints each pixel of font as 2x2 square)
      
      // Output: none
      virtual void update() = 0;
      virtual void reset() = 0;

   protected:
      virtual cmdc0de::ErrorType onInit() { return cmdc0de::ErrorType{}; };
      uint16_t Width = cmdc0de::DISPLAY_WIDTH;
      uint16_t Height = cmdc0de::DISPLAY_HEIGHT;
      RotationType RotationType : 3 = cmdc0de::START_ROT;
      uint32_t RefreshTopToBot : 1 = 0;
      FrameBuf* FB{ nullptr };
   };
}
#endif
