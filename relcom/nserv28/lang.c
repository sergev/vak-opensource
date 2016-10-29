/*
 * $Header: lang.c,v 2.0 92/08/05 08:42:52 vak Exp $
 * $Log:	lang.c,v $
 * Revision 2.0  92/08/05  08:42:52  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:20:07  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:10:53  usenet
 * 
 * Revision 1.2  92/06/19  22:08:13  usenet
 * News Server
 * 
 */
/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 * (C) 1993 Алексей Руднев <alex@kiae.su> - changes.
 */
# define extern /**/
# include "lang.h"

int lang = 'e';

static setenglish ()
{
	/* server.c */

	permission_denied = "Permission denied\n";
	server_reply = "reply from USENET server";
	news_server = "Relcom News Server, version %s.%d\nCopyright (C) 1991-1994 Serge Vakulenko\nModifications 1993 Aleksei Rudnev <alex@kiae.su>\n";
	to_be_continued = "-------- To be continued --------\n";

	/* reader.c */

	command_not_implemented = "Command is not implemented yet";
	too_long_line = "Too long line";
	too_many_arguments = "Too many arguments";

	/* commands.c */

	no_commands_entered = "No commands entered";
	no_help_available = "No help available";
	group_not_found = "Group %s not found\n";
	no_group_selected = "No group selected\n";
	no_such_group = "No such group '%s'\n";
	you_are_not_subscribed = "You are not subscribed to this server.\n";
	you_are_not_subscribed_to_group = "You are not subscribed to group %s.\n";
	no_such_article = "No such article\n";
	user_unsubscribed_from_group = "User %s unsubscribed from group %s\n";
	user_subscribed_to_group = "User %s subscribed to group %s\n";
	in_feed_mode = "in feed mode\n";
	in_notify_mode = "in notify mode\n";
	in_rfeed_mode = "in restricted feed mode\n";
	pack_turned_on = "Pack mode turned on\n";
	pack_turned_off = "Pack mode turned off\n";
	aging_turned_on =  "Aging turned on\n";
	aging_turned_off = "Aging turned off\n";
	no_such_package = "No such package %s\n";
	user_removed_from_list = "User %s removed from subscription list\n";
	you_are_not_registered = "You are not registered on this server\n";
	you_are_unknown = "You (%s) are unknown for me\n";
	short_list_of_news_groups="Short list of groups, for details ask 'groups NAME'\nwhere NAME may be simple comp, relcom, etc. or precise group name.\n\n\n";
	group_dont_allow="Group %s don't allowed for you\n";
	/* submit.c */

	cannot_post_article = "Cannot post article!\n";
	unsent_message_follows = "---------------- unsent message follows ----------------\n";
	groups_cannot_post = "Cannot post to the following groups:\n";
	dont_exist = "does not exist";
	no_moderator = "no moderator";
	closed_for_posting = "closed";
	bad_newsgroups_field = "Bad Newsgroups header line: %s\n";

	/* notify.c */

	new_groups_created = "New groups created:\n";
	groups_deleted = "Old groups deleted:";
	list_of_articles = "List of new Usenet articles";
	info_from_server = "Information from Usenet server";
	uncomment_this = "> Uncomment the next line to enable batching\n> Otherwise you request would have very little priotity\n";
	uncomment_article_commands = "> To order articles remove `-' from the first column of corresponding\n";
	and_send_list_back = "> lines and send the list back to %s%s.\n";
	you_send_nothing  = "> You have send nothing during %d days\n> System will forget you after %d days\n> Send any mail to server to keep subscriptions\n";
	rest_in_pease     = "> You are too silent. I have to forget you!\n> Rest In Pease, dear `%s'!\n>  !\n>      -+-\n>       !\n>      /^\\\n>      RIP\n";

	/* newnews.c */

	batch_of_articles = "Batch of Usenet articles";
}

static setrussian ()
{
	/* server.c */

	permission_denied = "Вам нельзя\n";
	server_reply = "ответ сервера USENET";
	news_server = "Сервер USENET, версия %s.%d\n(C) 1991-1992 Сергей Вакуленко\nМодификация 1993 Алексей Руднев\n";
	to_be_continued = "-------- Продолжение следует --------\n";

	/* reader.c */

	command_not_implemented = "Команда пока не реализована";
	too_long_line = "Слишком длинная строка";
	too_many_arguments = "Слишком много аргументов";

	/* commands.c */

	no_commands_entered = "Не было введено ни одной команды";
	no_help_available = "Нет информации";
	group_not_found = "Группа %s не найдена\n";
	no_group_selected = "Не выбрана группа\n";
	no_such_group = "Нет такой группы '%s'\n";
	you_are_not_subscribed = "Вы не подписаны на этом сервере\n";
	you_are_not_subscribed_to_group = "Вы не подписаны на группу %s\n";
	no_such_article = "Нет такой статьи\n";
	user_unsubscribed_from_group = "Подписка абонента %s на группу %s отменена\n";
	user_subscribed_to_group = "Абонент %s подключен к группе %s\n";
	in_feed_mode = "в режиме подписки\n";
	in_notify_mode = "в режиме уведомления\n";
	in_rfeed_mode = "в режиме ограниченной подписки\n";
	pack_turned_on = "Включен режим упаковки\n";
	pack_turned_off = "Режим упаковки выключен\n";
	aging_turned_on = "Включено старение подписки\n";
	aging_turned_off = "Старение подписки выключено\n";
	no_such_package = "Нет такого пакета %s\n";
	user_removed_from_list = "Абонент %s отключен от сервера\n";
	you_are_not_registered = "Вы не зарегистрированы на этом сервере\n";
	you_are_unknown = "Вы (%s) мне неизвестны. Возможно, ваш адрес изменился.\n";
	short_list_of_news_groups="Урезанный список групп. Подробный можно получить по команде\nlist Имя_группы_или_иерархии\n\n\n";
	group_dont_allow="Группа %s вам не разрешена\n";

	/* submit.c */

	cannot_post_article = "Ошибка при посылке статьи!\n";
	unsent_message_follows = "---------------- текст статьи ----------------\n";
	groups_cannot_post = "Нельзя послать статью в телеконференции:\n";
	dont_exist = "не существует";
	no_moderator = "нет модератора";
	closed_for_posting = "закрыта";
	bad_newsgroups_field = "Плохая строка заголовка Newsgroups: %s\n";

	/* notify.c */

	new_groups_created = "Созданы новые группы:\n";
	groups_deleted = "Удалены группы:";
	list_of_articles = "Список новых статей Usenet";
	info_from_server = "Информация от сервера Usenet";
	uncomment_this = "> Чтобы включить упаковку, раскомментируйте след. строку\n> Без упаковки ваш заказ может быть выполнет очень нескоро\n";
	uncomment_article_commands = "> Чтобы заказать статьи, удалите знак `-' из первой колонки соответствующих\n";
	and_send_list_back = "> строк и пошлите список обратно на %s%s.\n> Удалите также все ненужные строки, кроме команд GROUPS и ART номер_статьи. \n> Желательно удалить в командах ART все кроме номера статьи\n";
	you_send_nothing  = "> Вы ничего не посылали серверу в течении %d дней\n> Через %d дней сервер о вас забудет.\n> Пошлите ему хотя бы пустое письмо для сохранения подписки\n";
	rest_in_pease     = "> Поскольку вы молчите, сервер забывает о вас. \n> Покойся в мире, дорогой `%s'!\n>       !\n>      -+-\n>       !\n>      /^\\\n>      RIP\n";

	/* newnews.c */

	batch_of_articles = "Пакет статей Usenet";
}

setlang (l)
{
	if (l == lang)
		return;
	switch (l) {
	case 'l':
		l = 'e';
	case 'e':
		setenglish ();
		break;
	case 'r':
		setrussian ();
		break;
	}
	lang = l;
}
