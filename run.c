//***************************************************************************
//                          run.c
// Copyright (C) 2020 Dominik "Etua" Danelski <dominik@danelski.pl>
//***************************************************************************
//
// This file is part of Sagger.
//
// Sagger is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
//        the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
// Sagger is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sagger. If not, see <https://www.gnu.org/licenses/>.

#include "run.h"
#include "fpcalc.h"
#include "gtk.h"
#include "json.h"
#include "structures.h"
#include <errno.h>
#include <error.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Program-wide variables. Adjust them to fit your needs.
char program_name[] = "Sagger";
char version_number[] = "0.1";
char json_filename[] = "/tmp/sagger.json";
// TODO Include checks whether API key was changed and/or preferably more
// thoughtful way of storing it
char acoustid_key[] = "YOUR_KEY";

// Creates label both for success and certain types of errors
void create_label(int label_status, char *label_dest) {
  if (label_status >= 0) {
    sprintf(label_dest, "Successfully moved %d files.", label_status);
  } else {
    strcpy(label_dest, "Error: ");
    switch (label_status) {
    case -1:
      strcat(label_dest, "second directory is unavailable.");
      break;
    case -2:
      strcat(label_dest, "first directory is unavailable.");
      break;
    case -3:
      strcat(label_dest, "both source and target directory should be chosen.");
      break;
    default:
      strcat(label_dest, "unknown.");
    }
  }
}

// Creates new directory name and filename to be used later on
void new_filename(run_settings new_filename_settings,
                  recording_metadata *new_filename_metadata, char *second_dir,
                  new_names *set, char *special_name, _Bool retain_filename,
                  char *old_name) {
  char inter[] = " - ";

  // Prepare directory name
  strcpy(set->dirname, second_dir);
  strcat(set->dirname, "/");
  if (strcmp(special_name, "none") != 0) {
    strcat(set->dirname, special_name);
  } else {
    if (new_filename_settings.is_artist[0]) {
      strcat(set->dirname, new_filename_metadata->artist_name);
    } else if (new_filename_settings.is_album[0]) {
      strcat(set->dirname, new_filename_metadata->album_name);
    } else {
      strcat(set->dirname, new_filename_metadata->artist_name);
      strcat(set->dirname, inter);
      strcat(set->dirname, new_filename_metadata->album_name);
    }
  }

  // Prepare filename
  strcpy(set->filename, set->dirname);
  strcat(set->filename, "/");
  if (retain_filename) {
    strcat(set->filename, old_name);
  } else {
    if (new_filename_settings.is_artist[1]) {
      strcat(set->filename, new_filename_metadata->artist_name);
      strcat(set->filename, inter);
    }
    if (new_filename_settings.is_album[1]) {
      strcat(set->filename, new_filename_metadata->album_name);
      strcat(set->filename, inter);
    }
    strcat(set->filename, new_filename_metadata->song_name);
  }
}

// Makes new directories and moves files, returns FALSE for other errors than
// existing directory
_Bool change_name(char *source, new_names *to_change) {
  if (mkdir(to_change->dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1 &&
      errno != 17)
    return FALSE;
  if (access(to_change->filename, F_OK) != -1)
    return FALSE;
  rename(source, to_change->filename);
  return TRUE;
}

// Main function invoking other functions and coordinating data flow. It is not
// GUI-specific
void master_function(run_settings run_final, _Bool graphical) {
  fpcalc_data *master_fpcalc_data = malloc(sizeof(fpcalc_data));
  char master_curl_request[20000];
  recording_metadata *master_recording_metadata =
      malloc(sizeof(recording_metadata));
  new_names *master_names = malloc(sizeof(new_names));
  int file_number = 0;

  // Setup for FTS which will traverse source directory
  int fts_options = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR;
  char *const fts_dir_first[] = {run_final.f_dir_1, NULL};
  FTS *ftsp;
  FTSENT *file_p;

  // Checks permissions for directories and invokes FTS
  int existence_test = R_OK | W_OK;
  // Target directory is checked for permissions only once
  if (access(run_final.f_dir_2, existence_test) != 0)
    file_number = -1;
  if (file_number == 0)
    if (access(run_final.f_dir_1, existence_test) != 0)
      file_number = -2;
    else if ((ftsp = fts_open(fts_dir_first, fts_options, NULL)) == NULL)
      file_number = -2;

  if (file_number == 0) {
    while ((file_p = fts_read(ftsp)) != NULL) {
      // Reads next file from a loop
      if (file_p->fts_info == FTS_F) {
        // Checks permissions for individual file from source directory
        if (access(file_p->fts_path, existence_test) == 0) {
          // Progress in CLI is indicated by printing each file path. Currently
          // it is unconditional.
          if (!graphical)
            printf("Current file: %s\n", file_p->fts_path);

          //-1 means that fpcalc returned an error, usually because the source
          //file did not contain music.
          fpcalc(master_fpcalc_data, file_p->fts_path);
          if (master_fpcalc_data->length == -1) {
            if (!graphical)
              printf("Unsupported file format.\n");
            continue;
          }

          // Prepare request, download data to the temporary file and retrieve
          // metadata
          request_constructor(master_fpcalc_data->length,
                              master_fpcalc_data->fingerprint, acoustid_key,
                              master_curl_request);
          if (curl_download_data(json_filename, master_curl_request) != 0) {
            if (!graphical)
              printf("Network error.\n");
            continue;
          }
          glib_parse(master_recording_metadata, json_filename);

          // Fallback for corrupted or missing data
          if (master_recording_metadata->song_name[0] == '^')
            new_filename(run_final, master_recording_metadata,
                         run_final.f_dir_2, master_names, "[unknown]", TRUE,
                         file_p->fts_name);
          else
            new_filename(run_final, master_recording_metadata,
                         run_final.f_dir_2, master_names, "none", FALSE,
                         file_p->fts_name);

          // Manage duplicates
          char duplicate_name[20];
          // This loop will work until directory with no file of the same name
          // is found.
          // TODO Because of current logic duplicate files which otherwise
          // should have ended in "[unknown]" can be placed in "[duplicate]"
          for (int i = 1; !change_name(file_p->fts_path, master_names); i++) {
            sprintf(duplicate_name, "[duplicate%d]", i);
            new_filename(run_final, master_recording_metadata,
                         run_final.f_dir_2, master_names, duplicate_name, FALSE,
                         file_p->fts_name);
          }
          if (!graphical)
            printf("New location: %s\n", master_names->filename);
          // Indicates number of moved files or error code not allowing the loop
          // to start
          file_number++;
        }
      }
    }
    fts_close(ftsp);
  }

  // Free data structures
  free(master_fpcalc_data);
  free(master_recording_metadata);
  free(master_names);

  // Prepare final message
  char master_label[300];
  create_label(file_number, master_label);
  if (graphical)
    open_second_window(run_final.first_window, master_label);
  else if (file_number < 0)
    error(1, 0, "%s\n", master_label);
  else
    printf("%s\n", master_label);
}