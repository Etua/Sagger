//***************************************************************************
//                          fpcalc.c
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

#include "fpcalc.h"
#include "structures.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Uses fpcalc utility to calculate Chromaprint ID
void fpcalc(fpcalc_data *fpc_result, char *fpc_filename) {
  char fpc_command_start[263] = "fpcalc ", fpc_command_finish[] = " 2>&1",
       filter_sign[] = "=";
  char char_int[20], chromaprint[10000], err[] = "E";
  char *p_char_int, *p_chromaprint;

  strcat(fpc_command_start, "'");
  strcat(fpc_command_start, fpc_filename);
  strcat(fpc_command_start, "'");
  strcat(fpc_command_start, fpc_command_finish);

  FILE *fpc_return;
  fpc_return = popen(fpc_command_start, "r");
  if (getc(fpc_return) == err[0]) {
    fpc_result->length = -1;
    pclose(fpc_return);
    return;
  }

  fscanf(fpc_return, "%s %s", char_int, chromaprint);
  p_char_int = strstr(char_int, filter_sign);
  p_char_int += sizeof(char);
  p_chromaprint = strstr(chromaprint, filter_sign);
  p_chromaprint += sizeof(char);
  fpc_result->length = atoi(p_char_int);
  strcpy(fpc_result->fingerprint, p_chromaprint);

  pclose(fpc_return);
}

// Prepeares AcoustID API request to be used by curl_download_data
void request_constructor(int curl_lenght, char *curl_id, char *curl_api,
                         char *curl_request) {
  sprintf(curl_request,
          "https://api.acoustid.org:443/v2/lookup?client=%s"
          "&meta=recordings+releasegroups&duration=%d&fingerprint=%s",
          curl_api, curl_lenght, curl_id);
}

// Writes data downloaded by curl_download_data to the file
static size_t curl_write_data(void *ptr, size_t size, size_t nmemb,
                              void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

// Download data based on the request that was prepared beforehand
int curl_download_data(char *curl_filename, char *curl_request) {
  // Delay the request by third of a second (terms of AcoustID)
  // TODO Currently it is assumed that all other functions perform in no time.
  // If replaced by timer measuring period between requests, it would make the
  // program perform faster by a fraction of second for each checked file
  struct timespec wait;
  wait.tv_sec = 0;
  wait.tv_nsec = 333300000;
  nanosleep(&wait, &wait);

  CURL *curl_handle;
  CURLcode curl_error;
  FILE *curl_file;

  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, curl_request);
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_data);
  curl_file = fopen(curl_filename, "wb");
  if (curl_file) {
    curl_error = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, curl_file);
    curl_error = curl_easy_perform(curl_handle);
    fclose(curl_file);
  }
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();
  return curl_error;
}