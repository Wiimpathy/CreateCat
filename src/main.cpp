/****************************************************************************
 * main.cpp
 *
 * Create Category for WiiFlow
 *
 * Wiimpathy 2020
 *
 ***************************************************************************/
#include <iostream>
#include <fstream>
#include <locale>
#include <cstring>
#include <codecvt>
#include "categories.h"
#include "common.h"

// Command line return codes
#define ARGV_OK              0
#define ARGV_MISSING_OPTION  1
#define ARGV_UNKNOWN_OPTION  2

bool mobysearch = false;
bool skiphidden = false;
bool fuzzy = false;
string Fullpath;
string Filename;


static void Usage(const char *exename, int error, const char *arg_error)
{
	ClearScreen();

	fprintf(stderr, "\nCategories %s (Wiimpathy 2020)\n", VERSION_STR);

	if(error == ARGV_MISSING_OPTION)
	{
		fprintf(stderr, "\nERROR!!! Missing option: %s\n", arg_error);
	}

	if(error == ARGV_UNKNOWN_OPTION)
	{
		fprintf(stderr, "\nError!!! Unknown option : %s\n", arg_error);
	}

	fprintf(stderr, "\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s File [-OPTIONS] \n", exename);
	fprintf(stderr, "\n");
	fprintf(stderr, "#File#\n");
	fprintf(stderr, "  The path to the WiiFlow .db file.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "#OPTIONS#\n");
	fprintf(stderr, "  -fuzzy :      Enable fuzzy search. Slower search and probable wrong results.\n");
	fprintf(stderr, "  -mobysearch : Search also in the Mobygames database.\n");
	fprintf(stderr, "  -skiphidden : Skip searching previously hidden games.\n");
	fprintf(stderr, "  -debug :      Show debug information. \n");
	fprintf(stderr, "\n");
#ifdef WIN32
	fprintf(stderr, "Examples:\n");
	fprintf(stderr, "  %s \"e:\\WiiFlow\\cache\\lists\\usb1_534e5854.db\" -nofuzzy \n", exename);
	fprintf(stderr, "  %s \"e:\\WiiFlow\\cache\\lists\\usb1_53454761.db\" -skiphidden -mobysearch -debug \n", exename);
#else
	fprintf(stderr, "Examples:\n");
	fprintf(stderr, "  %s \"/WiiFlow/cache/lists/usb1_534e5854.db\" -nofuzzy \n", exename);
	fprintf(stderr, "  %s \"/WiiFlow/cache/lists/usb1_53454761.db\" -skiphidden -mobysearch -debug \n", exename);
#endif
	fprintf(stderr, "\n");
	exit(0);
}

void HandleArguments(u32 argcount, char *argv[])
{
	int option = 1;

	if(argcount < 2)
		Usage(argv[0], ARGV_MISSING_OPTION, "No input file!");

	while(option < argcount)
	{
		if(option == 1)
		{
			Fullpath = argv[1];
			Filename = GetFilename(argv[1]);
		}
		else
		{
			if (argv[option][0] == '-')
			{
			
				if (strcmp(argv[option], "-debug") == 0)
				{
					debug = true;
				}
				else if (strcmp(argv[option], "-mobysearch") == 0)
				{
					mobysearch = true;
				}
				else if (strcmp(argv[option], "-fuzzy") == 0)
				{
					fuzzy = true;
				}
				else if (strcmp(argv[option], "-skiphidden") == 0)
				{
					skiphidden = true;
				}
				else
				{
					Usage(argv[0], ARGV_UNKNOWN_OPTION, argv[option]);
				}
			}
			else
			{
				Usage(argv[0], ARGV_UNKNOWN_OPTION, argv[option]);
			}
		}
		option++;
	}
}

int main(int argc, char *argv[]) 
{
	// Check command lines options
	HandleArguments(argc, argv);

	Categories Cat;
	bool FoundPlatform = Cat.GetPlatform(Filename);

	if(!FoundPlatform)
		exit(0);

	Cat.SetDefaultCategories();
	Cat.ReadCache(Fullpath, skiphidden);
	Cat.Parse(fuzzy, mobysearch);

	return 0;
}
