This is a 3DS homebrew application for installing DSiWare savedata exploits, for DSiWare titles which you already have installed. This is intended as a .3dsx which requires access to SVCs which are normally not accessible(**arm11kernelhax/"cfw" required**).

This application *itself* does not use any exploits during installation that could be fixed.

When there's any input savefiles on SD where the DSiWare doesn't use those savefiles, AMPXI will be used to write those files to NAND with the \*.new filenames. This involves svcBackdoor. After doing so, a hw-reboot will be enabled when exiting the app. This is intended for DSiWare exploit(s) which load from \*.new filename(s) due to not enough space in the main savedata.

This can be used for general DSiWare saveimage import/export as well.

Before using this, you should have DSiWare export(s) from System Settings Data Management on SD for the target titles(in case you ever need to import the original later).

# Included DSiWare exploits
The release-archive includes the following:
* The exact same exploit saveimages(plaintext) from when the exploits were released for DSi. This includes sudokuhax, even though it's not really useful on 3DS anyway(since the original title-version isn't officially available for 3DS).
* [4swordshax](https://github.com/yellows8/dsi) for "The Legend of Zelda: Four Swords Anniversary Edition", only USA/EUR regions are supported. This will only display a red sub-screen since the payload can't be loaded.

The source for these DSiWare exploits is available [here](https://github.com/yellows8/dsi).

# SD layout
This section can be ignored if you *just* want to install dsiwarehax with the release-archive.

The input size for the \*.sav files must be <=0x100000.

The input data for DSiWare is located under the "dsiware/" directory where the .3dsx runs from. Then under this directory there's directories for each title using the titleID-low, for example: "4B464445". Under each title directory is the actual input data:
* "info": Required, the directory won't be detected on SD otherwise. Contains the display-text for this entry, normally the exploit name. This shouldn't contain any newlines(any text following newlines won't be displayed).
* "public.sav": Required, the actual savedata image.
* "banner.sav" Optional data to use for banner.sav instead of what's used during import, if the DSiWare uses it.
* "private.sav" Optional data to use for private.sav instead of what's used during import, if the DSiWare uses it.
* "public_original.sav" Output file written during installation, contains the original data for public.sav that would have been written to NAND during import.
* "banner_original.sav" Same as above except for banner.sav.
* "private_original.sav" Same as above except for private.sav.
