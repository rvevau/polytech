#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

struct semaphore hydrogen_semaphore;
struct semaphore oxygen_semaphore;
int hydrogen_count = 0;

static void init(void)
{
    sema_init(&hydrogen_semaphore, 0);
    sema_init(&oxygen_semaphore, 0);
}

static void oxygen(void* arg UNUSED)
{
    msg("O created.");
    sema_down(&oxygen_semaphore);
    sema_down(&hydrogen_semaphore);
    sema_down(&hydrogen_semaphore);
    timer_sleep(10);
    msg("Molecule H2O created.");
}

static void hydrogen(void* arg UNUSED)
{
    msg("H created.");

    hydrogen_count++;
    sema_up(&hydrogen_semaphore);
    if (hydrogen_count == 2) {
	sema_up(&oxygen_semaphore);
        hydrogen_count = 0;
    }
}

void test_molec(unsigned int num_oxygen, unsigned int num_hydrogen, int interval)
{
    int last_hyd = 1;
    unsigned int oxy = 0, hyd = 0;
    init();

    while ((oxy < num_oxygen) || (hyd < num_hydrogen))
    {
        timer_sleep(interval);

        if (oxy < num_oxygen && (last_hyd || (num_hydrogen == hyd)))
        {
            oxy++;
            thread_create("oxygen", PRI_DEFAULT, &oxygen, 0);
            last_hyd = 0;
        }
        else if (hyd < num_hydrogen)
        {
            hyd++;
            thread_create("hydrogen", PRI_DEFAULT, &hydrogen, 0);
            last_hyd = 1;
        }
    }

    timer_msleep(5000);
    pass();
}

