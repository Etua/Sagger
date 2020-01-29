//***************************************************************************
//                          json.c
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

#include "json.h"
#include "structures.h"
#include <glib-object.h>
#include <json-glib/json-glib.h>
#include <string.h>

//JSON parser that handles the file from start to finish
// "^" will be later used as an indicator of lacking or corrupted data
void glib_parse(recording_metadata *json_result, char *json_filename) {
    JsonParser *json_parser;
    JsonNode *json_node;
    JsonReader *json_cursor = NULL;

    json_parser = json_parser_new();

    if(json_parser_load_from_file(json_parser, json_filename, NULL)) {
        json_node = json_parser_get_root(json_parser);
        json_cursor = json_reader_new(json_node);

        json_reader_read_member(json_cursor, "results");
        if(json_reader_count_elements(json_cursor) > 0) {
            //Loop is used in order to allow quick skip to the end of the function
            for(int i = 0; i < 1; i++) {
                json_reader_read_element(json_cursor, 0);

                //Read the artist's name
                json_reader_read_member(json_cursor, "recordings");
                json_reader_read_element(json_cursor, 0);
                json_reader_read_member(json_cursor, "artists");
                json_reader_read_element(json_cursor, 0);
                if(!json_reader_read_member(json_cursor, "name")) {
                    json_result->song_name[0] = '^';
                    break;
                }
                strcpy(json_result->artist_name, json_reader_get_string_value(json_cursor));
                //Cursor climbs up the structure
                json_reader_end_member(json_cursor);
                json_reader_end_element(json_cursor);
                json_reader_end_member(json_cursor);

                //Retrieve name of the album
                json_reader_read_member(json_cursor, "releasegroups");
                json_reader_read_element(json_cursor, 0);
                if(!json_reader_read_member(json_cursor, "title")) {
                    json_result->song_name[0] = '^';
                    break;
                }
                strcpy(json_result->album_name, json_reader_get_string_value(json_cursor));
                //Cursor climbs up the structure
                json_reader_end_member(json_cursor);
                json_reader_end_element(json_cursor);
                json_reader_end_member(json_cursor);

                //Retrieve song name
                if(!json_reader_read_member(json_cursor, "title")) {
                    json_result->song_name[0] = '^';
                    break;
                }
                strcpy(json_result->song_name, json_reader_get_string_value(json_cursor));
            }
        } else {
            json_result->song_name[0] = '^';
        }
    }
    //Free JSON specific objects
    g_object_unref(json_cursor);
    g_object_unref(json_parser);
}