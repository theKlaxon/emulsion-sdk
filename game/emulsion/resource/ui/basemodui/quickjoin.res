"Resource/UI/QuickJoin.res"
{
	"LblTitle"
	{
		"ControlName"			"Label"					[$GAMECONSOLE]
		"ControlName"			"BaseModHybridButton"	[$WIN32]
		"fieldName"				"LblTitle"

		"xpos"					"0"		[$GAMECONSOLE]
		"ypos"					"0"		[$GAMECONSOLE]
		"wide"					"210"	[$GAMECONSOLE]
		"tall"					"40"	[$GAMECONSOLE]

		"xpos"					"21"	[$WIN32]
		"ypos"					"0"		[$WIN32]
		"wide"					"210"	[$WIN32]
		"tall"					"20"	[$WIN32]

		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"wrap"					"1"
		"labelText"				""
		"tooltiptext"			"#L4D360UI_MainMenu_PlayCoopWithFriends_Tip"
		"Font"					"DefaultMedium"
		"fgcolor_override"		"255 255 255 255"
		
		"style"						"MainMenuSmallButton"[$WIN32]
		"command"					"SeeAll"		[$WIN32]
		"ActivationType"			"1"				[$WIN32]
		"FocusDisabledBorderSize"	"1"				[$WIN32]
	}
	
	"GplQuickJoinList"
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"GplQuickJoinList"
		"xpos"							"-1"
		"ypos"							"20"
		"zpos"							"0"
		"wide"							"210"
		"tall"							"300"
		"autoResize"					"1"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"1"
		"panelBorder"					"0"
		"NoDrawPanel"					"1"
		"arrowsVisible"					"0"
	}
}