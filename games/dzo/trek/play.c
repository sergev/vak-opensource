# include       "trek.h"
# include       "getpar.h"

/*
**  INSTRUCTION READ AND MAIN PLAY LOOP
**
**      Well folks, this is it.  Here we have the guts of the game.
**      This routine executes moves.  It sets up per-move variables,
**      gets the command, and executes the command.  After the command,
**      it calls events() to use up time, attack() to have Klingons
**      attack if the move was not free, and checkcond() to check up
**      on how we are doing after the move.
*/
extern void     abandon(), destruct(), dock(), shell(), dcrept(), torped();
extern void     capture(), help(), dumpgame(), computer(), impulse(), shield();
extern void     rest(), reset(), srscan(), warp(), visual(), lrscan();
extern void     undock(), setwarp(), phaser();

void reset() {
# include       <setjmp.h>
	extern jmp_buf jmpbuf ;

	longjmp( jmpbuf, 0 ) ;
}

struct cvntab   Comtab[] =
{
      { "abandon",              "",             0,  0,  abandon   },
      { "покинуть",             "",             0,  0,  abandon   },
      { "ca",                   "pture",        0,  0,  capture   },
      { "вз",                   "зять",         0,  0,  capture   },
      { "cl",                   "oak",          -1, 0,  shield    },
      { "зак",                  "рыться",       -1, 0,  shield    },
      { "c",                    "omputer",      0,  0,  computer  },
      { "к",                    "омпьютер",     0,  0,  computer  },
      { "da",                   "mages",        0,  0,  dcrept    },
      { "по",                   "вреждения",    0,  0,  dcrept    },
      { "destruct",             "",             0,  0,  destruct  },
      { "разрушить",            "",             0,  0,  destruct  },
      { "do",                   "ck",           0,  0,  dock      },
      { "ст",                   "ыковка",       0,  0,  dock      },
      { "help",                 "",             0,  0,  help      },
      { "помощь",               "",             0,  0,  help      },
      { "i",                    "mpulse",       0,  0,  impulse   },
      { "и",                    "мпульс",       0,  0,  impulse   },
      { "m",                    "ove",          0,  0,  warp      },
      { "дв",                   "ижение",       0,  0,  warp      },
      { "l",                    "rscan",        0,  0,  lrscan    },
      { "д",                    "лок",          0,  0,  lrscan    },
      { "p",                    "hasers",       0,  0,  phaser    },
      { "ф",                    "азер",         0,  0,  phaser    },
      { "ram",                  "",             1,  0,  warp      },
      { "таран",                "",             1,  0,  warp      },
      { "dump",                 "",             0,  0,  dumpgame  },
      { "сох",                  "ранить",       0,  0,  dumpgame  },
      { "r",                    "est",          0,  0,  rest      },
      { "р",                    "емонт",        0,  0,  rest      },
      { "shell",                "",             0,  0,  shell     },
      { "шелл",                 "",             0,  0,  shell     },
      { "sh",                   "ield",         0,  0,  shield    },
      { "з",                    "ащита",        0,  0,  shield    },
      { "s",                    "rscan",        0,  0,  srscan    },
      { "б",                    "лок",          0,  0,  srscan    },
      { "st",                   "atus",         -1, 0,  srscan    },
      { "со",                   "стояние",      -1, 0,  srscan    },
      { "terminate",            "",             0,  0,  reset     },
      { "конец",                "",             0,  0,  reset     },
      { "t",                    "orpedo",       0,  0,  torped    },
      { "т",                    "орпеда",       0,  0,  torped    },
      { "u",                    "ndock",        0,  0,  undock    },
      { "о",                    "тстыковаться", 0,  0,  undock    },
      { "v",                    "isual",        0,  0,  visual    },
      { "в",                    "изуально",     0,  0,  visual    },
      { "w",                    "arp",          0,  0,  setwarp   },
      { "ск",                   "орость",       0,  0,  setwarp   },
      {  0, 0, 0, 0, 0 }
};

play()
{
	struct cvntab           *r;

	while (1)
	{
		Move.free = 1;
		Move.time = 0.0;
		Move.shldchg = 0;
		Move.newquad = 0;
		Move.resting = 0;
		skiptonl(0);
		r = getcodpar("\nКоманда", Comtab);
		(*r->fun)(r->value);
		events(0);
		attack(0);
		checkcond();
	}
}
