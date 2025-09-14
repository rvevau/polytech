
/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"

struct semaphore sema_cars_left;
struct semaphore sema_cars_right;
struct semaphore sema_emer_left;
struct semaphore sema_emer_right;

int cars_on_bridge = 0;

enum car_direction current_dir = -1;

// Called before test. Can initialize some synchronization objects.
void narrow_bridge_init(void)
{
	// Not implemented

	sema_init(&sema_cars_left, 0);
	sema_init(&sema_cars_right, 0);
	sema_init(&sema_emer_left, 0);
	sema_init(&sema_emer_right, 0);
}

void arrive_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED)
{
	// Not implemented. Remove UNUSED keyword if need.
	if (cars_on_bridge == 0) {
		cars_on_bridge++;
		current_dir = dir;
	}
	else if (cars_on_bridge == 1 && dir == current_dir) {
		cars_on_bridge++;
		current_dir = (current_dir == 1) ? 0 : 1;
	}
	else if ((cars_on_bridge == 1 && dir != current_dir) || cars_on_bridge == 2) {
		if (prio == car_emergency && dir == dir_left) {
			sema_down(&sema_emer_left);
		}
		if (prio == car_emergency && dir == dir_right) {
			sema_down(&sema_emer_right);
		}
		if (prio == car_normal && dir == dir_left) {
			sema_down(&sema_cars_left);
		}
		if (prio == car_normal && dir == dir_right) {
			sema_down(&sema_cars_right);
		}
	}
}

void exit_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED)
{
	// Not implemented. Remove UNUSED keyword if need.
	if (cars_on_bridge != 0) cars_on_bridge--;
	if (cars_on_bridge == 0) {
		if (!list_empty(&sema_emer_left.waiters) && list_empty(&sema_emer_right.waiters)) { 
			current_dir = dir_left;
			sema_up(&sema_emer_left);
			cars_on_bridge++;
			if (!list_empty(&sema_emer_left.waiters)) {
				sema_up(&sema_emer_left);
				cars_on_bridge++;
			}
			else if (!list_empty(&sema_cars_left.waiters)) {
				sema_up(&sema_cars_left);
				cars_on_bridge++;
			}
		}
		else if (!list_empty(&sema_emer_right.waiters) && list_empty(&sema_emer_left.waiters)) {
			current_dir = dir_right;
			sema_up(&sema_emer_right);
			cars_on_bridge++;
			if (!list_empty(&sema_emer_right.waiters)) {
                                sema_up(&sema_emer_right);
                                cars_on_bridge++;
                        }
			else if (!list_empty(&sema_cars_right.waiters)) {
				sema_up(&sema_cars_right);
				cars_on_bridge++;
			}
		}
		else if (!list_empty(&sema_emer_left.waiters) && !list_empty(&sema_emer_right.waiters)) {
			if (current_dir == dir) current_dir = (current_dir == 1) ? 0 : 1;
			if (current_dir == 0) {
				sema_up(&sema_emer_left);
				cars_on_bridge++;
				if (!list_empty(&sema_emer_left.waiters)) {
					sema_up(&sema_emer_left);
					cars_on_bridge++;
				}
				else if (!list_empty(&sema_cars_left.waiters)) {
					sema_up(&sema_cars_left);
					cars_on_bridge++;
				}
			}
			else {
				sema_up(&sema_emer_right);
				cars_on_bridge++;
				if (!list_empty(&sema_emer_right.waiters)) {
                                        sema_up(&sema_emer_right);
                                        cars_on_bridge++;
                                }
                                else if (!list_empty(&sema_cars_right.waiters)) {
                                        sema_up(&sema_cars_right);
                                        cars_on_bridge++;
                                }

			}
		}
		else if (!list_empty(&sema_cars_left.waiters) && list_empty(&sema_cars_right.waiters)) {
			sema_up(&sema_cars_left);
			cars_on_bridge++;
			if (!list_empty(&sema_cars_left.waiters)) {
				sema_up(&sema_cars_left);
				cars_on_bridge++;
			}
		}
		else if (!list_empty(&sema_cars_right.waiters) && list_empty(&sema_cars_left.waiters)) {
			sema_up(&sema_cars_right);
			cars_on_bridge++;
			if (!list_empty(&sema_cars_right.waiters)) {
				sema_up(&sema_cars_right);
				cars_on_bridge++;
			}
		}
		else if (!list_empty(&sema_cars_left.waiters) && !list_empty(&sema_cars_right.waiters)) {
			if (current_dir == dir) current_dir = (current_dir == 1) ? 0 : 1;
			if (current_dir == 1) {
                                sema_up(&sema_cars_right);
				cars_on_bridge++;
				if (!list_empty(&sema_cars_right.waiters)) {
                                	sema_up(&sema_cars_right);
					cars_on_bridge++;
				}
                        }
                        else {
				sema_up(&sema_cars_left);
                                cars_on_bridge++;
                                if (!list_empty(&sema_cars_left.waiters)) {
                                        sema_up(&sema_cars_left);
                                        cars_on_bridge++;
                                }

                        }

		}
	}

}
