//***************************************************************************
//                          main.c
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

#include "cli.h"
#include "gtk.h"
#include "structures.h"
#include <gtk/gtk.h>
#include <stdio.h>

// TODO Implement translation system

int main(int argc, char **argv) {
  int status = 0;

  if (argc < 2) {
    GtkApplication *app;

    app = gtk_application_new("pl.danelski.sagger", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
  } else
    status = parse_command(argc, argv);

  return status;
}
