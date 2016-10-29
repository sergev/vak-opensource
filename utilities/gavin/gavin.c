#define call(f,a,b,c)	((int (*)(int t, int q, int d))(f))(a,b,c)
#define word(m)		*((int*)(m))
#define byte(m)		*((signed char *)(m))
#define V		0x90200
#define K		0

#define L(c,d)		call (V+8, 100, c, 0); \
			while ((C = call (V-8, 100, 0, 0)) & 3) \
				D = call (V-8, 96, 0, 0); \
			call (V+8, 96, d, 0); \
			while ((C = call (V-8, 100, 0, 0)) & 3) \
				D = call (V-8, 96, 0, 0)

#define X(p,t,s)	(p>=t && p<(t+s) && (p-(t)&1023) < (s & 1023))
#define I(s)		(int)main-(int)s
#define P(s,c,k)	for (h=0; h>>14==0; h+=0x81)\
				word (16*c + h/1024 + word (V+36)) & 128 >> (h&7) ?\
					byte (s + (h & 0x3C07)) = k : k

int B (int t, int q, int d)
{
	int i=0, j=0, h, n, p = 0x60200, s = 26739, C, D;

	if (! --q) {
		D = call (word (V), C = call (word (V), word (t + 4) + 3, 4, 0), 2, 0);
		word (t + 12) = word (t + 20) = i;
		word (t + 24) = 1;
		word (t + 28) = t;
		word (t + 16) = 442890;
		word (t + 28) = d = call (word (V), s = D * 8 + 1664, 1, 0);
		for (p = 0; j < s; j++, p++)
			byte (d + j) = (i == D || j < p) ? p--, 0 :
				(n = byte (C + 512 + i++)) < ' ' ?
				p |= n * 56 - 497, 0 : n;
	}

	n = word (word (t + 4)) & 1;
	if (! --q) {
		byte (word (t + 28) + 1536) |= 62 & -n;
		for (D=0; D<0xC0000; D++)
			byte (d + D) = X (D, word (t + 12) + 26628, 412162) ?
				X (D, word (t + 12) + 27653, 410112) ?
				31 : 0 : byte (d + D);
		for (; j < 12800; j += 8)
			P (d + 27653 + word (t + 12) + ' ' * (j & ~511) + j % 512,
				byte (word (t + 28) + j / 8 + 64 * word (t + 20)), 0);
	}

	if (! --q) {
		if (n) {
			D = word (t + 28);
			if (d - 10)
				byte (++word (t + 24) + D + 1535) = d;
			else {
				for (i = D; i < D + 1600; i++)
					byte (i) = byte (i + 64);
				word (t + 24) = 1;
				call (word (V), i - 127, 3, 0);
			}
		} else
			word (t + 20) += ((d >> 4) ^ (d >> 5)) - 3;
	}
}

int _ (int t, int q, int d);
int o (int t, int q, int d);

int main (int t, int q, int d)
{
	int i=0, j=0, h, n, p = 0x60200, s = 26739, C, D, k = K;

	if (! t) {
		word (V) = V + 208 - (I (_));
		L (209, 223);
		L (168, 0);
		L (212, 244);
		_((int) &s, 3, 0);
		for (;;) {
			while ((C = call (V-8,100,0,0)) & 3) {
				D = call (V-8,96,0,0);
				n = word (V - 12);
				if (C & ' ') {
				    k++;
				    k %= 3;
				    if (k < 2) {
					word (j) -= p;
					word (j) += p += byte (&D) * (1 - k * 1025);
					if (k)
						goto y;
				    } else {
					for (C = V - 20;
					     !i && D & 1 && n
					     && (X (p, word (n + 12), word (n + 16)) ? j = n + 12, word (C + 8) =
						 word (n + 8), word (n + 8) = word (V - 12), word (V - 12) =
						 n, 0 : n); C = n, n = word (n + 8));
					i = D & 1;
					j &= -i;
				    }
				} else if (128 & ~D) {
					call (word (n), n, 3, byte (V + D % 64 + 131) ^ 32);
					n = word (V - 12);
y:					C = 1 << 24;
					for (D=0; D<0xC0000; D++)
						byte (C + D) = 125;
					o (n, 0, C);
					P (C + p - 8196, 88, 0);
					for (D=0; D<0xC0000; D++)
						byte (word (0x11028) + D) = byte (C + D);
				}
			}
		}
	}

	for (D = 720; D > -3888; D--)
		putchar (D > 0 ?
		   "  )!\320\234\360\256\370\256 0\230F           .,mnbvcxz    ;lkjhgfdsa \n][poiuytrewq  =-0987654321   \357\262   \337\337 \357\272   \337\337         ( )\"\343\312F\320!/ !\230 26!/\16 K>!/\16\332 \4\16\251\0160\355&\2271\20\2300\355`x\1730\355\347\2560 \237qpa%\231o!\230 \337\337\337     ,               )\"K\240   \343\316qrpxzy\0 sRDh\16\313\212u\343\314qrzy    !0( "
		   [D] ^ 32 : word (I (D)));
	return 0;
}

int o (int t, int q, int d)
{
	int i=0, j=0, h, n, C, D;

	if (t) {
		C = word (t + 12);
		j = word (t + 16);
		o (word (t + 8), 0, d);
		for (D=0; D<0xC0000; D++)
			byte (d + D) = X (D, C, j) ?
				X (D, C + 1025, j - 2050) ?
				X (D, C + 2050, j - 3075) ?
				X (D, C + 2050, j - 4100) ?
				X (D, C + 4100, ((j & 1023) + 18424)) ?
					176 : 24 : 20 : 28 : 0 : byte (d + D);
		for (n = word (t + 4); byte (i + n); i++)
			P (d + word (t + 12) + 5126 + i * 8, byte (n + i), 31);
		call (word (t), t, 2, d);
	}
}

int _ (int t, int q, int d)
{
	int i=0, j=0, n, D = word (V + 24);

	if (!--q) {
		word (V - 16) += t;
		D = word (V - 16) - t;
	}

	if (!--q) {
		for (i = 124; i < 135; i++)
			D = D << 3 | word (t + i) & 7;
	}

	if (q > 0) {
		for (; n = byte (D + i); i++)
			if (n - byte (t + i)) {
				D += _(D, 2, 0) + 1023 & ~511;
				i = ~0;
			}
		if (!--q) {
			if (word (D)) {
				n = _(164, 1, 0);
				word (n + 8) = word (V - 12);
				word (V - 12) = n;
				word (n + 4) = i = n + 64;
				for (; j < 96; j++)
					word (i + j) = word (t + j);
				i = D + 512;
				j = i + word (i + 32);
				for (; word (j + 12) != word (i + 24); j += 40)
					continue;
				call (word (n) = word (j + 16) + i, n, 1, 0);
			}
		}
	}
	return D;
}
