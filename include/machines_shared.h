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

#ifndef _MACHINES_SHARED_H_
#define _MACHINES_SHARED_H_

/**
 * @file
 * A shared header for all Machines subsystems.
 */

#include <stdlib.h>

#define __PUBLIC__ __attribute__((__visibility__("default")))
#define __PRINTF__ __attribute__((format(printf, 2, 3)))

#endif /* _MACHINES_SHARED_H_ */
