"Resource/UI/InGameMainMenu.res"
{
	"InGameMainMenu"
	{
		"ControlName"			"Frame"
		"fieldName"				"InGameMainMenu"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"f0"
		"tall"					"f0"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"PaintBackgroundType"	"0"
	}
	
	"PnlBackground"
	{
		"ControlName"		"Panel"
		"fieldName"			"PnlBackground"
		"xpos"				"0"
		"ypos"				"105"
		"zpos"				"-1"
		"wide"				"f0"
		"tall"				"260"
		"visible"			"1"			[$WIN32]
		"visible"			"0"			[$X360]
		"enabled"			"1"
		"fillColor"			"0 0 0 255"
	}
			
	"BtnReturnToGame"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnReturnToGame"
		"xpos"					"100"
		"ypos"					"230"
		"wide"					"220"	[$WIN32]
		"wide"					"180"	[$X360]
		"tall"					"20"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"1"
		"navUp"					"BtnExitToMainMenu"
		"navDown"				"BtnGoIdle"
		"labelText"				"#L4D360UI_InGameMainMenu_ReturnToGame"
		"tooltiptext"			"#L4D360UI_InGameMainMenu_ReturnToGame_Tip"
		"style"					"MainMenuButton"
		"command"				"ReturnToGame"
		"ActivationType"		"1"
	}
	
	"BtnOptions"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnOptions"
		"xpos"					"100"
		"ypos"					"260"
		"wide"					"220"	[$WIN32]
		"wide"					"180"	[$X360]
		"tall"					"20"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnStatsAndAchievements"
		"navDown"				"BtnExitToMainMenu"
		"labelText"				"#L4D360UI_MainMenu_Options"
		"tooltiptext"			"#L4D360UI_MainMenu_Options_Tip"
		"style"					"MainMenuButton"
		"command"				"FlmOptionsFlyout"
		"ActivationType"		"1"
	}

	"BtnExitToMainMenu"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnExitToMainMenu"
		"xpos"					"100"
		"ypos"					"290"
		"wide"					"220"	[$WIN32]
		"wide"					"180"	[$X360]
		"tall"					"20"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnOptions"
		"navDown"				"BtnReturnToGame"
		"labelText"				"#L4D360UI_InGameMainMenu_ExitToMainMenu"
		"tooltiptext"			"#L4D360UI_InGameMainMenu_ExitToMainMenu_Tip"
		"style"					"MainMenuButton"
		"command"				"ExitToMainMenu"
		"ActivationType"		"1"
	}

	"FlmOptionsFlyout"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmOptionsFlyout"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnVideo" [$WIN32]
		"InitialFocus"			"BtnAudioVideo" [$X360]
		"ResourceFile"			"resource/UI/basemodui/OptionsFlyout.res"		[$WIN32]
		"ResourceFile"			"resource/UI/basemodui/OptionsFlyoutIngame.res"	[$X360]
	}
	
	"FlmOptionsGuestFlyout"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmOptionsGuestFlyout"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnAudioVideo"
		"ResourceFile"			"resource/UI/basemodui/OptionsGuestFlyout.res"
	}

}
