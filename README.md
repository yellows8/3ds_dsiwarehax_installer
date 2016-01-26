This is a 3DS homebrew application for installing DSiWare savedata exploits, for DSiWare titles which you already have installed. This is intended as a .3dsx which requires access to svcBackdoor.

This uses the following two commands: https://3dbrew.org/wiki/AMPXI:WriteTWLSavedata https://3dbrew.org/wiki/AMPXI:InstallTitlesFinish
The former writes the "public.new" file into TWLNAND fine, however the latter just returns error 0xD8A083FA(https://3dbrew.org/wiki/Application_Manager_Services_PXI#Errors). The actual filename used by DSiWare is "public.sav".

Hence, this installer isn't actually usable currently. If/when this installer ever works correctly at all(ideally without requiring anything additional arm9-side-related), a release-archive containing the app + the required DSiWare exploits would be released.

