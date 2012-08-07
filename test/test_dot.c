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
#include "machines_dot.h"

/*
 * Every machine is also a state. A whole machine is just a single state.
 *
 * [a] is the [root] entry state.
 *
 * [root] does not have any exit state; an implicit exit state is [c]
 * because it has no transitions.
 *
 *  -[root]---------------------------
 * |                                  |
 * |   -[a]-       -[b]-       -[c]-  |
 * |  |     | --> |     | --> |     | |
 * |   -----       -----       -----  |
 * |                                  |
 *  ----------------------------------
 */

int main() {
     machines_state_t *root;
     machines_state_t *a;
     machines_state_t *b;
     machines_state_t *c;
     machines_dot_t *dot = machines_new_dot("test_dot");
     char dot_out[1024];

     root = machines_new_state("root", NULL);
     a    = machines_new_state("a"   , root);
     b    = machines_new_state("b"   , root);
     c    = machines_new_state("c"   , root);

     root->entry_at(root, a);

     a->add_transition(a, b, dot->transition(dot, "a->b", always_true("a->b")));
     b->add_transition(b, c, dot->transition(dot, "b->c", always_true("b->c")));

     root->add_entry(root, print_entry());
     a   ->add_entry(a   , print_entry());
     b   ->add_entry(b   , print_entry());
     c   ->add_entry(c   , print_entry());

     root->add_entry(root, dot->on_entry(dot, "root"));
     a   ->add_entry(a   , dot->on_entry(dot, "a   "));
     b   ->add_entry(b   , dot->on_entry(dot, "b   "));
     c   ->add_entry(c   , dot->on_entry(dot, "c   "));

     root->add_exit(root, print_exit());
     a   ->add_exit(a   , print_exit());
     b   ->add_exit(b   , print_exit());
     c   ->add_exit(c   , print_exit());

     root->add_exit(root, dot->on_exit(dot, "root"));
     a   ->add_exit(a   , dot->on_exit(dot, "a   "));
     b   ->add_exit(b   , dot->on_exit(dot, "b   "));
     c   ->add_exit(c   , dot->on_exit(dot, "c   "));

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

     dot->generate(dot, dot_out, 1024);
     printf("--8<--------------------------------------------------------------------\n");
     printf(dot_out);
     printf("-------------------------------------------------------------------->8--\n");

     assert(!strcmp(dot_out,
                    "digraph test_dot {\n"
                    "\ta   [label=\"a\"];\n"
                    "\ta->b [label=\"(1)\"]\n"
                    "\tb   [label=\"b\"];\n"
                    "\tb->c [label=\"(2)\"]\n"
                    "\tc   [label=\"c\"];\n"
                    "}\n"
                 ));

     return 0;
}
