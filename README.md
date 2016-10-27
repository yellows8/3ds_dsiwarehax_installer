This is a 3DS homebrew application for installing DSiWare savedata exploits, for DSiWare titles which you already have installed. This is intended as a .3dsx which requires access to SVCs which are normally not accessible(**arm11kernelhax/whatever required**).

Eventually a release-archive containing the app + the required DSiWare exploits should be released.

# SD layout
The input \*.sav size must be <=0x20000. Due to this the DSiWare grtpwn exploit can't installed currently.

The rest of this section isn't really needed if you *just* want to install dsiwarehax with the release-archive.

The input data for DSiWare is located under the "dsiware/" directory where the .3dsx runs from. Then under this directory there's directories for each title using the titleID-low, for example: "4B464445". Under each title directory is the actual input data:
* "info": Required, the directory won't be detected on SD otherwise. Contains the display-text for this entry, normally the exploit name. This shouldn't contain any newlines(any text following newlines won't be displayed).
* "public.sav": Required, the actual savedata image.
* "banner.sav" Optional data to use for banner.sav instead of what's used during import, if the DSiWare uses it.
* "private.sav" Optional data to use for private.sav instead of what's used during import, if the DSiWare uses it.
* "public_original.sav" Output file written during installation, contains the original data for public.sav that would have been written to NAND during import.
* "banner_original.sav" Same as above except for banner.sav.
* "private_original.sav" Same as above except for private.sav.
