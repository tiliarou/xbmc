#pragma once
/*
 *      Copyright (C) 2005-2017 Team KODI
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "../AddonBase.h"
#include "Window.h"

namespace kodi
{
namespace gui
{

  //============================================================================
  ///
  /// \defgroup cpp_kodi_gui_CControlSlider Control Slider
  /// \ingroup cpp_kodi_gui
  /// @brief \cpp_class{ kodi::gui::CControlSlider }
  /// **Window control for moveable slider**
  ///
  /// The slider control is used for things where a sliding bar best  represents
  /// the operation at hand (such as a volume control or seek control).  You can
  /// choose the position, size, and look of the slider control.
  ///
  /// It has the header \ref ControlSlider.h "#include <kodi/gui/ControlSlider.h>"
  /// be included to enjoy it.
  ///
  /// Here you find the needed skin part for a \ref Slider_Control "slider control"
  ///
  /// @note The  call of the  control  is only  possible from  the corresponding
  /// window as its class and identification number is required.
  ///
  class CControlSlider
  {
  public:
    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief Construct a new control
    ///
    /// @param[in] window               related window control class
    /// @param[in] controlId            Used skin xml control id
    ///
    CControlSlider(CWindow* window, int controlId)
      : m_Window(window),
        m_ControlId(controlId)
    {
      m_ControlHandle = ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->window.GetControl_Slider(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_Window->m_WindowHandle, controlId);
      if (!m_ControlHandle)
        kodi::Log(ADDON_LOG_FATAL, "kodi::gui::CControlSlider can't create control class from Kodi !!!");
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief Destructor
    ///
    virtual ~CControlSlider() { }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief Set the control on window to visible
    ///
    /// @param[in] visible              If true visible, otherwise hidden
    ///
    void SetVisible(bool visible)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetVisible(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, visible);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief Set's the control's enabled/disabled state
    ///
    /// @param[in] enabled              If true enabled, otherwise disabled
    ///
    void SetEnabled(bool enabled)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetEnabled(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, enabled);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief To reset slider on defaults
    ///
    void Reset()
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.Reset(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief With GetDescription becomes a string value of position returned.
    ///
    /// @return                        Text string about current slider position
    ///
    /// The following are the text definition returned from this:
    /// | Value     | Without range selection | With range selection           |
    /// |:---------:|:------------------------|:-------------------------------|
    /// | float     | <c>%2.2f</c>            | <c>[%2.2f, %2.2f]</c>          |
    /// | integer   | <c>%i</c>               | <c>[%i, %i]</c>                |
    /// | percent   | <c>%i%%</c>             | <c>[%i%%, %i%%]</c>            |
    ///
    std::string GetDescription() const
    {
      std::string text;
      text.resize(1024);
      unsigned int size = (unsigned int)text.capacity();
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.GetDescription(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, text[0], size);
      text.resize(size);
      text.shrink_to_fit();
      return text;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief To set the the range as integer of slider, e.g. -10 is the slider
    /// start and e.g. +10 is the from here defined position where it  reach the
    /// end.
    ///
    /// Ad default is the range from 0 to 100.
    ///
    /// The integer interval is as default 1 and can be changed with
    /// @ref SetIntInterval.
    ///
    /// @param[in] start                Integer start value
    /// @param[in] end                  Integer end value
    ///
    /// @note Percent, floating point or  integer are alone possible.  Combining
    /// these different values can be not together and can, therefore,  only one
    /// each can be used.
    ///
    void SetIntRange(int start, int end)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetIntRange(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, start, end);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup CControlSlider
    /// @brief Set the slider position  with the given integer value.  The Range
    /// must be defined with a call from \ref SetIntRange before.
    ///
    /// @param[in] value                Position in range to set with integer
    ///
    /// @note Percent, floating point or integer  are alone possible.  Combining
    /// these different values can be not together and can, therefore,  only one
    /// each can be used.
    ///
    void SetIntValue(int value)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetIntValue(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, value);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief To get the current position as integer value.
    ///
    /// @return                         The position as integer
    ///
    /// @note Percent,  floating point or integer are alone possible.  Combining
    /// these  different  values  can be not together and can,  therefore,  only
    /// one each can be used.
    ///
    int GetIntValue() const
    {
      return ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.GetIntValue(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief To set the interval  steps of slider,  as default is it 1.  If it
    /// becomes  changed  with this function  will a step  of the  user with the
    /// value fixed here be executed.
    ///
    /// @param[in] interval             Intervall step to set.
    ///
    /// @note Percent,  floating point or integer are alone possible.  Combining
    /// these different values can be not together and can, therefore,  only one
    /// each can be used.
    ///
    void SetIntInterval(int interval)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetIntInterval(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, interval);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief Sets the percent of the slider.
    ///
    /// @param[in] percent              float - Percent value of slide
    ///
    /// @note Percent, floating point or  integer are alone possible.  Combining
    /// these different values can be not together and can, therefore,  only one
    /// each can be used.
    ///
    void SetPercentage(float percent)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetPercentage(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, percent);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief Returns a float of the percent of the slider.
    ///
    /// @return                         float - Percent of slider
    ///
    /// @note Percent, floating point or integer  are alone possible.  Combining
    /// these different values can be not together and can, therefore,  only one
    /// each can be used.
    ///
    float GetPercentage() const
    {
      return ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.GetPercentage(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief To set the the range as float of slider, e.g. -25.0 is the slider
    /// start and e.g. +25.0 is  the from here  defined position  where it reach
    /// the end.
    ///
    /// As default is the range 0.0 to 1.0.
    ///
    /// The float interval is as default 0.1 and can be changed with
    /// @ref SetFloatInterval.
    ///
    /// @param[in] start                Integer start value
    /// @param[in] end                  Integer end value
    ///
    /// @note Percent,  floating point or integer are alone possible.  Combining
    /// these  different  values  can be not together and can,  therefore,  only
    /// one each can be used.
    ///
    void SetFloatRange(float start, float end)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetFloatRange(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, start, end);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief Set the slider  position with the  given float value.  The  Range
    /// can be defined with a call from  \ref SetIntRange before,  as default it
    /// is 0.0 to 1.0.
    ///
    /// @param[in] value                Position in range to set with float
    ///
    /// @note Percent, floating  point or integer are alone possible.  Combining
    /// these different values can be not together and can, therefore,  only one
    /// each can be used.
    ///
    void SetFloatValue(float value)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetFloatValue(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, value);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief To get the current position as float value.
    ///
    /// @return                         The position as float
    ///
    float GetFloatValue() const
    {
      return ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.GetFloatValue(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CControlSlider
    /// @brief To set the interval  steps of slider, as default is it 0.1  If it
    /// becomes changed  with this  function  will a step  of the user  with the
    /// value fixed here be executed.
    ///
    /// @param[in] interval             Intervall step to set.
    ///
    /// @note Percent, floating point or integer  are alone possible.  Combining
    /// these different  values can be  not together  and can,  therefore,  only
    /// one each can be used.
    ///
    void SetFloatInterval(float interval)
    {
      ::kodi::addon::CAddonBase::m_interface->toKodi.kodi_gui->controlSlider.SetFloatInterval(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, m_ControlHandle, interval);
    }
    //--------------------------------------------------------------------------

  private:
    CWindow* m_Window;
    int m_ControlId;
    void* m_ControlHandle;
  };

} /* namespace gui */
} /* namespace kodi */
