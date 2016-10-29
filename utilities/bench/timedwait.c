/*
 * Под Линуксом надо линковать с -lpthread, иначе работает неправильно.
 */
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define N       100

unsigned t0, t1, counter;

/*
 * Реальное время в микросекундах.
 */
unsigned getusec ()
{
	struct timeval tv;

	gettimeofday (&tv, 0);
	return tv.tv_sec * 1000000 + tv.tv_usec;

}

/*
 * Функция вызывается по таймеру реального времени.
 */
static void cpu_sigalarm (int signum)
{
	if (counter == 0) {
		/* Первый отсчёт времени. */
		t0 = getusec ();
	}
	++counter;
	if (counter <= N)
		return;

	/* Последний отсчёт после N интервалов. */
	t1 = getusec ();
	printf ("Measured interval is %.2f msec.\n",
		(t1 - t0) / 1000.0 / N);
	exit (0);

}

int main (int argc, char **argv)
{
	struct timespec ts;
	struct timeval tv;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	int msec;

	if (argc != 2) {
		fprintf (stderr, "Usage:\n");
		fprintf (stderr, "	timerbench msec\n");
		exit (1);
	}
	msec = strtol (argv[1], 0, 0);

	pthread_mutex_lock (&mutex);

	gettimeofday (&tv, 0);
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;

	counter = 0;
	for (;;) {
		ts.tv_nsec += msec * 1000000;
		if (ts.tv_nsec >= 1000000000) {
			ts.tv_sec += 1;
			ts.tv_nsec -= 1000000000;
		}
		pthread_cond_timedwait (&cond, &mutex, &ts);
		cpu_sigalarm (0);
	}
}
