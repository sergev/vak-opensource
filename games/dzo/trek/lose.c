# include       "trek.h"

/*
**  PRINT OUT LOSER MESSAGES
**
**      The messages are printed out, the score is computed and
**      printed, and the game is restarted.  Oh yeh, any special
**      actions which need be taken are taken.
*/

char    *Losemsg[] =
{
	"Время кончилось",
	"Энергия кончилась",
	"Вы уничтожены",
	"Вы влетели в негативный энергетический барьер",
	"Вы погибли при взрыве новой звезды",
	"Вы погибли при взрыве сверхновой",
	"Вы остались умирать в пустом пространстве",
	"Вас не смогли рематериализировать",
	"\n *** Корпус корабля был пробит ***",
	"Вы сгорели в звезде",
	"Да, Вы уничтожили себя, но вряд ли в этом есть что-либо хорошее",
	"Вы были захвачены в плен клингами и беспощадно уничтожены",
	"Последний член Вашей команды погиб",
};

void lose(why)
int     why;
{
	Game.killed = 1;
	sleep(1);
	printf("\n%s\n", Losemsg[why - 1]);
	switch (why)
	{

	  case L_NOTIME:
		Game.killed = 0;
		break;
	}
	Move.endgame = -1;
	score();
	skiptonl(0);
	reset();
}
