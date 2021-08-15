/* spheresweep_infinity_pa.pov version 2.1A
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection demo
 *
 * A demonstration of the SphereSweep module's sphere_sweep wrapper, 2-D control
 * points, and PointArrays conversion.  The rendered image has artifacts in
 * POV-Ray 3.6.1, but not 3.7.
 *
 * Copyright (C) 2013 - 2021 Richard Callwood III.  Some rights reserved.
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
 * Vers  Date         Comments
 * ----  ----         --------
 * n/a   2013-Jun-11  Created as spheresweep_infinity.pov.
 * 1.0   2013-Jul-22  Uploaded.
 *       2015-May-10  The aspect ratio is changed to fit the v1.1 demo montage.
 * 1.1   2015-Aug-28  A second frame is added with a B-spline union.
 * 1.2   2015-Sep-11  No change.
 *       2019-Apr-28  Due to its new dependence on the PointArrays module, the
 *                    sphere_sweep wrapper demo is spun off to this file.
 *       2019-Apr-29  A demo of the PointArrays conversion is added.
 * 2.0   2020-Sep-09  Uploaded.
 * 2.1   2020-Sep-27  No change.
 * 2.1A  2021-Aug-15  The license text is updated.
 */
// +W420 +H180 +A +AM2 +R4
#version 3.61;

#include "pointarrays.inc"
#include "spheresweep.inc"

global_settings
{ assumed_gamma 1
  radiosity
  { count 50
    error_bound 0.5
    pretrace_end 0.01
    recursion_limit 2
  }
}
#default { finish { ambient 0 diffuse 0.75 } }

camera
{ location -25 * z
  right 7/3 * x
  angle 32
}

light_source
{ <-1, 2, -2> * 1000, rgb 1
  area_light 500 * x, 500 * y, 9, 9 adaptive 2
  circular orient jitter
}

background { rgb 0.15 }

plane
{ -z, -0.85
  pigment { rgb <0.2, 0.6, 1.0> }
}

//------------------------------------------------------------------------------

#declare Pts = array[11]
{ <4, 3>, <0, 0>, <-4, -3>,
  <-6, 0>, <-4, 3>, <0, 0>, <4, -3>, <6, 0>,
  <4, 3>, <0, 0>, <-4, -3>
}
#declare Radii = array[10]
{ 0.10, 0.10, 0.60, 0.20, 0.20, 0.70, 0.20, 0.20, 0.60, 0.10
}
object
{ SphereSweep_Native (SSWP_B_SPLINE, Pts, Radii, 0)
  pigment { red 0.125 }
  finish { specular 0.25 roughness 0.025 }
}

// Dummy declarations:
#declare PtsPA = 0;
#declare RadiusPA = 0;
#declare NewPts = 0;
#declare NewRadii = 0;

SSwp_Convert_to_PA (Pts, Radii, PtsPA, RadiusPA)
object
{ PA_Sphere_Sweep (PA_B, PtsPA, RadiusPA, transform { scale 0.5 })
  pigment { rgb <0.41, 0.11, 0.036> }
  finish { specular 0.25 roughness 0.025 }
  scale 1.4
  translate -1.5 * z
}

SSwp_Convert_from_PA (PtsPA, RadiusPA, NewPts, NewRadii)
object
{ SphereSweep_Native (SSWP_B_SPLINE, NewPts, NewRadii, 0)
  pigment { rgb <0.78, 0.48, 0.20> }
  finish { specular 0.25 roughness 0.025 }
  scale 0.5 translate -3 * z
}
// end of spheresweep_infinity_pa.pov
