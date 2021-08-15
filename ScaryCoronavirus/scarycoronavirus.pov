/* scarycoronavirus.pov 1.0B
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection demo
 *
 * A demo scene for scarycoronavirus.inc: a cell being attacked by
 * coronaviruses.
 *
 * Copyright (C) 2020, 2021 Richard Callwood III.  Some rights reserved.
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
 * Vers  Date         Notes
 * ----  ----         -----
 *       2020-May-05  Started.
 * 1.0A  2020-May-09 Completed.
 * 1.0B  2021-Aug-15  The license text is updated.
 */
#version max (3.5, min (3.8, version));

// Scene quality:
//   0 = smooth virions, no media, no DOF, no radiosity unless 2-pass
//   1 = lumpy virions, scattering media, no DOF, no radiosity unless 2-pass
//   2 = lumpy virions, scattering media, DOF, radiosity
#ifndef (Quality) #declare Quality = 2; #end
// Multiplier for the number of virus particles:
#ifndef (Quantity) #declare Quantity = 1; #end
// Radiosity pass:
//   0 = Single pass
//   1 = Pass 1 of 2 (forces radiosity)
//   2 = Pass 2 of 2 (forces radiosity)
#ifndef (Pass) #declare Pass = 0; #end
// Wide angle overview?
#ifndef (Test_cam) #declare Test_cam = no; #end
// Thumbnail?
#ifndef (Thumbnail) #declare Thumbnail = no; #end

#include "scarycoronavirus.inc"
#include "math.inc"
#include "rand.inc"
#include "roundedge.inc"

global_settings
{ assumed_gamma 1
  max_trace_level 100
  adc_bailout 0.02
  #if (Quality >= 2 | Pass >= 1)
    //photons { spacing 0.9 autostop 0 /*media 100*/ }
    #declare Rad_filename = "scarycoronavirus_tmp.rad"
    radiosity
    { error_bound 0.5
      recursion_limit 2
      #switch (Pass)
        #case (0) // pass 1/1
          count 200
          normal on
          #if (version < 3.7)
            pretrace_start 0.08
            pretrace_end 0.01
          #else
            pretrace_start 64 / image_width
            pretrace_end 2 / image_width
          #end
          #break
        #case (1) // pass 1/2 (render at 1/2 size, -A -F)
          count 200
          #if (version < 3.7)
            pretrace_start 0.08
            pretrace_end 0.01
            save_file Rad_filename
          #else
            pretrace_start 32 / image_width
            pretrace_end 1 / image_width
          #end
          #break
        #case (2) // pass 2/2
          always_sample off
          count 1
          pretrace_start 1
          pretrace_end 1
          #if (version < 3.7) load_file Rad_filename #end
          #break
      #end
    }
  #end
}

#default { finish { diffuse 0.6 ambient (Quality >= 2? 0: 0.1) } }

camera
{ #if (Test_cam)
    ultra_wide_angle
    right 4/3 * x
    up y
    angle 240
  #else
    right 4/3 * x
    up y
    angle 40
    #if (Quality >= 2)
      aperture 0.5
      focal_point (Thumbnail? 26: 46) * z
      blur_samples (version < 3.7? 50: 19) // use anti-aliasing for 3.7+
    #end
  #end
}

light_source
{ <-1, 2, -1> * 5000, rgb 1.5
  parallel point_at 0
}

sky_sphere
{ pigment
  { gradient y color_map
    { #declare I = 0;
      #while (I <= 20)
        #declare Y = I / 20;
        [Y rgb pow (Y, 2.5) * 0.5]
        #declare I = I + 1;
      #end
    }
    scale -2
    translate -y
  }
}

//============================= CELL DEFINITIONS ===============================

#declare RUPPER = 400;
#declare RHUPPER = 300;
#declare PV_CTR_UPPER = <0, 150, 0>;
#declare RLOWER = 350;
#declare RHLOWER = 250;
#declare PV_CTR_LOWER = <-50, -200, 0>;
#declare MARGIN = 100;
#declare MEMBRANE = 0.4;

#declare Upper_field = RUPPER + MARGIN;
#declare Lower_field = RLOWER + MARGIN;
#declare RInUpper = RUPPER - MEMBRANE;
#declare RhInUpper = RHUPPER - MEMBRANE;
#declare RInLower = RLOWER - MEMBRANE;
#declare RhInLower = RHLOWER - MEMBRANE;

#declare Outer = blob
{ sphere
  { 0, Upper_field, RE_fn_Blob_strength (RUPPER, Upper_field)
    scale <1, RHUPPER / RUPPER, 1>
    translate PV_CTR_UPPER
  }
  sphere
  { 0, Lower_field, RE_fn_Blob_strength (RLOWER, Lower_field)
    scale <1, RHLOWER / RLOWER, 1>
    translate PV_CTR_LOWER
  }
}

#declare Inner = blob
{ sphere
  { 0, Upper_field, RE_fn_Blob_strength (RInUpper, Upper_field)
    scale <1, RhInUpper / RInUpper, 1>
    translate PV_CTR_UPPER
  }
  sphere
  { 0, Lower_field, RE_fn_Blob_strength (RInLower, Lower_field)
    scale <1, RhInLower / RInLower, 1>
    translate PV_CTR_LOWER
  }
}

#declare PV_CELL = <200, 0, 2000>;

//================================= VIRUSES ====================================

#declare t_Envelope = texture { pigment { rgb <0.8, 1, 0.2> } }
#declare t_EProtein = texture { pigment { rgb <0.8, 0.4, 0> } }
#declare t_MProtein = texture { pigment { rgb <0.2, 0.1, 0.9> } }
#declare t_Spike = texture { pigment { rgb <0.5, 0.08, 0.1> } }
#declare VSCALE = 1 / 10; // 1 POV unit = 10 nm

#declare Sizes = seed (101);

#macro Get_size()
  min (200, max (50, Rand_Normal (110, 25, Sizes)))
#end

//-------- foreground virion -----------

object
{ ScaryCoronavirus
  ( 110, t_Envelope, t_Spike, t_MProtein, t_EProtein,
    (Quality >= 1? SCOV_BLOB_QUALITY: SCOV_DRAFT_QUALITY)
  )
  scale VSCALE
  translate (Thumbnail? <-3.5, -1, 30>: <-10, -3, 50>)
}

//--------- floating virions -----------

#declare pv_Start = <0, 0, 100>;
#declare pv_End = <PV_CELL.x, 0, PV_CELL.z - RUPPER * 1.2>;
#declare Jitters = seed (97);

#declare END = 12;
#declare Path = spline
{ natural_spline
  #declare T = 0;
  #while (T <= END)
    T / END,
    < (1 - cos (Interpolate (T, 0, END, 0, pi, 1))) * pv_End.x / 2,
      (cos (Interpolate (T, 0, END, 0, 2 * pi, 1)) - 1) * 50,
      Interpolate (T, 0, END, pv_Start.z, pv_End.z, 1)
    >
    #declare T = T + 1;
  #end
}

#declare NLOOSE = floor (100 * Quantity + 0.5);
#declare I = 0;
#while (I < NLOOSE)
  #local T = I / NLOOSE;
  #local pv_Posn =
    Path (1 - Interpolate (T, 1, 0, 0, 1, 2))
  + vrotate
    ( Interpolate (T, 0, 1, 15, 300, 1) * rand (Jitters) * x,
      360 * rand (Jitters) * z
    );
  object
  { ScaryCoronavirus
    ( Get_size(), t_Envelope, t_Spike, t_MProtein, t_EProtein,
      (Quality >= 1 & pv_Posn.z < 250? SCOV_BLOB_QUALITY: SCOV_DRAFT_QUALITY)
    )
    scale VSCALE
    translate pv_Posn
  }
  #declare I = I + 1;
#end

//================================= ASSEMBLY ===================================

// Approximate number of points in a unit sphere:
#declare NENGAGED = floor (300 * Quantity + 0.5);

#declare N = 6 / pi * NENGAGED; // number of points in a 2X unit cube
#declare Places = seed (103);
#declare v_Norm = <0,0,0>;
#declare Tmp = 0;
union
{//----- cell -----
  union
  { object
    { Outer hollow
      pigment { rgbf 1 }
      finish
      { reflection { 1 fresnel } conserve_energy
        specular 0.601
        roughness 0.01
      }
      normal { bumps 0.1 scale 5 }
      interior { ior 1.48 }
    }
    object
    { Inner hollow
      pigment { rgbf 1 }
      finish
      { reflection { 1 fresnel } conserve_energy
        specular 0.0846
        roughness 0.01
      }
      normal { bumps 0.1 scale 5 }
      interior
      { ior 1.35
        fade_power 1000
        fade_distance 300
        #if (Quality >= 1)
          fade_color rgb VPow (<0.9, 0.6, 0.1>, 0.4)
          media
          { scattering { 3, rgb <0.0001, 0.0002, 0.0003> }
            samples 3, 3
            aa_level 2
          }
        #else
          fade_color rgb VPow (<0.9, 0.75, 0.5>, 0.4)
        #end
      }
    }
    sphere
    { -50 * y, 120
      pigment { rgb 0.3 }
    }
    //photons { target collect off reflection on refraction on }
    //split_union off
  }
 //----- virions in contact with cell -----
  #declare I = 0;
  #while (I < N)
    #declare v_Dir = <rand (Places), rand (Places), rand (Places)> * 2 - 1;
   // For unbiased coverage, this #if pares away points outside the unit sphere:
    #if (vlength (v_Dir) < 1)
      #declare pv_Contact = trace (Outer, 0, v_Dir, v_Norm);
      /* Testing with 3.5 (official Linux build) yields odd results:
      #debug concat ("direction = <", vstr (3, v_Dir, ", ", 0, 6), ">\n")
      #debug concat ("trace = <", vstr (3, pv_Contact, ", ", 0, 4), ">\n")
      #debug concat
      ( "normal = <", vstr (3, v_Norm, ", ", 0, 4), ">, length ",
        str (vlength (v_Norm), 0, 4), "\n"
      )*/
      #declare Size = Get_size();
      object
      { ScaryCoronavirus
        ( Size, t_Envelope, t_Spike, t_MProtein, t_EProtein,
          SCOV_DRAFT_QUALITY
        )
        scale VSCALE
        translate pv_Contact + v_Norm * Size * VSCALE / 2
      }
      #declare Tmp = Tmp + 1;
    #end
    #declare I = I + 1;
  #end
  translate PV_CELL
}

// end of scarycoronavirus.pov
