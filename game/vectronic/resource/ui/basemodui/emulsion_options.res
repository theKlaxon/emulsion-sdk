"Resource/UI/options.res"
{
	"Options"
	{
		"ControlName"		"Frame"
		"fieldName"			"Options"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"4"		[$GAMECONSOLE]
		"wide"				"5"		[!$GAMECONSOLE]
		"tall"				"2"		[$GAMECONSOLE]
		"tall"				"3"		[!$GAMECONSOLE]
		"autoResize"		"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
	}
	
	"AudioVideo"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnAudioVideo"
		"xpos"						"0"
		"ypos"						"25"
		"wide"						"0"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnController"
		"navDown"					"BtnController"
		"labelText"					"#L4D360UI_AudioVideo"
		"style"						"DefaultButton"
		"command"					"AudioVideo"
		"ActivationType"			"1"
	}

	"BtnKeyboardMouse"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnKeyboardMouse"
		"xpos"						"0"
		"ypos"						"75"
		"wide"						"0"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnVideo"
		"navDown"					"BtnController"
		"labelText"					"#L4D360UI_KeyboardMouse"
		"style"						"DefaultButton"
		"command"					"KeyboardMouse"
		"ActivationType"			"1"
	}
}