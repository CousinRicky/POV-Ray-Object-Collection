/* rc3metal_brushed.pov version 1.3A
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection demo
 *
 * Contrast the brushed metal normals from textures.inc and rc3metal.inc.
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
 * Vers.  Date         Comments
 * -----  ----         --------
 *        2011-Aug-27  Created.
 *        2012-Sep-22  Adapted for Object Collection: my custom-written include
 *                     files are dropped and replaced with in-line code.
 * 1.0    2013-Mar-09  Uploaded.
 * 1.1    2013-Apr-23  No changes.
 * 1.2    2013-Sep-06  No changes.
 * 1.2.1  2016-Mar-11  The lighting is replaced with calculated values from my
 *                     private render rig.
 * 1.3    2019-Sep-13  No changes.
 * 1.3A   2021-Aug-14  The license text is updated.
 */
// +W240 +H240 +A0.05 +AM1 +R5 +J +KFF2
#version 3.5;

#include "shapes.inc"
#include "textures.inc"
#include "rc3metal.inc"

global_settings { assumed_gamma 1 }

camera
{ location <-3.4729, 3.8356, -3.4729>
  look_at y
  right x
  up y
  angle 29.6299
}
light_source
{ <-3.3125, 7.6250, -5.7374>, rgb 4045.5
  fade_power 2 fade_distance 0.10417
  spotlight point_at y radius 45 falloff 90
}

#default { finish { diffuse 0.6 ambient rgb <0.15814, 0.16950, 0.11646> } }

box
{ -<7, 1, 7>, <7, 9, 7> hollow
  pigment { rgb 1 }
}

plane
{ y, 0
  pigment { checker rgb <0.0, 0.5, 0.08333> rgb <1.0, 0.75, 0.0> }
}

object
{ Round_Box_Union (<-1, 0, -1>, <1, 2, 1>, 0.25)
  #switch (frame_number)
    #case (1) texture { Brushed_Aluminum } #break
    #case (2) texture { Brushed_Aluminum normal { RC3M_n_Brushed } } #break
  #end
}
// end of rc3metal_brushed.pov
