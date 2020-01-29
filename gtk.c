//***************************************************************************
//                          gtk.c
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

#include "gtk.h"
#include "run.h"
#include "structures.h"
#include <string.h>
#include <gtk/gtk.h>

//Second window opener that is used both for errors and indicating end of work
void open_second_window(GtkWidget *first_window, char *second_window_label) {
    GtkWidget *second_window, *label, *content_area;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    second_window = gtk_dialog_new_with_buttons("End", GTK_WINDOW(first_window),
                                                flags, ("OK"), GTK_RESPONSE_ACCEPT, NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG (second_window));
    label = gtk_label_new(second_window_label);
    g_signal_connect_swapped(second_window, "response", G_CALLBACK(gtk_widget_destroy), second_window);
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(second_window);
}

//Prepares window available under "Info"
void open_info_window(GtkWidget *somewidget) {
    GtkAboutDialog *about;
    const gchar *authors[] = {"Dominik 'Etua' Danelski<dominik@danelski.pl>", NULL};

    //TODO Inlude logo
    about = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
    gtk_about_dialog_set_program_name(about, program_name);
    gtk_about_dialog_set_version(about, version_number);
    gtk_about_dialog_set_copyright(about, "Copyright © 2020 Dominik 'Etua' Danelski");
    gtk_about_dialog_set_comments(about, "Tag audio files with ease");
    gtk_about_dialog_set_license_type(about, GTK_LICENSE_GPL_3_0);
    gtk_about_dialog_set_authors(about, authors);
    gtk_dialog_run(GTK_DIALOG(about));
    gtk_widget_destroy( GTK_WIDGET(about));
}

//Extracts data from GTK objects after the "Run" button has been used
static void run_all(GtkWidget *somewidget, setup *passed_settings) {
    run_settings final;
    char error_label[256];
    //TODO File chooser may be requred to be freed

    if(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(passed_settings->first_dir_wid)) == NULL ||
            gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(passed_settings->second_dir_wid)) == NULL) {
        create_label(-3, error_label);
        open_second_window(passed_settings->first_window, error_label);
    }
    else {
        strcpy(final.f_dir_1, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(passed_settings->first_dir_wid)));
        strcpy(final.f_dir_2, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(passed_settings->second_dir_wid)));
        final.is_artist[0] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(passed_settings->artist_1_wid));
        final.is_artist[1] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(passed_settings->artist_2_wid));
        final.is_album[0] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(passed_settings->album_1_wid));
        final.is_album[1] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(passed_settings->album_2_wid));
        final.first_window = passed_settings->first_window;
        master_function(final, TRUE);
    }
}

//Sets a widget to use maximum possible space
static void set_expand(GtkWidget *widget) {
    gtk_widget_set_hexpand(widget, 1);
    gtk_widget_set_vexpand(widget, 1);
}

//Main GTK function, the whole main window is defined there
void activate(GtkApplication *app) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *frame;
    GtkWidget *settings;


    GtkWidget *directory1;
    GtkWidget *directory2;
    GtkWidget *artist_1;
    GtkWidget *artist_2;
    GtkWidget *album_1;
    GtkWidget *album_2;
    GtkWidget *both;
    GtkWidget *button;

    //Prepares main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW (window), "Sagger");
    gtk_window_set_default_size(GTK_WINDOW (window), 400, 200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    //Prepares app grid
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    //Prepares menu to choose directories
    settings = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    frame = gtk_frame_new("Source and target directories");
    set_expand(settings);
    gtk_container_add(GTK_CONTAINER(frame), settings);
    directory1 = gtk_file_chooser_button_new("Source directory", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    set_expand(directory1);
    gtk_container_add(GTK_CONTAINER(settings), directory1);
    directory2 = gtk_file_chooser_button_new("Target directory", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    set_expand(directory2);
    gtk_container_add(GTK_CONTAINER(settings), directory2);
    gtk_grid_attach(GTK_GRID(grid), frame, 0, 0, 2, 1);

    //Prepares menu to choose directory name
    settings = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(settings), GTK_BUTTONBOX_EXPAND);
    frame = gtk_frame_new("Directory name");
    set_expand(settings);
    gtk_container_add(GTK_CONTAINER(frame), settings);
    gtk_grid_attach(GTK_GRID(grid), frame, 0, 1, 1, 1);
    artist_1 = gtk_radio_button_new_with_label_from_widget(NULL, "Artist");
    gtk_container_add(GTK_CONTAINER(settings), artist_1);
    album_1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(artist_1), "Album");
    gtk_container_add(GTK_CONTAINER(settings), album_1);
    both = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(artist_1), "Artist and album");
    gtk_container_add(GTK_CONTAINER(settings), both);

    //Prepares menu to choose filename
    settings = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(settings), GTK_BUTTONBOX_EXPAND);
    frame = gtk_frame_new("Filename");
    set_expand(settings);
    gtk_container_add(GTK_CONTAINER(frame), settings);
    gtk_grid_attach(GTK_GRID(grid), frame, 1, 1, 1, 1);
    artist_2 = gtk_toggle_button_new_with_label("Artist");
    gtk_container_add(GTK_CONTAINER(settings), artist_2);
    album_2 = gtk_toggle_button_new_with_label("Album");
    gtk_container_add(GTK_CONTAINER(settings), album_2);
    button = gtk_toggle_button_new_with_label("Song");
    gtk_widget_set_sensitive(button, FALSE);
    gtk_container_add(GTK_CONTAINER(settings), button);

    //Prepares "Info" and "Run" buttons
    static setup setup_run;
    settings = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(settings), GTK_BUTTONBOX_EXPAND);
    set_expand(settings);
    gtk_grid_attach(GTK_GRID(grid), settings, 0, 2, 2, 1);
    button = gtk_button_new_with_label("Info");
    gtk_container_add(GTK_CONTAINER(settings), button);
    g_signal_connect(button, "clicked", G_CALLBACK(open_info_window), NULL);
    button = gtk_button_new_with_label("Run");
    gtk_container_add(GTK_CONTAINER(settings), button);
    g_signal_connect(button, "clicked", G_CALLBACK(run_all), (gpointer) &setup_run);

    //Attaches GTK objects to be passed to run_all
    setup_run.first_dir_wid = directory1;
    setup_run.second_dir_wid = directory2;
    setup_run.artist_1_wid = artist_1;
    setup_run.artist_2_wid = artist_2;
    setup_run.album_1_wid = album_1;
    setup_run.album_2_wid = album_2;
    setup_run.first_window = window;

    gtk_widget_show_all(window);
}