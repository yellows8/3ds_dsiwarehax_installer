#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <3ds.h>

static int menu_curprintscreen = 0;
static PrintConsole menu_printscreen[2];

typedef struct {
	int initialized;
	u64 titleid;
	char dirpath[64];
	char desc[256];
} dsiware_entry;

#define MAX_DSIWARE 16
u32 dsiware_total;
dsiware_entry dsiware_list[MAX_DSIWARE];

char *dsiware_menuentries[MAX_DSIWARE];

void initsrv_allservices();

void display_menu(char **menu_entries, int total_entries, int *menuindex, char *headerstr)
{
	int i;
	u32 redraw = 1;
	u32 kDown = 0;

	while(1)
	{
		gspWaitForVBlank();
		hidScanInput();

		kDown = hidKeysDown();

		if(redraw)
		{
			redraw = 0;

			consoleClear();
			printf("%s.\n\n", headerstr);

			for(i=0; i<total_entries; i++)
			{
				if(*menuindex==i)
				{
					printf("-> ");
				}
				else
				{
					printf("   ");
				}

				printf("%s\n", menu_entries[i]);
			}
		}

		if(kDown & KEY_B)
		{
			*menuindex = -1;
			return;
		}

		if(kDown & (KEY_DDOWN | KEY_CPAD_DOWN))
		{
			(*menuindex)++;
			if(*menuindex>=total_entries)*menuindex = 0;
			redraw = 1;

			continue;
		}
		else if(kDown & (KEY_DUP | KEY_CPAD_UP))
		{
			(*menuindex)--;
			if(*menuindex<0)*menuindex = total_entries-1;
			redraw = 1;

			continue;
		}

		if(kDown & KEY_Y)
		{
			gspWaitForVBlank();
			consoleClear();

			menu_curprintscreen = !menu_curprintscreen;
			consoleSelect(&menu_printscreen[menu_curprintscreen]);
			redraw = 1;
		}

		if(kDown & KEY_A)
		{
			break;
		}
	}
}

void displaymessage_waitbutton()
{
	printf("\nPress the A button to continue.\n");
	while(1)
	{
		gspWaitForVBlank();
		hidScanInput();
		if(hidKeysDown() & KEY_A)break;
	}
}

Result load_file(char *path, u8 *buffer, u32 size, u32 *readsize)
{
	FILE *f;
	struct stat filestats;
	u32 tmp;

	if(stat(path, &filestats)==-1)return -7;

	if(size > filestats.st_size)size = filestats.st_size;
	if(size == 0)return -6;

	f = fopen(path, "rb");
	if(f==NULL)return -5;

	tmp = fread(buffer, 1, size, f);
	if(readsize)*readsize = tmp;
	fclose(f);

	if(tmp == 0)return -6;

	return 0;
}

Result loadnand_dsiware_titlelist()
{
	Result ret=0;
	u32 titlecount=0;
	u64 *tidbuf;
	u32 pos, i;
	u32 entcount = 0;
	dsiware_entry *ent = &dsiware_list[0];
	char str[256];

	ret = AM_GetTitleCount(MEDIATYPE_NAND, &titlecount);
	if(ret)
	{
		printf("AM_GetTitleCount failed: 0x%08x.\n", (unsigned int)ret);
		return ret;
	}

	if((titlecount & 0xE0000000) || titlecount==0)
	{
		printf("Invalid titlecount: 0x%08x.\n", (unsigned int)titlecount);
		return -1;
	}

	tidbuf = malloc(titlecount << 3);
	if(tidbuf==NULL)
	{
		printf("Failed to allocate tidbuf.\n");
		return -2;
	}

	ret = AM_GetTitleIdList(MEDIATYPE_NAND, titlecount, tidbuf);
	if(ret)
	{
		free(tidbuf);
		printf("AM_GetTitleIdList failed: 0x%08x.\n", (unsigned int)ret);
		return ret;
	}

	for(pos=0; pos<titlecount; pos++)
	{
		if((tidbuf[pos] >> 32) == 0x00048004)
		{
			memset(ent, 0, sizeof(dsiware_entry));

			memset(str, 0, sizeof(str));

			ent->titleid = tidbuf[pos];

			snprintf(ent->dirpath, sizeof(ent->dirpath)-1, "dsiware/%08X", (unsigned int)(ent->titleid & 0xffffffff));
			snprintf(str, sizeof(str)-1, "%s/info", ent->dirpath);

			ret = load_file(str, (u8*)ent->desc, sizeof(ent->desc)-1, NULL);
			if(ret==0)
			{
				ent->initialized = 1;

				i = strlen(ent->desc);
				if(i)
				{
					while(i>0)
					{
						i--;
						if(ent->desc[i] == 0x0a || ent->desc[i] == 0x0d)
						{
							ent->desc[i] = 0;
						}
						else
						{
							break;
						}
					}
				}

				snprintf(dsiware_menuentries[entcount], 63, "%08X %s", (unsigned int)(ent->titleid & 0xffffffff), ent->desc);
				printf("%u: %s\n", (unsigned int)entcount, dsiware_menuentries[entcount]);

				entcount++;
				if(entcount>=MAX_DSIWARE)break;
				ent = &dsiware_list[entcount];
			}
		}
	}

	dsiware_total = entcount;

	free(tidbuf);

	return 0;
}

Result loadsave_dsiwarehax(dsiware_entry *ent, u8 *savebuf, u32 savebuf_maxsize, u32 *actual_savesize)
{
	char str[256];

	memset(str, 0, sizeof(str));

	snprintf(str, sizeof(str)-1, "%s/public.sav", ent->dirpath);

	return load_file(str, savebuf, savebuf_maxsize, actual_savesize);
}

Result terminatelaunch_am(u32 type)
{
	Result ret=0;
	u64 titleid = 0x0004013000001502ULL;

	ret = nsInit();
	if(ret)return ret;

	if(type==0)ret = NS_TerminateProcessTID(titleid);
	if(type==1)ret = NS_LaunchTitle(titleid, 0, NULL);

	nsExit();

	return ret;
}

Result install_dsiwarehax(dsiware_entry *ent, u8 *savebuf, u32 savesize)
{
	Result ret=0;

	ret = ampxiInit(0);
	if(ret<0)
	{
		if(ret==0xd8e06406)
		{
			printf("The AMPXI service is inaccessible. Attempting to get access via kernelmode, this will hang the system if svcBackdoor isn't accessible.\n");
			initsrv_allservices();
			printf("Attempting to init the AMPXI service-handle again...\n");
			ret = ampxiInit(0);
			if(ret==0)printf("AMPXI init was successful.\n");
		}
		if(ret<0)return ret;
	}

	ret = ampxiWriteTWLSavedata(ent->titleid, savebuf, savesize, 0, 5, 5);
	if(ret<0)printf("ampxiWriteTWLSavedata failed: 0x%08x.\n", (unsigned int)ret);

	if(ret==0)
	{
		ret = ampxiInstallTitlesFinish(MEDIATYPE_NAND, 0, 1, &ent->titleid);
		if(ret<0)printf("ampxiInstallTitlesFinish failed: 0x%08x.\n", (unsigned int)ret);
	}

	ampxiExit();

	return ret;
}

int main(int argc, char **argv)
{
	Result ret = 0;
	int menuindex = 0;
	int reboot_required = 0;
	u32 pos;

	u8 *savebuf;
	u32 savebuf_maxsize = 0x100000, savesize;

	char headerstr[512];

	// Initialize services
	gfxInitDefault();

	menu_curprintscreen = 0;
	consoleInit(GFX_TOP, &menu_printscreen[0]);
	consoleInit(GFX_BOTTOM, &menu_printscreen[1]);
	consoleSelect(&menu_printscreen[menu_curprintscreen]);

	memset(dsiware_list, 0, sizeof(dsiware_list));
	dsiware_total = 0;

	printf("3ds_dsiwarehax_installer %s by yellows8.\n", VERSION);

	if(ret==0)
	{
		ret = amInit();
		if(ret!=0)
		{
			printf("Failed to initialize AM: 0x%08x.\n", (unsigned int)ret);
			if(ret==0xd8e06406)
			{
				printf("The AM service is inaccessible. With the *hax payloads this should never happen. This is normal with plain ninjhax v1.x: this app isn't usable from ninjhax v1.x without any further hax.\n");
			}
		}

		if(ret==0)
		{
			for(pos=0; pos<MAX_DSIWARE; pos++)
			{
				dsiware_menuentries[pos] = malloc(256);
				if(dsiware_menuentries[pos]==NULL)
				{
					ret = -2;
					printf("Failed to allocate memory for a menuentry.\n");
					break;
				}
				memset(dsiware_menuentries[pos], 0, 256);
			}

			ret = loadnand_dsiware_titlelist();

			amExit();

			if(ret)printf("Failed to load the DSiWare titlelist: 0x%08x.\n", (unsigned int)ret);
		}

		if(ret==0 && dsiware_total==0)
		{
			ret = -10;
			printf("No DSiWare titles were detected in NAND with a matching exploit directory on SD.\n");
		}
	}

	if(ret>=0)
	{
		memset(headerstr, 0, sizeof(headerstr));
		snprintf(headerstr, sizeof(headerstr)-1, "3ds_dsiwarehax_installer %s by yellows8.\n\nSelect a DSiWare exploit to install with the below menu(the hex word is the detected DSiWare titleID-low on your system). You can press the B button to exit, this will do a hardware-reboot if needed due to exploit installation. You can press the Y button at any time while at a menu like the below one, to toggle the screen being used by this app", VERSION);

		while(ret==0)
		{
			display_menu(dsiware_menuentries, dsiware_total, &menuindex, headerstr);

			if(menuindex==-1)break;

			consoleClear();

			savesize = 0;

			savebuf = malloc(savebuf_maxsize);
			if(savebuf==NULL)
			{
				printf("Failed to allocate memory for the savedata.\n");
				ret = -2;
			}
			memset(savebuf, 0, savebuf_maxsize);

			if(ret==0)
			{
				printf("Loading the savedata from SD...\n");
				ret = loadsave_dsiwarehax(&dsiware_list[menuindex], savebuf, savebuf_maxsize, &savesize);
				if(ret)
				{
					printf("Failed to load the dsiwarehax savedata: 0x%08x.\n", (unsigned int)ret);
				}
			}

			if(ret==0)
			{
				printf("Terminating the AM sysmodule...\n");
				ret = terminatelaunch_am(0);
				if(ret==0)
				{
					reboot_required = 1;
					svcSleepThread(1000000000ULL);//Sleep 1 second.
				}
			}

			if(ret==0)
			{
				printf("Installing the savedata...\n");
				ret = install_dsiwarehax(&dsiware_list[menuindex], savebuf, savesize);
				if(ret)printf("Failed to install the savedata: 0x%08x.\n", (unsigned int)ret);
			}

			if(savebuf)free(savebuf);

			printf("Launching the AM sysmodule...\n");
			terminatelaunch_am(1);

			if(ret==0)
			{
				printf("The exploit was successfully installed.\n");
				displaymessage_waitbutton();
			}
			else
			{
				printf("Exploit installation failed: 0x%08x.\n", (unsigned int)ret);
			}
		}
	}

	if(ret!=0)printf("An error occured. You can report this to here if it persists(or comment on an already existing issue if needed), with a screenshot: https://github.com/yellows8/3ds_dsiwarehax_installer/issues\n");

	for(pos=0; pos<MAX_DSIWARE; pos++)
	{
		if(dsiware_menuentries[pos])free(dsiware_menuentries[pos]);
	}

	printf("Press the START button to exit.\n");
	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}

	if(reboot_required)
	{
		consoleClear();
		gfxExit();
		aptOpenSession();//Do a hardware reboot.
		APT_HardwareResetAsync();
		aptCloseSession();
		return 0;
	}

	// Exit services
	gfxExit();
	return 0;
}

