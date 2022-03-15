//***************************************************************************
//                          fpcalc.h
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

#include "structures.h"
#ifndef SAGGER_FPCALC_H
#define SAGGER_FPCALC_H

void fpcalc(fpcalc_data *fpc_result, char *fpc_filename);
void request_constructor(int curl_lenght, char *curl_id, char *curl_api,
                         char *curl_request);
int curl_download_data(char *curl_filename, char *curl_request);

#endif // SAGGER_FPCALC_H
