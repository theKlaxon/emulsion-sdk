"Resource/UI/MainMenu.res"
{
	"MainMenu"
	{
		"ControlName"				"Frame"
		"fieldName"					"MainMenu"
		"xpos"						"0"
		"ypos"						"0"
		"wide"						"f0"
		"tall"						"f0"
		"autoResize"				"0"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"PaintBackgroundType"		"0"
	}
						
	// Single player
	"BtnPlaySolo"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnPlaySolo"
		"xpos"						"88"		[$GAMECONSOLE && ($GAMECONSOLEWIDE && !$ANAMORPHIC)]
		"xpos"						"63"		[$GAMECONSOLE && (!$GAMECONSOLEWIDE || $ANAMORPHIC)]	
		"xpos"						"88"		[!$GAMECONSOLE && $WIN32WIDE]
		"xpos"						"63"		[!$GAMECONSOLE && !$WIN32WIDE]	
		"ypos"						"220"		[$GAMECONSOLE]  
		"ypos"						"198"		[!$GAMECONSOLE] 	
		"wide"						"280"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnQuit"	[$GAMECONSOLE]
		"navUp"						"BtnEconUI"		[!$GAMECONSOLE]
		"navDown"					"BtnCoOp"
		"labelText"					"#PORTAL2_MainMenu_Solo"
		"style"						"MainMenuButton"
		"command"					"SoloPlay"
		"ActivationType"			"1"
		"FocusDisabledBorderSize"	"1"
	}

	
	"BtnCoOp"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnCoOp"
		"xpos"						"88"		[$GAMECONSOLE && ($GAMECONSOLEWIDE && !$ANAMORPHIC)]
		"xpos"						"63"		[$GAMECONSOLE && (!$GAMECONSOLEWIDE || $ANAMORPHIC)]	
		"xpos"						"88"		[!$GAMECONSOLE && $WIN32WIDE]
		"xpos"						"63"		[!$GAMECONSOLE && !$WIN32WIDE]	
		"ypos"						"250"		[$GAMECONSOLE]  
		"ypos"						"228"		[!$GAMECONSOLE] 	
		"wide"						"280"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnQuit"					[$GAMECONSOLE]
		"navUp"						"BtnPlaySolo"				[!$GAMECONSOLE]
		"navDown"					"BtnCommunity"
		"labelText"					"#PORTAL2_MainMenu_CoOp"
		"style"						"MainMenuButton"
		"command"					"CoopPlay"
		"ActivationType"			"1"
		"FocusDisabledBorderSize"	"1"
	}
	
	// Community Maps
	"BtnCommunity" [!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnCommunity"
		"xpos"						"88"	[$GAMECONSOLE && ($GAMECONSOLEWIDE && !$ANAMORPHIC)]
		"xpos"						"63"	[$GAMECONSOLE && (!$GAMECONSOLEWIDE || $ANAMORPHIC)]	
		"xpos"						"88"	[!$GAMECONSOLE && $WIN32WIDE]
		"xpos"						"63"	[!$GAMECONSOLE && !$WIN32WIDE]	
		"ypos"						"280"	[$GAMECONSOLE]  
		"ypos"						"258"	[!$GAMECONSOLE] 	
		"wide"						"280"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnOptions"	[$GAMECONSOLE]
		"navUp"						"BtnCoOp"		[!$GAMECONSOLE]
		"navDown"					"BtnOptions"
		"labelText"					"#PORTAL2_MainMenu_Community"
		"style"						"MainMenuButton"
		"command"					"CreateChambers"
		"ActivationType"			"1"
		"FocusDisabledBorderSize"	"1"
	}
	
	"BtnOptions"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnOptions"
		"xpos"						"88"	[$GAMECONSOLE && ($GAMECONSOLEWIDE && !$ANAMORPHIC)]
		"xpos"						"63"	[$GAMECONSOLE && (!$GAMECONSOLEWIDE || $ANAMORPHIC)]	
		"xpos"						"88"	[!$GAMECONSOLE && $WIN32WIDE]
		"xpos"						"63"	[!$GAMECONSOLE && !$WIN32WIDE]	
		"ypos"						"310"	[$GAMECONSOLE]  
		"ypos"						"288"	[!$GAMECONSOLE] 
		"wide"						"280"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnCoOp"			[$GAMECONSOLE]
		"navUp"						"BtnCommunity"	[!$GAMECONSOLE]
		"navDown"					"BtnPlaySolo"		[$GAMECONSOLE]
		"navDown"					"BtnExtras"			[!$GAMECONSOLE]
		"labelText"					"#PORTAL2_MainMenu_Options"
		"style"						"MainMenuButton"
		"command"					"Options"
		"ActivationType"			"1"
	}
	
	"BtnExtras" [!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnExtras"
		"xpos"						"88"	[$GAMECONSOLE && ($GAMECONSOLEWIDE && !$ANAMORPHIC)]
		"xpos"						"63"	[$GAMECONSOLE && (!$GAMECONSOLEWIDE || $ANAMORPHIC)]	
		"xpos"						"88"	[!$GAMECONSOLE && $WIN32WIDE]
		"xpos"						"63"	[!$GAMECONSOLE && !$WIN32WIDE]	
		"ypos"						"340"	[$GAMECONSOLE]  
		"ypos"						"318"	[!$GAMECONSOLE]   
		"wide"						"280"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnOptions"
		"navDown"					"BtnQuit"
		"labelText"					"#L4D360UI_MainMenu_Extras"
		"style"						"MainMenuButton"
		"command"					"Extras"
		"ActivationType"			"1"
	}

	"BtnQuit" [!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnQuit"
		"xpos"						"88"	[$WIN32WIDE]
		"xpos"						"63"	[!$WIN32WIDE]	
		"ypos"						"348"
		"wide"						"280"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnExtras"
		"navDown"					"BtnEconUI"
		"labelText"					"#PORTAL2_MainMenu_Quit"
		"style"						"MainMenuButton"
		"command"					"QuitGame"
		"ActivationType"			"1"
	}

	"BtnEconUI" [!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnEconUI"
		"xpos"						"88"	[$WIN32WIDE]
		"xpos"						"63"	[!$WIN32WIDE]	
		"ypos"						"378"
		"wide"						"280"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnQuit"
		"navDown"					"BtnPlaySolo"
		"labelText"					"#PORTAL2_MainMenu_Econ"
		"style"						"BitmapButton"
		"command"					"EconUI"
		"ActivationType"			"1"
		"bitmap_enabled"			"vgui/store/store_button"
		"bitmap_focus"				"vgui/store/store_button_focus_anim"
	}

	"PnlCloudPic"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"PnlCloudPic"
		"xpos"					"310"
		"ypos"					"288"
		"wide"					"50"
		"tall"					"50"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"scaleImage"			"1"
		"image"					"resource/icon_cloud_small"
	}
}
