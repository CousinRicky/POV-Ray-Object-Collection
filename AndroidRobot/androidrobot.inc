/* androidrobot.inc version 5.0A
 * Persistence of Vision Raytracer include file
 * POV-Ray Object Collection
 *
 * A 3-D model of Google's Android[TM] mascot.
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
 *
 * 2.0    2011-Jul-23  The code is separated into include and demo files.  R.C.
 *                     The code is made fully namespace compliant.
 *                     The pigment is removed from the robot and the
 *                     official color is declared separately.
 *                     The robot can be posed.
 *
 * 2.1    2012-Jun-26  Non-gamma-adjusted official color is added.         R.C.
 * 2.1a   2012-Jun-28  A correction is made to Google license statement    R.C.
 *
 * 2.2    2014-Oct-05  The arms are moved away from the body, as they      R.C.
 *                     appear in the 2D logo.
 *                     A backwards compatibility switch is added.
 *                     The Google license statement is upgraded.
 *
 * 3.0    2015-Apr-25  The robot is remodeled based on an SVG source.      R.C.
 *                     Tools are added to facilitate user modeling of
 *                     eyes.
 * 3.0a   2015-Apr-27  A correction is made to the user manual.            R.C.
 *
 * 4.0    2018-Sep-23  The short prefix is reinstated.                     R.C.
 *                     The official color is updated.
 *
 * 4.0.1  2019-Mar-22  View_POV_Include_Stack is utilized.                 R.C.
 *
 * 5.0    2020-Sep-19  The official color is updated to its 2019 value.    R.C.
 *                     The pre-3.7 nonlinear color adjustment is changed
 *                     from a power function to sRGB.
 *                     New body proportions are added, extrapolated from
 *                     the 2019 head logo.  Proportions of previous
 *                     versions remain selectable.
 *                     The head is available standalone.
 *
 * 5.0A   2021-Aug-14  The license text is updated.                        R.C.
 *
 * Implementation note: due to POV-Ray namespace scope leakage, local variables
 * and function parameters (other than x, y, and z) *must* be prefixed.
 */
#ifndef (AndroidRobot_Inc_Temp) #declare AndroidRobot_Inc_Temp = version;

//Take advantage of POV-Ray 3.7+ feature if and only if available:
#version max (3.5, min (3.8, version));

#ifdef (View_POV_Include_Stack)
  #debug concat
  ( "#include \"androidrobot.inc\" //version 5.0A\n  #version ",
    str(version,0,1), ";\n"
  )
#end

#ifndef (AndroidRobot_Version) #declare AndroidRobot_Version = 5.0; #end

//============================= PUBLIC CONSTANTS ===============================

#declare ANDROIDROBOT_HEIGHT = 1.65;
#declare ANDROIDROBOT_V_BASE = <0, -0.55, 0>;

//======================= INTERNAL (PRIVATE) FEATURES ==========================
// These features are not intended for direct use by the end user.

#declare Droid__fn_sRGB = function (x)
{ select (x - 0.04045, x / 12.92, pow ((x + 0.055) / 1.055, 2.4))
}

//---------------------- Ostmo model -----------------------

#declare DROID__ANTENNAE_ANGLE = 30;
#declare DROID__ANTENNAE_LENGTH = 0.2;
#declare DROID__EYE_HEIGHT = 0.8;

#macro AndroidRobot__V2
( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
)
  #local DROID_ZARM = (AndroidRobot_Version < 2.2? 0.6: 0.65);

  #local Droid_Arm = #if (Use_merge) merge #else union #end
  { sphere { <0, 0.5, 0>, 0.1 }
    cylinder { <0, 0.5, 0>, <0, 0.1, 0>, 0.1 }
    sphere { <0, 0.1, 0>, 0.1 }
  }

  #local Droid_Leg = #if (Use_merge) merge #else union #end
  { cylinder
    { <0, 0.5, 0>, <0, -0.2, 0>, 0.1
      translate <0, -0.25, 0>
    }
    sphere
    { <0, -0.2, 0>, 0.1
      translate <0, -0.25, 0>
    }
  }

  #if (Use_merge) merge #else union #end
  {
   //Torso
    cylinder { <0, 0.6, 0>, <0, -0.1, 0>, 0.5 }
    cylinder { <0, 0.1, 0>, <0, -0.2, 0>, 0.4 }
    torus { 0.4, 0.1 translate -0.1 * y }

   //Head
    object
    { AndroidRobot__Head_v2 (Use_merge)
      AndroidRobot_Head_x (x_Head)
    }

    object
    { Droid_Arm
      translate -0.5 * y //move to origin
      transform { x_Right_arm }
      translate 0.5 * y //move back from origin
      translate z*(-DROID_ZARM)
    }

    object
    { Droid_Arm
      translate -0.5 * y //move to origin
      transform { x_Left_arm }
      translate 0.5 * y //move back from origin
      translate z*(DROID_ZARM)
    }

    object
    { Droid_Leg
      translate 0.1 * y //move to origin
      transform { x_Left_leg }
      translate -0.1 * y //move back from origin
      translate z*(.25)
    }

    object
    { Droid_Leg
      translate 0.1 * y //move to origin
      transform { x_Right_leg }
      translate -0.1 * y //move back from origin
      translate z*(-.25)
    }

  }
#end

//--------------------------------------

#macro AndroidRobot__Head_v2 (Use_merge)

  #local Droid_Antenna = cylinder
  { <0, DROID__ANTENNAE_LENGTH, 0>, <0, 0, 0>, 0.02
  }

  #local Droid_EyeSocket = cylinder
  { <0, 0.5, 0>, <0, 0, 0>, 0.05
    rotate z*(-90)
    translate <0, DROID__EYE_HEIGHT, 0>
  }

  #if (Use_merge) merge #else union #end
  { difference
    { AndroidRobot_Head (0)
      object { Droid_EyeSocket translate z*(-.25) }
      object { Droid_EyeSocket translate z*(.25) }
    }
    object
    { Droid_Antenna
      rotate x*(-DROID__ANTENNAE_ANGLE)
      translate <0, 1, -0.25>
    }
    object
    { Droid_Antenna
      rotate x*(DROID__ANTENNAE_ANGLE)
      translate <0, 1, 0.25>
    }
  }
#end

//-------------------- Callwood model ----------------------

/* The messy values in this section were derived from the file
 * https://commons.wikimedia.org/wiki/File:Android_robot.svg, and transformed
 * such that the new version of the robot has the same size and position as the
 * Ostmo model.
 *
 * The Google Web page from which the Wikimedia image was taken now redirects
 * to a page (see the Google attribution in the header comment) with a different
 * SVG file that has only the head defined.  The proportions of this new SVG
 * file have been extrapolated for version 5.0 of AndroidRobot. --RC
 */

#macro AndroidRobot__V3
( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
)
  #if (AndroidRobot_Version < 5.0)
   // Radii
    #local DROID_RTORSO = 0.483731;
    #local DROID_RBUTT = 0.116946;     // minor radius
    #local DROID_RLIMB = 0.108441;
   // Side positions
    #local DROID_ZSHOULDER = 0.632571;
    #local DROID_ZHIP = 0.180735;
   // Vertical positions
    #local DROID_Y2TORSO = 0.615738;   // top
    #local DROID_Y1TORSO = -0.202352;  // bottom
    #local DROID_YSHOULDER = 0.525902; // center of curvature
    #local DROID_Y2LEG = 0.010277;     // top center of curvature
   // Length
    #local DROID_HLIMB = 0.451836;     // length excluding end radii
  #else
   // Radii
    #local DROID_RTORSO = 0.485546;
    #local DROID_RBUTT = 0.117385;     // minor radius
    #local DROID_RLIMB = 0.108848;
   // Side positions
    #local DROID_ZSHOULDER = 0.634945;
    #local DROID_ZHIP = 0.181413;
   // Vertical positions
    #local DROID_Y2TORSO = 0.620112;   // top
    #local DROID_Y1TORSO = -0.201047;  // bottom
    #local DROID_YSHOULDER = 0.529939; // center of curvature
    #local DROID_Y2LEG = 0.012379;     // top center of curvature
   // Length
    #local DROID_HLIMB = 0.453532;     // length excluding end radii
  #end

  #local DROID_ZBUTT = DROID_RTORSO - DROID_RBUTT;  // center of curvature
  #local DROID_YBUTT = DROID_Y1TORSO + DROID_RBUTT; // center of curvature
  #local DROID_YHIPF = DROID_Y1TORSO + DROID_RLIMB; // leg final translation
  #local DROID_YHIP0 = DROID_Y2LEG - DROID_YHIPF;   // leg translation to origin

  #local Droid_Limb = #if (Use_merge) merge #else union #end
  { sphere { 0, DROID_RLIMB }
    cylinder { 0, -DROID_HLIMB * y, DROID_RLIMB }
    sphere { -DROID_HLIMB * y, DROID_RLIMB }
  }

  #if (Use_merge) merge #else union #end
  {
   // torso
    cylinder { DROID_Y2TORSO * y, DROID_YBUTT * y, DROID_RTORSO }
    cylinder { (DROID_YBUTT + 0.001) * y, DROID_Y1TORSO * y, DROID_ZBUTT }
    torus { DROID_ZBUTT, DROID_RBUTT translate DROID_YBUTT * y }

   // head
    object
    { AndroidRobot__Head_v3 (Use_merge)
      AndroidRobot_Head_x (x_Head)
    }

    object
    { Droid_Limb
      transform { x_Right_arm }
      translate <0, DROID_YSHOULDER, -DROID_ZSHOULDER>
    }
    object
    { Droid_Limb
      transform { x_Left_arm }
      translate <0, DROID_YSHOULDER, DROID_ZSHOULDER>
    }
    object
    { Droid_Limb
      translate DROID_YHIP0 * y
      transform { x_Right_leg }
      translate <0, DROID_YHIPF, -DROID_ZHIP>
    }
    object
    { Droid_Limb
      translate DROID_YHIP0 * y
      transform { x_Left_leg }
      translate <0, DROID_YHIPF, DROID_ZHIP>
    }
  }
#end

//--------------------------------------

#macro AndroidRobot__Head_v3 (Use_merge)
  #if (AndroidRobot_Version < 5.0)
   // Angle
    #local DROID_AANTENNA = 29;
   // Radius
    #local DROID_RANTENNA = 0.015416;
   // Side position
    #local DROID_ZANTENNA = 0.108972;  // top center of curvature
   // Vertical positions
    #local DROID_YORIGIN = 0.871424;   // needed for antenna rotation
    #local DROID_Y2ANTENNA = 0.422600; // top center of curvature
    #local DROID_Y1ANTENNA = 0.034552; // corrected (though inconsequential)
  #else
   // Angle
    #local DROID_AANTENNA = 30;
   // Radius
    #local DROID_RANTENNA = 0.016810;
   // Side position
    #local DROID_ZANTENNA = 0.304231;  // top center of curvature
   // Vertical position
    // Origin is not needed because the 2019 SVG creates the antennae in place.
    #local DROID_Y2ANTENNA = 1.188860; // top center of curvature
   // Length
    #local DROID_HANTENNA = 0.18;      // sufficient length
  #end

  #local Droid_EyeSocket = cylinder
  { 0, x, AndroidRobot_Eye_radius()
    translate AndroidRobot_Eye_v() * <0, 1, 1>
  }

  #local Droid_Antenna = #if (Use_merge) merge #else union #end
  { #if (AndroidRobot_Version < 5.0)
      cylinder { DROID_Y1ANTENNA * y, DROID_Y2ANTENNA * y, DROID_RANTENNA }
      sphere { DROID_Y2ANTENNA * y, DROID_RANTENNA }
      translate DROID_ZANTENNA * z
      rotate DROID_AANTENNA * x
      translate DROID_YORIGIN * y
    #else
      sphere { 0, DROID_RANTENNA }
      cylinder { 0, -DROID_HANTENNA * y, DROID_RANTENNA }
      rotate DROID_AANTENNA * x
      translate <0, DROID_Y2ANTENNA, DROID_ZANTENNA>
    #end
  }

  #if (Use_merge) merge #else union #end
  { difference
    { object { AndroidRobot_Head (0) }
      object { Droid_EyeSocket }
      object { Droid_EyeSocket scale <1, 1, -1> }
    }
    object { Droid_Antenna }
    object { Droid_Antenna scale <1, 1, -1> }
  }
#end

//============================== PUBLIC MACROS =================================

#macro AndroidRobot_Eye_v()
  #local Droid_Test = AndroidRobot_Head (0)
  trace
  ( Droid_Test,
    ( AndroidRobot_Version < 3.0?
      <1, DROID__EYE_HEIGHT, 0.25>:
      ( AndroidRobot_Version < 5.0?
        <1, 0.871424, 0.223260>: <1, 0.837335, 0.223475>
      )
    ),
    -x
  )
#end

//--------------------------------------

#macro AndroidRobot_Eye_radius()
  ( AndroidRobot_Version < 3.0?
    0.05:
    (AndroidRobot_Version < 5.0? 0.040399: 0.040440)
  )
#end

//--------------------------------------

#macro AndroidRobot_Head (Outset)
  #if (AndroidRobot_Version < 3.0)
    difference
    { sphere { <0, 0.6, 0>, 0.5 + Outset }
      plane { y, 0.65 - Outset }
    }
  #else
    #if (AndroidRobot_Version < 5.0)
      #local DROID_RZHEAD = 0.483731;
      #local DROID_RYHEAD = 0.446521;
      #local DROID_YCHEAD = 0.653479;
      #local DROID_Y1HEAD = 0.654011;
    #else
      #local DROID_RZHEAD = 0.487201;
      #local DROID_RYHEAD = 0.481096;
      #local DROID_YCHEAD = 0.618904;
      #local DROID_Y1HEAD = 0.658529;
    #end
    intersection
    { sphere
      { 0, 1
        scale <DROID_RZHEAD, DROID_RYHEAD, DROID_RZHEAD> + Outset
        translate DROID_YCHEAD * y
      }
      plane { -y, Outset - DROID_Y1HEAD }
    }
  #end
#end

//--------------------------------------

#macro AndroidRobot_Make_head (Use_merge)
  #if (AndroidRobot_Version < 3.0)
    AndroidRobot__Head_v2 (Use_merge)
  #else
    AndroidRobot__Head_v3 (Use_merge)
  #end
#end

//--------------------------------------

#macro AndroidRobot_Head_x (x_Head)
  #local droid_Neck =
  ( AndroidRobot_Version < 3.0?
    0.625: (AndroidRobot_Version < 5.0? 0.634874: 0.639320)
  );
  transform
  { translate -droid_Neck * y // Move center of neck to origin
    transform { x_Head }
    translate droid_Neck * y // Move back from origin
  }
#end

//--------------------------------------

#macro AndroidRobot_Posed
( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
)
  #if (AndroidRobot_Version < 3.0)
    AndroidRobot__V2
    ( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
    )
  #else
    AndroidRobot__V3
    ( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
    )
  #end
#end

//--------------------------------------

#macro AndroidRobot (Use_merge)
  AndroidRobot_Posed
  ( Use_merge, transform{}, transform{}, transform{}, transform{}, transform{}
  )
#end

//--------------------------------------

#macro AndroidRobot_Set_color (Version)
  #declare DROID_C_RAWCOLOR = rgb
  ( Version < 4?
    <164, 198, 57>:   // #A4C639
    ( Version < 5?
      <120, 194, 87>: // #78C257
      <61, 220, 132>  // #3DDC84
    )
  ) / 255;
  #if (version < 3.7)
    #declare DROID_C_COLOR = rgb
    < Droid__fn_sRGB (DROID_C_RAWCOLOR.red),
      Droid__fn_sRGB (DROID_C_RAWCOLOR.green),
      Droid__fn_sRGB (DROID_C_RAWCOLOR.blue)
    >;
  #else
    #declare DROID_C_COLOR = srgbft DROID_C_RAWCOLOR;
  #end
  #declare ANDROIDROBOT_C_RAWCOLOR = DROID_C_RAWCOLOR;
  #declare ANDROIDROBOT_C_COLOR = DROID_C_COLOR;
#end

AndroidRobot_Set_color (AndroidRobot_Version) // Initialize.

//--------------------- GRANDFATHER ------------------------

#macro AndroidRobot_posed
( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
)
  AndroidRobot_Posed
  ( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
  )
#end

//=============================  SHORT PREFIXES ================================

#declare DROID_HEIGHT = ANDROIDROBOT_HEIGHT;
#declare DROID_V_BASE = ANDROIDROBOT_V_BASE;
#macro Droid_Eye_v() AndroidRobot_Eye_v() #end
#macro Droid_Eye_radius() AndroidRobot_Eye_radius() #end
#macro Droid_Head (Outset) AndroidRobot_Head (Outset) #end
#macro Droid_Head_x (x_Head) AndroidRobot_Head_x (x_Head) #end
#macro Droid_Make_head (Use_merge) AndroidRobot_Make_head (Use_merge) #end
#macro Droid_Posed
( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
)
  AndroidRobot_Posed
  ( Use_merge, x_Head, x_Left_arm, x_Right_arm, x_Left_leg, x_Right_leg
  )
#end
#macro Droid (Use_merge) AndroidRobot (Use_merge) #end
#macro Droid_Set_color (Version) AndroidRobot_Set_color (Version) #end

//==============================================================================

#version AndroidRobot_Inc_Temp;
#end
// end of androidrobot.inc
