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
  along with Machines.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "test.h"
#include "machines.h"

/*
 * Every machine is also a state. A whole machine is just a single state.
 *
 *  -[root]---------------------------
 * |                                  |
 * |   -[a]-       -[b]-       -[c]-  |
 * |  |     | --> |     | --> |     | |
 * |   -----       -----       -----  |
 * |                                  |
 *  ----------------------------------
 */

static void entry_exit(machine_state_t *state, void *payload) {
     char *what = (char*)payload;
     char *state_name = state ? (char*)state->payload(state) : "<nil>";
     printf("%s %s\n", what, state_name);
}

static bool_t always_true(machine_state_t *from_state, machine_state_t *to_state, void *payload) {
     char *transition = (char*)payload;
     char *from_name = from_state ? (char*)from_state->payload(from_state) : "<nil>";
     char *to_name = to_state ? (char*)to_state->payload(to_state) : "<nil>";
     printf("[%s] from %s to %s\n", transition, from_name, to_name);
     return true;
}

int main() {
     machine_state_t *root;
     machine_state_t *a;
     machine_state_t *b;
     machine_state_t *c;

     a = machines_new_state("a", root, NULL);
     root = machines_new_state("root", NULL, a);
     b = machines_new_state("b", root, NULL);
     c = machines_new_state("c", root, NULL);

     a->add_transition(a, always_true, b, "a->b");
     b->add_transition(b, always_true, c, "b->c");

     root->add_entry(root, entry_exit, "enter");
     a->add_entry(a, entry_exit, "enter");
     b->add_entry(b, entry_exit, "enter");
     c->add_entry(c, entry_exit, "enter");

     root->add_exit(root, entry_exit, "exit");
     a->add_exit(a, entry_exit, "exit");
     b->add_exit(b, entry_exit, "exit");
     c->add_exit(c, entry_exit, "exit");

     root->trigger(root);
     assert(root->current(root) == a);

     root->trigger(root);
     assert(root->current(root) == b);

     root->trigger(root);
     assert(root->current(root) == c);

     root->trigger(root);
     assert(root->current(root) == NULL);

     root->trigger(root);
     assert(root->current(root) == a);

     return 0;
}
