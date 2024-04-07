# Emulsion
Emulsion is a paint focused puzzle game built on the Portal 2 Source Engine branch.

# Setup
Before settng up the code project, I recommend that you install Visual Studio 2010 from the link provided. 
You do not have to use the actual vs 2010 IDE, you just need it installed so that you can use the v100 platform tools for emulsion's compilation.
HOWEVER, The v120_xp toolset is still used for gamepadui due to compatibility issues with the v100 toolset. 
Once you have Visual Studio 2010 installed, you will be able to use the v100 toolset with your MSVC IDE of choice (i.e. vs 2013, 2019, 2022, etc.)

Link to archived Visual Studio 2010. I recommend downloading the ISO image, mounting it, and running the installer inside.
https://archive.org/details/en_vs_2010_ult

To setup emulsion, you must first create a gameinfo.txt. I did things this way to prevent shipping Portal 2's assets.
to setup the gameinfo.txt, go into game/emulsion, and locate "gameinfo.EDIT_ME_WITH_TXT_EDITOR". Follow the instructions inside this fie,
and save it as a "gameinfo.txt" in the same directory you found it. This will also need to be done for the skeleton template (game/skeleton), if it is used.

Have Fun!