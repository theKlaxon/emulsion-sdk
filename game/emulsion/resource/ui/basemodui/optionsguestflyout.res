"Resource/UI/OptionsGuestFlyout.res"
{
	"PnlBackground"
	{
		"ControlName"		"Panel"
		"fieldName"			"PnlBackground"
		"xpos"				"0"
		"ypos"				"0"
		"zpos"				"-1"
		"wide"				"154"
		"tall"				"45"
		"visible"			"1"
		"enabled"			"1"
		"paintbackground"	"1"
		"paintborder"		"1"
	}

	"BtnAudioVideo"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnAudioVideo"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"150"
		"tall"					"20"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnController"
		"navDown"				"BtnController"
		"tooltiptext"			"#L4D360UI_Options_AudioVideo"
		"disabled_tooltiptext"	"#L4D360UI_Options_AudioVideo_Disabled"
		"labelText"				"#L4D360UI_AudioVideo"
		"style"					"FlyoutMenuButton"
		"command"				"AudioVideo"
		"usablePlayerIndex"		"0"
	}

	"BtnController"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnController"
		"xpos"					"0"
		"ypos"					"20"
		"wide"					"150"
		"tall"					"20"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnAudioVideo"
		"navDown"				"BtnAudioVideo"
		"tooltiptext"			"#L4D360UI_Options_Controller"
		"disabled_tooltiptext"	"#L4D360UI_Options_Controller_Disabled"
		"labelText"				"#L4D360UI_Controller"
		"style"					"FlyoutMenuButton"
		"command"				"Controller"
		"usablePlayerIndex"		"0"
	}
}