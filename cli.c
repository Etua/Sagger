//***************************************************************************
//                          cli.c
//Copyright (C) 2020 Dominik "Etua" Danelski <dominik@danelski.pl>
//***************************************************************************
//
//This file is part of Sagger.
//
//Sagger is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//        the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Sagger is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Sagger. If not, see <https://www.gnu.org/licenses/>.

#include "structures.h"
#include "run.h"
#include <stdio.h>
#include <string.h>
#include <error.h>

//Performs basic checks on command and passes it to master_function
int parse_command(int argc, char *argv[]) {
    if(argc == 2) {
        if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printf("Usage: sagger [OPTIONS] SOURCE_DIRECTORY TARGET_DIRECTORY\n\n"
                   "Tag audio files using AcoustID fingerprints and MusicBrainz database.\n\n"
                   "Options:\n  -album/-artist/-both choose what to include in directory name and filename. "
                   "Two arguments are mandatory, song name is included in filename each time.\n"
                   "  -h/--help print this help\n"
                   "  -v/--version print version number and license\n\n"
                   "If no options are specified, the program is set to run in GUI mode.\n");
        }
        else if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
            printf("%s %s\nCopyright (C) 2020 Dominik Danelski\n\n"
                   "This program comes with ABSOLUTELY NO WARRANTY;\n"
                   "This is free software, and you are welcome to redistribute it under certain conditions.\n"
                   "More information in the COPYING file and license notices in the source files.\n", program_name, version_number);
        }
        else
            error(1, 22, NULL);
    } else if(argc > 2 && argc < 5)
        error(1, 22, "Wrong number of arguments");
    else if(argc == 5) {
        run_settings cli_settings;
        if(strcmp(argv[1], "-album") == 0) {
            cli_settings.is_album[0] = TRUE;
            cli_settings.is_artist[0] = FALSE;
        } else if(strcmp(argv[1], "-artist") == 0) {
            cli_settings.is_album[0] = FALSE;
            cli_settings.is_artist[0] = TRUE;
        } else if(strcmp(argv[1], "-both") == 0) {
            cli_settings.is_album[0] = FALSE;
            cli_settings.is_artist[0] = FALSE;
        } else
            error(1, 22, "Number 2");

        if(strcmp(argv[2], "-album") == 0) {
            cli_settings.is_album[1] = TRUE;
            cli_settings.is_artist[1] = FALSE;
        } else if(strcmp(argv[2], "-artist") == 0) {
            cli_settings.is_album[1] = FALSE;
            cli_settings.is_artist[1] = TRUE;
        } else if(strcmp(argv[2], "-both") == 0) {
            cli_settings.is_album[1] = TRUE;
            cli_settings.is_artist[1] = TRUE;
        } else
            error(1, 22, "Number 3");

        strcpy(cli_settings.f_dir_1, argv[3]);
        strcpy(cli_settings.f_dir_2, argv[4]);
        master_function(cli_settings, FALSE);
        return 0;
    } else
        error(1, 7, NULL);
    return 1;
}