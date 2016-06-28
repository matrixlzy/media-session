/* Simple Plugin API
 * Copyright (C) 2016 Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __SPA_DEFS_H__
#define __SPA_DEFS_H__

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif
#include <inttypes.h>
#include <stdlib.h>

typedef enum {
  SPA_RESULT_OK                        =  0,
  SPA_RESULT_ERROR                     = -1,
  SPA_RESULT_INACTIVE                  = -2,
  SPA_RESULT_NO_FORMAT                 = -3,
  SPA_RESULT_INVALID_COMMAND           = -4,
  SPA_RESULT_INVALID_PORT              = -5,
  SPA_RESULT_HAVE_ENOUGH_INPUT         = -6,
  SPA_RESULT_NEED_MORE_INPUT           = -7,
  SPA_RESULT_PORTS_CHANGED             = -9,
  SPA_RESULT_FORMAT_CHANGED            = -10,
  SPA_RESULT_PROPERTIES_CHANGED        = -11,
  SPA_RESULT_NOT_IMPLEMENTED           = -12,
  SPA_RESULT_INVALID_PROPERTY_INDEX    = -13,
  SPA_RESULT_PROPERTY_UNSET            = -14,
  SPA_RESULT_ENUM_END                  = -15,
  SPA_RESULT_WRONG_PROPERTY_TYPE       = -16,
  SPA_RESULT_WRONG_PROPERTY_SIZE       = -17,
  SPA_RESULT_INVALID_MEDIA_TYPE        = -18,
  SPA_RESULT_INVALID_FORMAT_PROPERTIES = -19,
  SPA_RESULT_FORMAT_INCOMPLETE         = -20,
  SPA_RESULT_INVALID_ARGUMENTS         = -21,
  SPA_RESULT_UNKNOWN_INTERFACE         = -22,
  SPA_RESULT_INVALID_DIRECTION         = -23,
  SPA_RESULT_TOO_MANY_PORTS            = -24,
  SPA_RESULT_INVALID_PROPERTY_ACCESS   = -25,
} SpaResult;

typedef enum {
  SPA_DIRECTION_INVALID         = 0,
  SPA_DIRECTION_INPUT,
  SPA_DIRECTION_OUTPUT
} SpaDirection;

typedef void (*SpaNotify) (void *data);

#define SPA_N_ELEMENTS(arr)  (sizeof (arr) / sizeof ((arr)[0]))

#ifdef __cplusplus
}  /* extern "C" */
#endif


#endif /* __SPA_DEFS_H__ */
