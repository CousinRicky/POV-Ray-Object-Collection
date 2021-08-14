/* androidrobot_posed.pov version 5.0A
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection demo
 *
 * Demo of Google's Android[TM] mascot, showing how it can be posed.
 *
 * Android is a trademark of Google LLC.  The Android robot is reproduced or
 * modified from work created and shared by Google and used according to terms
 * described in the Creative Commons 3.0 Attribution License.
 * See https://developer.android.com/distribute/marketing-tools/ and
 * https://creativecommons.org/licenses/by/3.0/ for more information.
 *
 * Copyright (C) 2011 - 2021 Richard Callwood III.  Some rights reserved.
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
 * 2.0    2011-Jul-23  Created.                                            R.C.
 * 2.1    2012-Jun-26  The #version is changed to 3.5.                     R.C.
 * 2.2    2014-Oct-05  The robot is more charismatic.                      R.C.
 *                     Radiosity is added.
 *                     The metallic finish is tweaked.
 *                     The Google license statement is copied from the
 *                     include file.
 * 3.0    2015-Apr-25  Lenses are placed in the eyes.                      R.C.
 *                     Some kick is added to the robot's pose.
 * 4.0    2018-Sep-23  The environmental lighting is overhauled.           R.C.
 *                     The metal finish is tweaked.
 *                     The code is reorganized.
 * 5.0    2020-Sep-19  The shadows are sharpened a bit.                    R.C.
 *                     The #version is auto-detected within a range, and   R.C.
 *                     blur samples is defaulted accordingly
 * 5.0A   2021-Aug-14  The license text is updated.                        R.C.
 */
/* 3.5, 3.6 // +W280 +H280
 * 3.7, 3.8 // +W280 +H280 +A
 */
#version max (3.5, min (3.8, version));

#include "consts.inc" // for glass property and blur samples

#ifndef (BLUR_SAMPLES)
  #if (version < 3.7)
    #declare BLUR_SAMPLES = 100;
  #else
    #declare BLUR_SAMPLES = Hex_Blur3; // POV 3.7 can use smaller value with +A
  #end
#end
#ifndef (Rad) #declare Rad = yes; #end // Use radiosity?

global_settings
{ assumed_gamma 1
 // With the eye lenses introduced in the 3.0 version of this demo, the trace
 // maxes out at 160+, but there's no visual improvement at that level.
  max_trace_level 10
  #if (Rad)
    radiosity
    { count 100
      error_bound 0.5
      pretrace_end 0.01
      recursion_limit 2
    }
  #end
  photons { spacing 0.002 autostop 0 }
}

#include "transforms.inc"
#include "androidrobot.inc"

#declare V_AIM = <0, 0.85, 0>;
#declare V_CAM = <1.7, 0.75, -2.1>;
#declare HVIEW = 2.5;
#declare Field = HVIEW / vlength (V_AIM - V_CAM);
camera
{ location V_CAM
  look_at V_AIM
  up Field * y
  right Field * x
  #if (BLUR_SAMPLES)
    aperture 0.02
    focal_point V_AIM
    blur_samples BLUR_SAMPLES
  #end
}

//================================= LIGHTING ===================================

#if (Rad) // Turn off ambient for POV-Ray < 3.7.
  #declare C_AMBIENCE = rgb 0;
  #declare C_GROUND_AMBIENCE = rgb 0;
#else // Simulate radiosity.
  #declare C_AMBIENCE = rgb <0.24, 0.28, 0.36>;
  #declare C_GROUND_AMBIENCE = rgb <0.07, 0.12, 0.25>;
#end
#declare DIFFUSE = 0.8;
#default { finish { ambient C_AMBIENCE diffuse DIFFUSE } }

#declare V_SUN = vrotate (-z, <45, 25, 0>);
light_source
{ V_SUN * 1000, rgb <1.81, 1.73, 1.67>
  parallel point_at 0
  #if(1)
    area_light x * 100, y * 100, 17, 17
    circular orient adaptive 1 jitter
  #end
}

//=============================== ENVIRONMENT ==================================

#declare C_HALO = rgb <0.51, 0.81, 1.68>;
sky_sphere
{ pigment
  { function { pow (1 - y, 3) }
    color_map { [0 rgb <0.05, 0.10, 0.25>] [1 rgb <0.4, 0.6, 0.8>] }
  }
  pigment
  { function { select (z, pow (max (1 - sqrt (x*x + y*y) * 1.3, 0), 3), 0) }
    color_map { [0 C_HALO transmit 1] [1 C_HALO] }
    Reorient_Trans(-z, V_SUN)
  }
}

plane
{ y, 0
  pigment { checker rgb 0.125 rgb 0.625 }
  finish { ambient C_GROUND_AMBIENCE }
}

//================================ THE ROBOT ===================================

#declare UserDefinedEye = sphere
{ 0, AndroidRobot_Eye_radius()
  scale <0.5, 1, 1>
  pigment { rgbf 1 }
  finish
  { reflection { 0 1 fresnel } conserve_energy
    specular 125 roughness 0.001
  }
  interior { ior Crown_Glass_Ior }
 // While the stored photons aren't visible in this scene,
 // enabling photons adds to the contrast in the eyes.
  photons { target collect off reflection on refraction on }
  translate AndroidRobot_Eye_v() - <0.05, 0, 0>
}

#declare MyHeadRotation = transform { rotate <0, -25, 0> }

union
{ object
  { AndroidRobot_Posed
    ( no,
      MyHeadRotation,
      transform { rotate <-135, 30, 0> },
      transform { rotate <135, -30, 0> },
      transform {},
      transform { translate -0.1 * y rotate <0, 0, 60> }
    )
    pigment { ANDROIDROBOT_C_COLOR }
    finish
    { reflection { 0.9 metallic }
      diffuse DIFFUSE * 0.05
      ambient C_AMBIENCE * 0.05
      specular 1125 metallic
      roughness 0.0001
    }
  }
  union // facial features
  { object { UserDefinedEye }
    object { UserDefinedEye scale <1, 1, -1> }
    // transform { MyHeadRotation } // Wrong!
    AndroidRobot_Head_x (MyHeadRotation) // Correct
  }
  rotate 90 * y
  translate -ANDROIDROBOT_V_BASE
}

// end of androidrobot_posed.pov
