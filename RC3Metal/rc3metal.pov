/* rc3metal.pov version 1.3A
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection demo
 *
 * Demo of rc3metal.inc
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
 *        2011-Sep-23  Created.
 * 1.0    2013-Mar-09  Uploaded.
 * 1.1    2013-Apr-23  No changes to demo.
 * 1.2    2013-Sep-06  RC3Metal_Set_highlight() is used.
 *        2015-May-30  The specularity of the spheres is boosted a bit.
 *        2016-Feb-24  Outdoor colors are tweaked.
 * 1.2.1  2016-Mar-11  A yellower gold is used.
 *        2019-Aug-23  The POV-Ray version is auto-detected.
 *        2019-Aug-27  The specularity of the metals is boosted a lot more.
 *        2019-Aug-27  RC3Metal_Set_highlight (0) is set for all metals except
 *                     the galvanized.
 *        2019-Sep-03  The outdoor lighting is reformed, and clouds are added to
 *                     moderate the sky color.
 *        2019-Sep-12  The indoor lighting is reformed.
 * 1.3    2019-Sep-13  Photons are used on some objects.
 * 1.3A   2021-Aug-14  The license text is updated.
 */
/*
 * POV-Ray 3.5, 3.7, 3.8 // +A0.05 +AM1 +R5 +W800 +H600
 * POV-Ray 3.6           // +A0.02 +AM1 +R5 +W800 +H600
 * Fast test             // +A Declare=Fast=1 Declare=Rad=0
 */
#version max (3.5, min (3.8, version));

//-------------------------------- PARAMETERS ----------------------------------

#ifndef (Fast) #declare Fast = no; #end
#ifndef (Rad) #declare Rad = 2; #end

/*       Blurred
 * Fast  Reflection  Photons  Shadows
 * ----  ----------  -------  -------
 * no    yes         yes      fuzzy
 * yes   no          no       sharp
 *
 * Rad  Radiosity
 * ---  ---------
 *  0   none
 *  1   low quality
 *  2   high quality
 */

#declare INDOOR_BRIGHTNESS = 0.75;
#declare OUTDOOR_BRIGHTNESS = 40; // realistic value is ~3000 * indoor
#declare C_LIGHT = rgb <1.140, 0.966, 0.781>;
#declare C_LIGHT = C_LIGHT * INDOOR_BRIGHTNESS;
#declare RC3M_Ambient = rgb (Rad? 0: <0.108, 0.124, 0.153>);

#declare RROOM = 6;
#declare HROOM = 8;
#declare DWALL = 0.5;
#declare XWIN = 3;
#declare RWWIN = 1.25;
#declare YSILL = 3;
#declare HWIN = 3.5;
#declare RLIGHT = 1/3;

#declare LV_LIGHT = <0, HROOM - 1, 0>;
#declare LV_AIM = <0, 1, RROOM - 2>;
#declare lv_Floor_spot = <LV_AIM.x, 0, LV_AIM.z>;

//-------------------------------- LIBRARIES -----------------------------------

#include "colors.inc" //required by skies.inc
#include "shapes.inc"
#include "skies.inc"
#include "rc3metal.inc"
RC3Metal_Set_highlight (0)

//--------------------------------- SETTINGS -----------------------------------

#default { finish { ambient RC3M_Ambient } }

global_settings
{ assumed_gamma 1
  max_trace_level 10
  #if (Rad)
    radiosity
    { always_sample no
      error_bound 0.5
      nearest_count 1
      #if (Rad = 1)
        count 200
        pretrace_end 0.01
        recursion_limit 1
        brightness 1.25
      #else
        count 400
        pretrace_end 2 / max (image_width, image_height)
        pretrace_start 32 / max (image_width, image_height)
        recursion_limit 2
      #end
    }
  #end
  #if (!Fast) photons { spacing 0.002 autostop 0 } #end
}

camera
{ location <1 - RROOM, 5, 1 - RROOM>
  look_at LV_AIM + <0.14, -0.14, 0>
  angle 22.5
}

//------------------------------- ENVIRONMENT ----------------------------------

//------- outdoor illumination ---------

sphere
{ 0, 1 hollow
  texture
  { pigment
    { gradient y color_map
      { [0 rgb <0.118, 0.223, 0.485>]
        [1 rgb <0.008, 0.031, 0.173>]
      }
    }
    finish
    { diffuse 0
      #if (version < 3.7)
        ambient OUTDOOR_BRIGHTNESS
      #else
        ambient 0 emission OUTDOOR_BRIGHTNESS
      #end
    }
  }
  texture
  { pigment { P_Cloud1 scale 0.075 }
   // Use a finish to adjust the brightness of P_Cloud1:
    finish
    { diffuse 0
      #if (version < 3.7)
        ambient OUTDOOR_BRIGHTNESS * 0.382
      #else
        ambient 0 emission OUTDOOR_BRIGHTNESS * 0.382
      #end
    }
  }
  scale 10000
}

plane
{ y, -1
  pigment { rgb <0.075, 0.079, 0.030> }
  finish
  { diffuse 0
    #if (version < 3.7) ambient OUTDOOR_BRIGHTNESS
    #else ambient 0 emission OUTDOOR_BRIGHTNESS
    #end
  }
}

//--------------- room -----------------

union
{ box { -z, 1 scale <DWALL, HROOM, RROOM + DWALL> translate RROOM * x }
  box { -z, 1 scale <-DWALL, HROOM, RROOM + DWALL> translate -RROOM * x }
  box { -x, 1 scale <RROOM, HROOM, DWALL> translate RROOM * z }
  difference
  { box { -x, 1 scale <RROOM, HROOM, -DWALL> }
    box { <-RWWIN, 0, -1>, <RWWIN, HWIN, 1> translate <-XWIN, YSILL, 0> }
    box { <-RWWIN, 0, -1>, <RWWIN, HWIN, 1> translate <XWIN, YSILL, 0> }
    translate -RROOM * z
  }
  box { y - 1, 1 scale RROOM + DWALL translate HROOM * y }
  pigment { rgb 1 }
}

box { -1, 1 - y scale RROOM + DWALL pigment { checker rgb 1 rgb 0.05 } }

//------- indoor illumination ----------

#declare dLight = vlength (LV_LIGHT - LV_AIM);
// Use a large fade distance because, prior to POV-Ray 3.7,
// no_radiosity is unavailable for the looks_like:
#declare rFade = dLight / 3;
union
{ light_source
  { 0, C_LIGHT * (1 + pow (dLight / rFade, 2)) / 2
    fade_power 2 fade_distance rFade
    #if (!Fast)
      #declare RES = 9;
     // See Area light idiosyncrasies in p.b.i for this formula:
      #declare Size = 2 * RLIGHT * (RES - 1) / RES;
      area_light Size * x, Size * z, RES, RES
      circular orient adaptive 1 jitter
    #end
    looks_like
    { sphere
      { 0, RLIGHT
        pigment { C_LIGHT }
        finish
        { diffuse 0
          #if (version < 3.7) ambient #else ambient 0 emission #end
            pow (dLight / rFade, 2)
        }
      }
    }
  }
  union
  { cylinder { RLIGHT * y, (HROOM - LV_LIGHT) * y, 1/16 }
    cone { y, 1, 2*y, 0 open scale RLIGHT / sqrt(2) }
    RC3Metal (RC3M_C_BRONZE, 0.25, 0.5)
  }
  translate LV_LIGHT
}

//----------------------------------- DEMO -------------------------------------

#macro Stand (c_Medal, Height, s_Label)
  #local R = 0.4;
  #local R1 = 0.1;
  #local R2 = 0.04;
  #local R3 = 0.01;
  #local c_Bronze = RC3M_C_BRONZE_WARM * 0.4 + RC3M_C_BRONZE * 0.3;
  #local Platform = union
  { difference
    { cylinder { (Height - 2*R1 - R2) * y, Height * y, R - R1 }
      torus { R - R1, R2 translate (Height - 2*R1 - R2) * y }
    }
    cylinder { (2*R1 + R2) * y, (Height - 2*R1 - R2) * y, R - R1 - R2 }
    difference
    { cylinder { R1 * y, (2*R1 + R2) * y, R - R1 }
      torus { R - R1, R2 translate (2*R1 + R2) * y }
    }
    cylinder { R3 * y, R1 * y, R }
    torus { R - R1, R1 translate (Height - R1) * y }
    torus { R - R1, R1 translate R1 * y }
    torus { R - R3, R3 translate R3 * y }
    photons { target collect off reflection on }
  }
  union
  { sphere
    { y, 1 scale 0.5
      RC3Metal (c_Medal, 0.9, 0.5)
      translate Height * y
    }
    #if (Fast)
      object { Platform RC3Metal (c_Bronze, 0.25, 0.5) }
    #else
      RC3Metal_Blur
      ( Platform, RC3Metal_Texture (c_Bronze, 0.5, 0.5), 0.35, 0.001, 10
      )
    #end
    #local o_Label = text { ttf "cyrvetic.ttf" s_Label 1, 0 }
    object
    { o_Label Center_Trans (o_Label, x)
      scale <0.6, 0.6, -0.005>
      RC3Metal (RC3M_C_ALUMINUM, 0.5, 0.5)
      #if (!Fast) normal { RC3M_n_Brushed } #end
      translate <0, 1/24, -R>
      photons { target collect off reflection on }
    }
  }
#end

object
{ Stand (RC3M_C_GOLD_LIGHT, 1, "1")
  translate lv_Floor_spot + <0, 0, 1>
}
object
{ Stand (RC3M_C_SILVER, 0.75, "2")
  translate lv_Floor_spot + <-1.2, 0, 1>
}
object
{ Stand (RC3M_C_BRONZE_NEW, 0.5, "3")
  translate lv_Floor_spot + <1.2, 0, 1>
}

sphere
{ y, 1 scale 0.5
  uv_mapping texture
  { RC3Metal_Finish (0.8, 0.5)
    pigment //"antique brass"
    { bozo color_map
      { [0.4 RC3M_C_BRONZE_COOL * 0.7]
        [0.6 RC3M_C_BRASS_COOL * 0.7]
      }
    }
    #if (!Fast) normal { bozo 0.01 } #end
    scale <1, 0.002, 1>
  }
  rotate -90 * y //Turn uv-mapped seam so it doesn't reflect in chrome sphere
  translate lv_Floor_spot + <-1.2, 0, -1>
}

sphere
{ y, 1 scale 0.5
  RC3Metal (RC3M_C_CHROME, 1, 0.5)
  translate lv_Floor_spot + <0, 0, -1>
}

RC3Metal_Set_highlight (0.3)
#declare t_Galv = RC3Metal_Galvanized_t
( RC3M_C_ZINC, RC3M_C_STEEL, 0, 0.8, 0.25, 0.5, 0.4
)
object
{ Wire_Box_Union (-<0.5, 0, 0.5>, <0.5, 1, 0.5>, 1/12)
  texture { t_Galv scale 0.05 }
  translate lv_Floor_spot + <1.2, 0, -1>
}
// end of rc3metal.pov
