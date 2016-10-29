#include <windows.h>

extern int optind;
int getopt (int argc, char *const *argv, const char *optstring);

extern int (__stdcall *pNtQueryPerformanceCounter)(
  LARGE_INTEGER *PerformanceCounter,
  LARGE_INTEGER *PerformanceFrequency
);

extern LARGE_INTEGER PerformanceFrequency;

void *fix_time ()
{
	FILETIME ft0;
	static LARGE_INTEGER t0;

	GetSystemTimeAsFileTime (&ft0);
	t0.u.LowPart = ft0.dwLowDateTime;
	t0.u.HighPart = ft0.dwHighDateTime;

	return &t0;
}

unsigned long seconds_elapsed (void *arg)
{
	FILETIME ft1;
	LARGE_INTEGER t1, *t0 = arg;
	unsigned long seconds;

	GetSystemTimeAsFileTime (&ft1);
	t1.u.LowPart = ft1.dwLowDateTime;
	t1.u.HighPart = ft1.dwHighDateTime;

	seconds = (unsigned long)((t1.QuadPart - t0->QuadPart) / 10000000ul);
	if (seconds < 1)
		seconds = 1;
	return seconds;
}

void get_current_timestamp(LARGE_INTEGER *pTimestamp)
{
	if (pNtQueryPerformanceCounter)
		pNtQueryPerformanceCounter (pTimestamp, 0);
	else {
		FILETIME filetime;
		GetSystemTimeAsFileTime (&filetime);
		pTimestamp->HighPart = filetime.dwHighDateTime;
		pTimestamp->LowPart = filetime.dwLowDateTime;
	}
}

void delay_100ns_since (unsigned long target_100ns, LARGE_INTEGER timestamp)
{
	LARGE_INTEGER target_delta, now;

	target_delta.QuadPart = target_100ns;
	if (pNtQueryPerformanceCounter)
		target_delta.QuadPart = target_delta.QuadPart * PerformanceFrequency.QuadPart / 10000000l;

	for (;;) {
		get_current_timestamp (&now);
		if (now.QuadPart - timestamp.QuadPart >= target_delta.QuadPart)
			break;
		Sleep(0);
	}
}

void usleep (unsigned usec)
{
	LARGE_INTEGER timestamp;
	get_current_timestamp(&timestamp);
	if (usec >= 1000)
		Sleep (usec / 1000);
	delay_100ns_since (usec * 10, timestamp);
}
