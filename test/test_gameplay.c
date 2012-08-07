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
 * A simple game with one menu and two levels
 */

int main() {
     machines_state_t *root;
     machines_state_t *level_1;
     machines_state_t *level_2;
     machines_state_t *menu;
     machines_state_t *level_1_play;
     machines_state_t *level_1_quit;
     machines_state_t *level_2_play;
     machines_state_t *level_2_quit;

     machines_dot_t *dot = machines_new_dot("test_gameplay");
     char *dot_out = (char*)malloc(16384);

     int condition_level_1 = 0;
     int condition_level_2 = 0;
     int condition_quit    = 0;

     root         = machines_new_state("root", NULL);
     level_1      = machines_new_state("level 1", root);
     level_2      = machines_new_state("level 2", root);
     menu         = machines_new_state("menu", root);
     level_1_play = machines_new_state("play level 1", level_1);
     level_1_quit = machines_new_state("quit level 1", level_1);
     level_2_play = machines_new_state("play level 2", level_2);
     level_2_quit = machines_new_state("quit level 2", level_2);

     root        ->add_entry(root        , dot->on_entry(dot, "root        "));
     level_1     ->add_entry(level_1     , dot->on_entry(dot, "level_1     "));
     level_2     ->add_entry(level_2     , dot->on_entry(dot, "level_2     "));
     menu        ->add_entry(menu        , dot->on_entry(dot, "menu        "));
     level_1_play->add_entry(level_1_play, dot->on_entry(dot, "level_1_play"));
     level_1_quit->add_entry(level_1_quit, dot->on_entry(dot, "level_1_quit"));
     level_2_play->add_entry(level_2_play, dot->on_entry(dot, "level_2_play"));
     level_2_quit->add_entry(level_2_quit, dot->on_entry(dot, "level_2_quit"));

     root        ->add_exit(root        , dot->on_exit(dot, "root        "));
     level_1     ->add_exit(level_1     , dot->on_exit(dot, "level_1     "));
     level_2     ->add_exit(level_2     , dot->on_exit(dot, "level_2     "));
     menu        ->add_exit(menu        , dot->on_exit(dot, "menu        "));
     level_1_play->add_exit(level_1_play, dot->on_exit(dot, "level_1_play"));
     level_1_quit->add_exit(level_1_quit, dot->on_exit(dot, "level_1_quit"));
     level_2_play->add_exit(level_2_play, dot->on_exit(dot, "level_2_play"));
     level_2_quit->add_exit(level_2_quit, dot->on_exit(dot, "level_2_quit"));

     root->entry_at(root, menu);
     menu->add_transition(menu, level_1, dot->transition(dot, "menu->level_1", iff("menu->level_1", &condition_level_1)));
     menu->add_transition(menu, level_2, dot->transition(dot, "menu->level_2", iff("menu->level_2", &condition_level_2)));
     level_1->add_transition(level_1, menu, dot->transition(dot, "level_1->menu", always_true("level 1 finished, back to menu")));
     level_2->add_transition(level_2, menu, dot->transition(dot, "level_1->menu", always_true("level 2 finished, back to menu")));

     level_1->entry_at(level_1, level_1_play);
     level_1->exit_at(level_1, level_1_quit);
     level_1_play->add_transition(level_1_play, level_1_quit, dot->transition(dot, "level_1_play->level_1_quit", iff("play->quit", &condition_quit)));
     level_1_play->add_transition(level_1_play, level_1_play, dot->transition(dot, "level_1_play->level_1_play", always_true("continue playing level 1"))); // note that transitions are checked in order until first match

     level_2->entry_at(level_2, level_2_play);
     level_2->exit_at(level_2, level_2_quit);
     level_2_play->add_transition(level_2_play, level_2_quit, dot->transition(dot, "level_2_play->level_2_quit", iff("play->quit", &condition_quit)));
     level_2_play->add_transition(level_2_play, level_2_play, dot->transition(dot, "level_2_play->level_2_play", always_true("continue playing level 2"))); // note that transitions are checked in order until first match

     // application startup
     printf("**** trigger: go to menu\n");
     root->trigger(root);
     assert(root->current(root) == menu);

     // the menu is displayed; the user chooses the first level
     condition_level_1 = 1;
     printf("**** trigger: go to first level\n");
     root->trigger(root);
     assert(root->current(root) == level_1);
     assert(level_1->current(level_1) == level_1_play);

     // the user keeps playing
     printf("**** trigger: keep playing\n");
     root->trigger(root);
     assert(root->current(root) == level_1);
     assert(level_1->current(level_1) == level_1_play);

     // the user reaches the end of the level
     condition_quit = 1;
     printf("**** trigger: exit level\n");
     root->trigger(root);
     assert(root->current(root) == level_1);
     assert(level_1->current(level_1) == level_1_quit);

     // the user returns to the menu
     printf("**** trigger: back to the menu\n");
     root->trigger(root);
     assert(root->current(root) == menu);

     // (a bit of internal cleanup)
     condition_level_1 = 0;
     condition_quit = 0;

     // the menu is displayed; the user chooses the second level
     condition_level_2 = 1;
     printf("**** trigger: go to second level\n");
     root->trigger(root);
     assert(root->current(root) == level_2);
     assert(level_2->current(level_2) == level_2_play);

     // the user keeps playing
     printf("**** trigger: keep playing\n");
     root->trigger(root);
     assert(root->current(root) == level_2);
     assert(level_2->current(level_2) == level_2_play);

     // the user keeps playing
     printf("**** trigger: keep playing\n");
     root->trigger(root);
     assert(root->current(root) == level_2);
     assert(level_2->current(level_2) == level_2_play);

     // the user keeps playing
     printf("**** trigger: keep playing\n");
     root->trigger(root);
     assert(root->current(root) == level_2);
     assert(level_2->current(level_2) == level_2_play);

     // the user reaches the end of the level
     condition_quit = 1;
     printf("**** trigger: exit level\n");
     root->trigger(root);
     assert(root->current(root) == level_2);
     assert(level_2->current(level_2) == level_2_quit);

     // the user returns to the menu
     printf("**** trigger: back to the menu\n");
     root->trigger(root);
     assert(root->current(root) == menu);

     dot->generate(dot, dot_out, 16384);
     printf("--8<--------------------------------------------------------------------\n");
     printf(dot_out);
     printf("-------------------------------------------------------------------->8--\n");

     return 0;
}
