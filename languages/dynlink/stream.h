#include "ada-compatible.h"

class Stream_t : public Ada_Compatible {
public:
	virtual void Put_String (const char *str, int strlim[2] = 0) = 0;
	virtual void Put_Integer (long val, int width = 0,
		int base = 10) = 0;
	virtual void Put_Unsigned (unsigned long val, int width = 0,
		int base = 10) = 0;
};
