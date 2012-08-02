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

/*
 * Every machine is also a state. A whole machine is just a single state.
 *
 * [a] is the [root] entry state.
 *
 * [root] does not have any exit state so there is no way to quit the
 * sub-machine.
 *
 *  -[root]---------------------------
 * |                                  |
 * |   -[a]-       -[b]-       -[c]-  |
 * |  |     | --> |     | --> |     | |
 * |   -----       -----       -----  |
 * |     ^                       |    |
 * |     `-----------------------'    |
 * |                                  |
 *  ----------------------------------
 */

int main() {
     machine_state_t *root;
     machine_state_t *a;
     machine_state_t *b;
     machine_state_t *c;

     root = machines_new_state("root", NULL);
     a = machines_new_state("a", root);
     b = machines_new_state("b", root);
     c = machines_new_state("c", root);

     root->entry_at(root, a);

     a->add_transition(a, b, always_true, "a->b");
     b->add_transition(b, c, always_true, "b->c");
     c->add_transition(c, a, always_true, "c->a");

     root->add_entry(root, print_entry, NULL);
     a->add_entry(a, print_entry, NULL);
     b->add_entry(b, print_entry, NULL);
     c->add_entry(c, print_entry, NULL);

     root->add_exit(root, print_exit, NULL);
     a->add_exit(a, print_exit, NULL);
     b->add_exit(b, print_exit, NULL);
     c->add_exit(c, print_exit, NULL);

     root->trigger(root);
     assert(root->current(root) == a);

     root->trigger(root);
     assert(root->current(root) == b);

     root->trigger(root);
     assert(root->current(root) == c);

     root->trigger(root);
     assert(root->current(root) == a);

     return 0;
}
