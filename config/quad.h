/**
 * @file
 * Type representing a quad-option
 *
 * @authors
 * Copyright (C) 2017-2018 Richard Russon <rich@flatcap.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MUTT_CONFIG_QUAD_H
#define MUTT_CONFIG_QUAD_H

struct Buffer;
struct ConfigSet;
struct HashElem;

extern const char *quad_values[];

/**
 * enum QuadOption - Possible values for a quad-option
 */
enum QuadOption
{
  MUTT_ABORT = -1,
  MUTT_NO,
  MUTT_YES,
  MUTT_ASKNO,
  MUTT_ASKYES
};

void quad_init(struct ConfigSet *cs);
int quad_he_toggle(struct ConfigSet *cs, struct HashElem *he, struct Buffer *err);

#endif /* MUTT_CONFIG_QUAD_H */
