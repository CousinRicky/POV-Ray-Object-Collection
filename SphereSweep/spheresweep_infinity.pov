/* spheresweep_infinity.pov version 2.1A
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection demo
 *
 * A demonstration of the SphereSweep module's 2-D control points and B-spline.
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
 * n/a   2013-Jun-11  Created.
 * 1.0   2013-Jul-22  Uploaded.
 *       2015-May-10  The aspect ratio is changed to fit the v1.1 demo montage.
 * 1.1   2015-Aug-28  A second frame is added with a B-spline union.
 * 1.2   2015-Sep-11  No change.
 *       2019-Apr-28  Due to its new dependence on the PointArrays module, the
 *                    sphere_sweep wrapper demo is spun off to a separate scene
 *                    file, spheresweep_infinity_pa.pov.
 *       2019-Apr-28  Colors and lighting contrast are adjusted a bit.
 * 2.0   2020-Sep-09  Uploaded.
 * 2.1   2020-Sep-27  No change.
 * 2.1A  2021-Aug-15  The license text is updated.
 */
// +W420 +H180 +A +AM2 +R4
#version 3.5;

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
  area_light 500 * x, 500 * y, 9, 9 adaptive 1
  circular orient jitter
}

background { rgb 0.15 }

plane
{ -z, -0.85
  pigment { rgb 0.6 }
}

//------------------------------------------------------------------------------

#declare Pts = array[11]
{ <4, 3>, <0, 0>, <-4, -3>,
  <-6, 0>, <-4, 3>, <0, 0>, <4, -3>, <6, 0>,
  <4, 3>, <0, 0>, <-4, -3>
}
#declare Radii = array[10]
{ 0.12, 0.12, 0.72, 0.24, 0.24, 0.84, 0.24, 0.24, 0.72, 0.12
}

object
{ SphereSweep_Union (SSWP_B_SPLINE, Pts, Radii, 20)
  pigment { red 0.125 }
  finish { specular 0.25 roughness 0.025 }
}
// end of spheresweep_infinity.pov
