//***************************************************************************
//                          structures.h
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

#include <gtk/gtk.h>

#ifndef SAGGER_STRUCTURES_H
#define SAGGER_STRUCTURES_H

//TODO Define length of char[] used across project for clarity and faster debug
//Some prettier way of handling data may exist but implementing them would require overhaul of code

typedef struct {
    char f_dir_1[256];
    char f_dir_2[256];
    _Bool is_artist[2];
    _Bool is_album[2];
    GtkWidget *first_window;
} run_settings;

typedef struct {
    char song_name[256];
    char album_name[256];
    char artist_name[256];
} recording_metadata;

typedef struct {
    int length;
    char fingerprint[20000];
} fpcalc_data;

typedef struct {
    char filename[256];
    char dirname[256];
} new_names;

typedef struct {
    GtkWidget *first_dir_wid;
    GtkWidget *second_dir_wid;
    GtkWidget *artist_1_wid;
    GtkWidget *artist_2_wid;
    GtkWidget *album_1_wid;
    GtkWidget *album_2_wid;
    GtkWidget *first_window;
} setup;

#endif //SAGGER_STRUCTURES_H
