MODULE GR_Demo;

(* GR_xxxx demonstration:
   Test code for Ellipses - Normalized Coordinates
   Use all aspect ratios, with hardware (EGA/VGA) colours in DefaultAspect
   and RBG colours in others. *)

FROM GR_Hardware   IMPORT VideoConfig, GetVideoConfig, SetVideoMode,
                          CGA_Adapter, EGA_Adapter, VGA_Adapter, HGC_Adapter,
                          CGA_Mode_4, EGA_Mode_16, VGA_Mode_18,
                          HGC_Graphics_Mode;
FROM GR_Attributes IMPORT AspectSetting, DefaultAspect, GetWriteMode,
                          MaxYAspect, SetBgColour, SetColour,
			  SetRGBBgColour, SetRGBColour,
			  SetNDCClipRect, SetNDCAspect, SetWriteMode,
			  WriteMode, xorMode,
                          White, Red, LightBlue, Yellow, Black;
FROM GR_Primitives IMPORT ClearDisplay, EllipseNDC, LineNDC;

FROM InOut    IMPORT WriteString, WriteLn, WriteCard;
FROM Terminal IMPORT GetKeyStroke;

VAR
    vinfo   : VideoConfig;
    oldMode,
    fg, bg  : CARDINAL;
    ok      : BOOLEAN;
    ch      : CHAR;
    x1, y1,
    x2, y2  : REAL;
    a       : AspectSetting;

BEGIN
    GetVideoConfig(vinfo);
    oldMode := vinfo.mode;
    IF vinfo.adapter = CGA_Adapter THEN
           ok := SetVideoMode(CGA_Mode_4);
           WriteString("set CGA"); WriteLn;
    ELSIF vinfo.adapter = EGA_Adapter THEN
           ok := SetVideoMode(EGA_Mode_16);
           WriteString("set EGA"); WriteLn;
    ELSIF vinfo.adapter = VGA_Adapter THEN
           ok := SetVideoMode(VGA_Mode_18);
           WriteString("set VGA"); WriteLn;
    ELSIF vinfo.adapter = HGC_Adapter THEN
           ok := SetVideoMode(HGC_Graphics_Mode);
           WriteString("set HGC"); WriteLn;
    ELSE
           WriteString("Graph error : unsupported video adapter.");
           WriteLn;
           HALT;
    END;
    
    GetVideoConfig(vinfo);

    FOR a := DefaultAspect TO MaxYAspect DO
	IF a = DefaultAspect THEN
	  SetBgColour(Red);
	ELSE
	  SetRGBBgColour(0.5, 0.0, 0.0); (* red - >0.75,0.0,0.0 is pink? *)
	END;
        SetNDCAspect(a);
        SetNDCClipRect(0.0, 0.0, 1.0, 1.0);
        ClearDisplay;

        EllipseNDC(-0.1, -0.1, 1.1, 1.1);
        EllipseNDC( 0.0,  0.0, 1.0, 1.0);
        EllipseNDC( 0.2,  0.2, 0.8, 0.8);

        x1 := 0.3; y1 := 0.3;
        x2 := 0.7; y2 := 0.7;
        SetNDCClipRect(x1, y1, x2, y2);
	IF a = DefaultAspect THEN
	  SetColour(Yellow);
	ELSE
          SetRGBColour(1.0, 1.0, 0.0); (* yellow *)
	END;
        LineNDC(x1, y1, x2, y1);
        LineNDC(x2, y1, x2, y2);
        LineNDC(x2, y2, x1, y2);
        LineNDC(x1, y2, x1, y1);

	IF a = DefaultAspect THEN
	  SetColour(LightBlue);
	ELSE
          SetRGBColour(0.0, 0.0, 1.0); (* (light) blue *)
	END;
        EllipseNDC(0.25, 0.25, 0.75, 0.75);
        EllipseNDC(0.3,  0.3,  0.7,  0.7);

	IF a = DefaultAspect THEN
	  SetColour(White);
	ELSE
          SetRGBColour(1.0, 1.0, 1.0); (* white *)
	END;
        EllipseNDC(0.4,  0.1,  0.6,  0.9);

        GetKeyStroke(ch);            (* wait for user to input   *)
	IF a = DefaultAspect THEN
	  SetBgColour(Black);
	ELSE
          SetRGBBgColour(0.0, 0.0, 0.0); (* black *)
	END;
        SetNDCClipRect(0.0, 0.0, 1.0, 1.0);
        ClearDisplay;
    END;  (* FOR *)

    ok := SetVideoMode(oldMode); (* return to text mode 3    *)

END GR_Demo.
