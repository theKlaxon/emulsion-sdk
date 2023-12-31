"Resource/UI/puzzlemakermychambers.res"
{
	"PuzzleMakerMyChambers"
	{
		"ControlName"		"Frame"
		"fieldName"			"PuzzleMakerMyChambers"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"10"
		"tall"				"5"
		"autoResize"		"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"PaintBackgroundType"	"0"
		"dialogstyle"		"1"
// 		"chamber_list_x"	"0"
// 		"chamber_list_y"	"50"
// 		"chamber_list_w"	"250"
// 		"chamber_list_h"	"200"
	}

	"ListBtnChamberSort"
	{
		"ControlName"		"CDialogListButton"
		"fieldName"			"ListBtnChamberSort"
		"xpos"				"0"
		"ypos"				"10"
		"zpos"				"1"
		"wide"				"250"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"font"				"NewGameChapterName"
		//"navUp"				"SldBrightness"
		//"navDown"			"DrpResolution"
		//"labelText"			"#GameUI_AspectRatio"
		"style"				"AltButton"
		"list"
		{
			"#PORTAL2_EditorMenu_ByStatus"		"EditorMenu_ByStatus"
			"#PORTAL2_EditorMenu_ByDate"		"EditorMenu_ByDate"
		}
	}

	"ChamberList"
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"ChamberList"
		"xpos"							"0"
		"ypos"							"50"
		"zpos"							"1"
		"wide"							"250"
		"tall"							"200"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"NoWrap"						"1"
		"panelBorder"					"0"
	}

	"ImgChamberThumb"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ImgChamberThumb"
		"xpos"							"258"
		"ypos"							"-18"
		"zpos"							"999"
		"wide"							"230"
		"tall"							"200"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							""
	}

	"LblChamberName"
	{
		"ControlName"			"Label"
		"fieldName"				"LblChamberName"
		"xpos"					"258"
		"ypos"					"150"
		"wide"					"240"
		"tall"					"25"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListSmall"
		"labelText"				""
		"textAlignment"			"west"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}

	"LblChamberStatRating"
	{
		"ControlName"			"Label"
		"fieldName"				"LblChamberStatRating"
		"xpos"					"280"
		"ypos"					"180"
		"wide"					"80"
		"tall"					"15"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListStatusLine"
		"labelText"				"#PORTAL2_EditorMenu_Rating"
		"textAlignment"			"east"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}
	
	"LblChamberStatCreated"
	{
		"ControlName"			"Label"
		"fieldName"				"LblChamberStatCreated"
		"xpos"					"280"
		"ypos"					"195"
		"wide"					"80"
		"tall"					"15"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListStatusLine"
		"labelText"				"#PORTAL2_EditorMenu_Created"
		"textAlignment"			"east"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}
	
	"LblChamberStatCreatedData"
	{
		"ControlName"			"Label"
		"fieldName"				"LblChamberStatCreatedData"
		"xpos"					"370"
		"ypos"					"195"
		"wide"					"100"
		"tall"					"15"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListStatusLine"
		"labelText"				""
		"textAlignment"			"west"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}

	"LblChamberStatLastModified"
	{
		"ControlName"			"Label"
		"fieldName"				"LblChamberStatLastModified"
		"xpos"					"280"
		"ypos"					"210"
		"wide"					"80"
		"tall"					"15"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListStatusLine"
		"labelText"				"#PORTAL2_EditorMenu_LastModified"
		"textAlignment"			"east"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}
	
	"LblChamberStatLastModifiedData"
	{
		"ControlName"			"Label"
		"fieldName"				"LblChamberStatLastModifiedData"
		"xpos"					"370"
		"ypos"					"210"
		"wide"					"100"
		"tall"					"15"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListStatusLine"
		"labelText"				""
		"textAlignment"			"west"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}

	"LblChamberStatLastPublished"
	{
		"ControlName"			"Label"
		"fieldName"				"LblChamberStatLastPublished"
		"xpos"					"280"
		"ypos"					"225"
		"wide"					"80"
		"tall"					"15"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListStatusLine"
		"labelText"				"#PORTAL2_EditorMenu_LastPublished"
		"textAlignment"			"east"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}
	
	"LblChamberStatLastPublishedData"
	{
		"ControlName"			"Label"
		"fieldName"				"LblChamberStatLastPublishedData"
		"xpos"					"370"
		"ypos"					"225"
		"wide"					"100"
		"tall"					"15"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListStatusLine"
		"labelText"				""
		"textAlignment"			"west"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}

	"RatingsItem"
	{
		"ControlName"				"IconRatingItem"
		"fieldName"					"RatingsItem"
		"xpos"							"370"
		"ypos"							"172"
		"wide"							"60"
		"tall"							"32"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"				"0"
		"on_image"					"rating_on"
		"off_image"					"rating_off"
		"half_image"				"rating_half"
	}	

	"InfoIcon"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"InfoIcon"
		"xpos"							"9"
		"ypos"							"59"
		"zpos"							"999"
		"wide"							"32"
		"tall"							"32"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"hud/icon_info"
	}

	"LblAutoSaveFound"
	{
		"ControlName"			"Label"
		"fieldName"				"LblAutoSaveFound"
		"xpos"					"52"
		"ypos"					"55"
		"wide"					"197"
		"tall"					"150"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"textinsetx"			"0"
		"Font"					"FriendsListStatusLine"
		"labelText"				"#PORTAL2_PuzzleMaker_AutoSaveFound"
		"textAlignment"			"northwest"
		"Wrap"						"1"
		"fgcolor_override"		"201 211 207 255"
		"bgcolor_override"		"0 0 0 0"
	}
	
	"PuzzleListSpinner"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"PuzzleListSpinner"
		"xpos"							"75"	
		"ypos"							"75"
		"zpos"							"999"
		"wide"							"96"
		"tall"							"96"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"				"0"
		"scaleImage"				"1"
		"image"							"spinner"
	}
	
	"ThumbnailSpinner"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ThumbnailSpinner"
		"xpos"							"325"
		"ypos"							"38"
		"zpos"							"999"
		"wide"							"96"
		"tall"							"96"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"				"0"
		"scaleImage"				"1"
		"image"							"spinner"
	}
}
