
# include <stdio.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

fputhdr (filhdr, coutb)
struct exec *filhdr;
register FILE *coutb;
{
	fputh ((long) filhdr->a_magic, coutb);
	fputh (0L, coutb);

	fputh ((long) filhdr->a_const, coutb);
	fputh (0L, coutb);

	fputh ((long) filhdr->a_text, coutb);
	fputh (0L, coutb);

	fputh ((long) filhdr->a_data, coutb);
	fputh (0L, coutb);

	fputh ((long) filhdr->a_bss, coutb);
	fputh (0L, coutb);

	fputh ((long) filhdr->a_abss, coutb);
	fputh (0L, coutb);

	fputh ((long) filhdr->a_syms, coutb);
	fputh (0L, coutb);

	fputh ((long) filhdr->a_entry, coutb);
	fputh (0L, coutb);

	fputh ((long) filhdr->a_flag, coutb);
	fputh (0L, coutb);
}
