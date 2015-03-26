// Squirrel Shell
// Copyright (c) 2006-2012, Constantin Makshin
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// Update these when releasing a new version of Squirrel Shell
#define SHELL_VERSION_MAJOR 1
#define SHELL_VERSION_MINOR 2
#define SHELL_VERSION_PATCH 7

// Update copyright years and/or authors when necessary
#define SHELL_VERSION_COPYRIGHT "Copyright (c) 2006-2012, Constantin Makshin"

// Don't touch the rest of this file :)

#include "platform.h"

#define SHELL_VERSION_TOSTR2(major, minor, patch) #major "." #minor "." #patch
#define SHELL_VERSION_TOSTR(major, minor, patch)  SHELL_VERSION_TOSTR2(major, minor, patch)

#define SHELL_VERSION_STR						  SHELL_VERSION_TOSTR(SHELL_VERSION_MAJOR, SHELL_VERSION_MINOR, SHELL_VERSION_PATCH)
