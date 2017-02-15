/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "system.h"
#include "SystemdUtils.h"
#include "utils/log.h"

#ifdef HAS_DBUS

// systemd DBus interface specification:
// https://www.freedesktop.org/wiki/Software/systemd/dbus

#define SYSTEMD_DEST  "org.freedesktop.systemd1"
#define SYSTEMD_PATH  "/org/freedesktop/systemd1"
#define SYSTEMD_IFACE "org.freedesktop.systemd1.Manager"

bool CSystemdUtils::EnableUnit(const char **units, unsigned int length)
{
  CDBusMessage message(SYSTEMD_DEST, SYSTEMD_PATH, SYSTEMD_IFACE, "EnableUnitFiles");

  // It takes a list of unit files to enable (either just file names or full absolute paths)
  message.AppendArgument(units, length);

  // The first controls whether the unit shall be enabled for runtime only (true, /run), or persistently (false, /etc)
  message.AppendArgument(false);

  // The second one controls whether symlinks pointing to other units shall be replaced if necessary
  message.AppendArgument(false);
  return message.SendSystem() != NULL;
}

bool CSystemdUtils::DisableUnit(const char **units, unsigned int length)
{
  CDBusMessage message(SYSTEMD_DEST, SYSTEMD_PATH, SYSTEMD_IFACE, "DisableUnitFiles");

  // It takes a list of unit files to enable (either just file names or full absolute paths)
  message.AppendArgument(units, length);

  // The first controls whether the unit shall be enabled for runtime only (true, /run), or persistently (false, /etc)
  message.AppendArgument(false);

  return message.SendSystem() != NULL;
}

bool CSystemdUtils::StartUnit(const char *unit)
{
  CDBusMessage message(SYSTEMD_DEST, SYSTEMD_PATH, SYSTEMD_IFACE, "StartUnit");

  // Takes the unit to activate, plus a mode string
  message.AppendArgument(unit);
  message.AppendArgument("replace");

  return message.SendSystem() != NULL;
}

bool CSystemdUtils::StopUnit(const char *unit)
{
  CDBusMessage message(SYSTEMD_DEST, SYSTEMD_PATH, SYSTEMD_IFACE, "StopUnit");

  // Takes the unit to activate, plus a mode string
  message.AppendArgument(unit);
  message.AppendArgument("replace");

  return message.SendSystem() != NULL;
}

// timedated DBus interface specification:
// https://www.freedesktop.org/wiki/Software/systemd/timedated

#define TIMEDATED_DEST  "org.freedesktop.timedate1"
#define TIMEDATED_PATH  "/org/freedesktop/timedate1"
#define TIMEDATED_IFACE "org.freedesktop.timedate1"

bool CSystemdUtils::SetTimezone(const char *timezone)
{
  CDBusMessage message(TIMEDATED_DEST, TIMEDATED_PATH, TIMEDATED_IFACE, "SetTimezone");

  // Pass a value like "Europe/Berlin" to set the timezone
  message.AppendArgument(timezone);

  // The user_interaction boolean parameters can be used to control whether PolicyKit should interactively ask the user for authentication credentials if it needs to.
  message.AppendArgument(false);

  return message.SendSystem() != NULL;
}

#endif
