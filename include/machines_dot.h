/*
  This file is part of Machines.

  Machines is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  Machines is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with YacJP.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _MACHINES_DOTS_H_
#define _MACHINES_DOTS_H_

/**
 * @file
 * Want dot diagrams? Add those functions to your states.
 */

#include "machines_states.h"

typedef struct machines_dot machines_dot_t;

typedef void (*machines_dot_free_fn)(machines_dot_t *this);
typedef machines_state_observer_t (*machines_dot_on_entry_fn)(machines_dot_t *this, char *label);
typedef machines_state_observer_t (*machines_dot_on_exit_fn)(machines_dot_t *this, char *label);
typedef machines_state_transition_t (*machines_dot_transition_fn)(machines_dot_t *this, char *label, machines_state_transition_t transition);
typedef int (*machines_dot_generate_fn)(machines_dot_t *this, char *buffer, int buffer_size);

struct machines_dot {
     machines_dot_free_fn       free;
     machines_dot_on_entry_fn   on_entry;
     machines_dot_on_exit_fn    on_exit;
     machines_dot_transition_fn transition;
     machines_dot_generate_fn   generate;
};

__PUBLIC__ machines_dot_t *machines_new_dot(char *name, cad_memory_t memory);

#endif /* _MACHINES_DOTS_H_ */
