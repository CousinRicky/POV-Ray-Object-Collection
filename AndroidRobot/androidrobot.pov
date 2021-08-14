/* androidrobot.pov version 5.0A
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection demo
 *
 * Demo of Google's Android[TM] mascot.
 *
 * Android is a trademark of Google LLC.  The Android robot is reproduced or
 * modified from work created and shared by Google and used according to terms
 * described in the Creative Commons 3.0 Attribution License.
 * See https://developer.android.com/distribute/marketing-tools/ and
 * https://creativecommons.org/licenses/by/3.0/ for more information.
 *
 * See also https://commons.wikimedia.org/wiki/File:Android_Robot_POV-Ray.png
 *
 * Portions of the SDL source code are copyright (C) 2009 Karl Ostmo.  Some
 * rights reserved.  Portions of the SDL source code are copyright (C) 2011 -
 * 2021 Richard Callwood III.  Some rights reserved.
 * This file is licensed under the terms of the CC-LGPL
 * a.k.a. the GNU Lesser General Public License version 2.1.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  Please
 * visit https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html for
 * the text of the GNU Lesser General Public License version 2.1.
 *
 * Vers.  Date         Comments                                           Author
 * -----  ----         --------                                           ------
 * 1.0    2009-Oct-30  The original code is written as droid.pov.          K.O.
 * 2.0    2011-Jul-23  The demo environment is moved to this separate      R.C.
 *                     scene file.  A new ground plane is added, along
 *                     with a background and some global settings.
 * 2.1    2012-Jun-26  The #version is changed to 3.5.                     R.C.
 * 2.2    2014-Oct-05  The Google license statement is copied from the     R.C.
 *                     include file.
 * 5.0    2020-Sep-19  The #version is auto-detected within a range.       R.C.
 * 5.0A   2021-Aug-14  The license text is updated.                        R.C.
 */
#version max (3.5, min (3.8, version));

global_settings
{ assumed_gamma 1
  #if(1)
    radiosity
    { recursion_limit 1
      error_bound 0.1
      pretrace_end 0.01
      count 200
    }
  #end
}
#default { finish { ambient 0 } } //for pre-3.7 radiosity

light_source {
   <2.8, 2, -2.83333>, rgb 1
   parallel point_at 0
   #if(1)
     area_light x, y, 17, 17
     circular orient adaptive 1 jitter
   #end
}

camera {
   perspective
   location <2, 1.2, -0.3>
   sky <0, 1, 0>
   direction <0, 0, 1>
   right <1.33333, 0, 0>
   up <0, 1, 0>
   look_at <0, 0.5, 0>
}

background { rgb 1 }

#include "androidrobot.inc"

object
{ AndroidRobot (no)
  pigment { ANDROIDROBOT_C_COLOR }
}

plane
{ y, ANDROIDROBOT_V_BASE.y
  pigment { rgb 1 }
  finish { reflection { 0 0.15 fresnel } }
  interior { ior 1.5 }
}

// end of androidrobot.pov
