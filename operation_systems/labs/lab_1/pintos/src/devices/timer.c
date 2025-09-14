#include "devices/timer.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include "devices/pit.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
  
/* See [8254] for hardware details of the 8254 timer chip. */

#if TIMER_FREQ < 19
#error 8254 timer requires TIMER_FREQ >= 19
#endif
#if TIMER_FREQ > 1000
#error TIMER_FREQ <= 1000 recommended
#endif

#define MAX_SLEEPING_THREADS 256 //максимальное количество потоков в массиве

/* Number of timer ticks since OS booted. */
static int64_t ticks;

/* Number of loops per timer tick.
   Initialized by timer_calibrate(). */
static unsigned loops_per_tick;

//локальные функции
static intr_handler_func timer_interrupt;
static bool too_many_loops (unsigned loops);
static void busy_wait (int64_t loops);
static void real_time_sleep (int64_t num, int32_t denom);
static void real_time_delay (int64_t num, int32_t denom);

/* Структура для хранения информации о спящих потоках */
struct sleeping_thread {
    struct thread* t;  // Указатель на поток
    int64_t wake_up_time; // Время пробуждения
};

/* Массив для хранения спящих процессов */
static struct sleeping_thread sleeping_threads[MAX_SLEEPING_THREADS];
static int sleeping_threads_count = 0; // Количество спящих потоков


/* Sets up the timer to interrupt TIMER_FREQ times per second,
   and registers the corresponding interrupt. 

Инициализация таймера, преобразовывает частоту в определённое количество тактов в секунду. Устанавливает таймер на прерывание раз в секунду и регистрирует внешние прерывания.

*/
void
timer_init (void) 
{
  pit_configure_channel (0, 2, TIMER_FREQ);
  intr_register_ext (0x20, timer_interrupt, "8254 Timer");
}

//сортировка массива
void sort_sleeping_threads() {
    int i = 0;
    while (i < sleeping_threads_count) {
        if (i == 0 || sleeping_threads[i - 1].wake_up_time <= sleeping_threads[i].wake_up_time) {
            i++;
        } else {
            // Меняем местами элементы
            struct sleeping_thread temp = sleeping_threads[i];
            sleeping_threads[i] = sleeping_threads[i - 1];
            sleeping_threads[i - 1] = temp;
            i--; // Двигаемся назад
        }
    }
}

//удаление элемента из массива
void remove_sleeping_thread(int index){
	int i = index;
	do {
		sleeping_threads[i] = sleeping_threads[i + 1];
		i++;
	} while (i < sleeping_threads_count - 1);
	
	sleeping_threads_count--;

}


/* Calibrates loops_per_tick, used to implement brief delays.

Устанавливает величину loops_per_tick, переменная, которая используется для реализации коротких задержек.


 */
void
timer_calibrate (void) 
{
  unsigned high_bit, test_bit;

  ASSERT (intr_get_level () == INTR_ON);
  printf ("Calibrating timer...  ");

  /* Approximate loops_per_tick as the largest power-of-two
     still less than one timer tick. */
  loops_per_tick = 1u << 10;
  while (!too_many_loops (loops_per_tick << 1)) 
    {
      loops_per_tick <<= 1;
      ASSERT (loops_per_tick != 0);
    }

  /* Refine the next 8 bits of loops_per_tick. */
  high_bit = loops_per_tick;
  for (test_bit = high_bit >> 1; test_bit != high_bit >> 10; test_bit >>= 1)
    if (!too_many_loops (high_bit | test_bit))
      loops_per_tick |= test_bit;

  printf ("%'"PRIu64" loops/s.\n", (uint64_t) loops_per_tick * TIMER_FREQ);
}

/* Returns the number of timer ticks since the OS booted.

Возвращает количество тактов таймера, которые прошли с момента загрузки ОС.

 */
int64_t
timer_ticks (void) 
{
  enum intr_level old_level = intr_disable ();
  int64_t t = ticks;
  intr_set_level (old_level);
  return t;
}

/* Returns the number of timer ticks elapsed since THEN, which
   should be a value once returned by timer_ticks(). 

Возвращает количество тактов таймера, которые прошли с определённого момента then.

*/
int64_t
timer_elapsed (int64_t then) 
{
  return timer_ticks () - then;
}

/* Sleeps for approximately TICKS timer ticks.  Interrupts must
   be turned on. 

Погружает текущий процесс в сон. Пока происходит активное ожидание переводит другие процессы в состояние готовности. 

*/
void timer_sleep(int64_t ticks) {
    int64_t start = timer_ticks();
    int64_t wake_up_time = start + ticks;

    ASSERT(intr_get_level() == INTR_ON);

    // Отключить прерывания
    enum intr_level old_level = intr_disable();

    // Добавить поток в список спящих
    sleeping_threads[sleeping_threads_count].t = thread_current();
    sleeping_threads[sleeping_threads_count].wake_up_time = wake_up_time;
    sleeping_threads_count++;

    // Отсортировать массив спящих по времени пробуждения
    sort_sleeping_threads();

    // Перевести текущий поток в состояние ожидания
    thread_block();

    // Восстановить уровень прерываний
    intr_set_level(old_level);
}


/* Sleeps for approximately MS milliseconds.  Interrupts must be
   turned on. 

Процесс погружается в сон на миллисекунды. Прерывания должны быть включены.

*/
void
timer_msleep (int64_t ms) 
{
  real_time_sleep (ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts must be
   turned on.

Процесс спит миллисекунды. Прерывания должны быть включены.

 */
void
timer_usleep (int64_t us) 
{
  real_time_sleep (us, 1000 * 1000);
}

/* Sleeps for approximately NS nanoseconds.  Interrupts must be
   turned on. 

Процесс спит наносекунды. Прерывания должны быть выключены.

*/
void
timer_nsleep (int64_t ns) 
{
  real_time_sleep (ns, 1000 * 1000 * 1000);
}

/* Busy-waits for approximately MS milliseconds.  Interrupts need
   not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_msleep()
   instead if interrupts are enabled.

Поток занят. Процесс ждёт примерно млисекунды, из-за этого тратятся циклы процессора. Включение прерывания приведёт к потери тиков таймера.

 */
void
timer_mdelay (int64_t ms) 
{
  real_time_delay (ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts need not
   be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_usleep()
   instead if interrupts are enabled. 

Поток занят. Процесс спит микросекунды, из-за ожидания тратяся циклы процессора. Включение прерывания приведёт к потери тиков таймера.

*/
void
timer_udelay (int64_t us) 
{
  real_time_delay (us, 1000 * 1000);
}

/* Sleeps execution for approximately NS nanoseconds.  Interrupts
   need not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_nsleep()
   instead if interrupts are enabled.

Поток занят. Процесс спит наносекунды, из-за ожидания тратятся циклы процессора. Включение прерывания приведёт к потери тиков таймера.

*/
void
timer_ndelay (int64_t ns) 
{
  real_time_delay (ns, 1000 * 1000 * 1000);
}

/* Prints timer statistics. 

Печатает статистику таймера. Выводит количество тиков, которое отведено под определённый процесс.

*/
void
timer_print_stats (void) 
{
  printf ("Timer: %"PRId64" ticks\n", timer_ticks ());
}

/* Timer interrupt handler. 

Обработчик прерывания. Работа заключена на активном ожидании, при каждом такте таймера вызывает функцию, которая проверяет несколько условий и обновляет статистику. После достижения определённого условия возвразает из прерывания один процесс, чтобы уступить управления другому.

*/


static void timer_interrupt(struct intr_frame *args UNUSED) {
    ticks++;

    // Проверить массив спящих процессов
    int i = 0;
    while (i < sleeping_threads_count) {
        if (sleeping_threads[i].wake_up_time <= ticks) {
            // Пробудить поток
            thread_unblock(sleeping_threads[i].t);

            // Удалить поток из массива
            remove_sleeping_thread(i);
            // После удаления индекс остается на текущей позиции,
            // так как элементы сдвинуты
        } 
	// Так как массив отсортирован, если условие не выполнено,
        // можно прекратить проверку
	else break;
        
    }

    // Обновить состояние потоков
    thread_tick();
}


/* Returns true if LOOPS iterations waits for more than one timer
   tick, otherwise false. 

Возвращает истину если циклы итерации ожидает более чем один таймер.

*/
static bool
too_many_loops (unsigned loops) 
{
  /* Wait for a timer tick. */
  int64_t start = ticks;
  while (ticks == start)
    barrier ();

  /* Run LOOPS loops. */
  start = ticks;
  busy_wait (loops);

  /* If the tick count changed, we iterated too long. */
  barrier ();
  return start != ticks;
}

/* Iterates through a simple loop LOOPS times, for implementing
   brief delays.

   Marked NO_INLINE because code alignment can significantly
   affect timings, so that if this function was inlined
   differently in different places the results would be difficult
   to predict. 

Проходит цикл loops раз, это активное ожидание, которое используется для реализации коротких задержек.

*/
static void NO_INLINE
busy_wait (int64_t loops) 
{
  while (loops-- > 0)
    barrier ();
}

/* Sleep for approximately NUM/DENOM seconds.

Переводит по поределённой форме секунды в такты, пе чего на определённое количество тиков текущий процесс погружается в сон.

 */
static void
real_time_sleep (int64_t num, int32_t denom) 
{
  /* Convert NUM/DENOM seconds into timer ticks, rounding down.
          
        (NUM / DENOM) s          
     ---------------------- = NUM * TIMER_FREQ / DENOM ticks. 
     1 s / TIMER_FREQ ticks
  */
  int64_t ticks = num * TIMER_FREQ / denom;

  ASSERT (intr_get_level () == INTR_ON);
  if (ticks > 0)
    {
      /* We're waiting for at least one full timer tick.  Use
         timer_sleep() because it will yield the CPU to other
         processes. */                
      timer_sleep (ticks); 
    }
  else 
    {
      /* Otherwise, use a busy-wait loop for more accurate
         sub-tick timing. */
      real_time_delay (num, denom); 
    }
}

/* Busy-wait for approximately NUM/DENOM seconds. 

Поток занят, процесс ожидает несколько тиков, которые были переведены из секунд.

*/
static void
real_time_delay (int64_t num, int32_t denom)
{
  /* Scale the numerator and denominator down by 1000 to avoid
     the possibility of overflow. */
  ASSERT (denom % 1000 == 0);
  busy_wait (loops_per_tick * num / 1000 * TIMER_FREQ / (denom / 1000)); 
}
