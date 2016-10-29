/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

char *server_version;
char *server_copyright;

char *already_subscribed;
char *archive_of_lot;
char *archive_sent_to_you;
char *bad_add_command;
char *bad_create_command;
char *bad_delete_command;
char *bad_get_command;
char *bad_newindex_command;
char *bad_newstatus_command;
char *bad_send_command;
char *cannot_create;
char *cannot_create_file;
char *cannot_link_to;
char *cannot_write;
char *cannot_write_to;
char *command_not_implemented;
char *end_of_file;
char *lot_already_exist;
char *lot_created;
char *lot_destroyed;
char *lot_does_not_exist;
char *lot_is_closed;
char *lot_not_exist;
char *message_from;
char *message_sent_to;
char *messages_are;
char *no_access_to_server;
char *no_help_available;
char *no_information_available;
char *no_subscribers_available;
char *not_allowed_to_destroy;
char *not_allowed_to_get;
char *permission_denied;
char *server_reply;
char *subject_lot_expired;
char *subscribed_to;
char *subscribers_are;
char *time_range_is;
char *time_range_to;
char *too_long_line;
char *too_many_arguments;
char *unknown_command;
char *unsubscribed_from;
char *was_not_subscribed;
char *you_owner_of_lot;
char *your_lot_expired;

static seteng ()
{
	server_version = "Infosoft Mail Server, Version 1.3";
	server_copyright = "Copyright (C) 1991 Serge Vakulenko";

	already_subscribed = "Addressee %s is already subscribed to %s\n";
	archive_of_lot = "Archive of lot";
	archive_sent_to_you = "Archive of lot %s sent to you by separate letter\n";
	bad_add_command = "Bad ADD command";
	bad_create_command = "Bad CREATE command";
	bad_delete_command = "Bad DELETE command";
	bad_get_command = "Bad GET command";
	bad_newindex_command = "Bad NEWINDEX command";
	bad_newstatus_command = "Bad NEWSTATUS command";
	bad_send_command = "Bad SEND command";
	cannot_create = "Cannot create %s";
	cannot_create_file = "Cannot create %s\n";
	cannot_link_to = "Cannot link %s to %s\n";
	cannot_write = "Cannot write to %s\n";
	cannot_write_to = "Cannot write to %s";
	command_not_implemented = "Command is not implemented yet";
	end_of_file = "End of file";
	lot_already_exist = "Lot %s already exist";
	lot_created = "Lot %s created\n";
	lot_destroyed = "Lot %s destroyed\n";
	lot_does_not_exist = "Lot %s does not exist\n";
	lot_is_closed = "Lot %s is closed";
	lot_not_exist = "Lot %s does not exist";
	message_from = "Message from %s, lot %s:\n";
	message_sent_to = "Message sent to %s\n";
	messages_are = "Messages:\n";
	no_access_to_server = "Adressee %s does not have access to server";
	no_help_available = "No help available";
	no_information_available = ": No information available\n";
	no_subscribers_available = ": No subscribers available\n";
	not_allowed_to_destroy = "You are not allowed to destroy lot %s\n";
	not_allowed_to_get = "You are not allowed to get archive of lot %s\n";
	permission_denied = "Permission denied\n";
	server_reply = "Server Reply";
	subscribed_to = "Addressee %s subscribed to %s\n";
	subscribers_are = "Subscribers:\n";
	time_range_is = "Time range is %s ";
	time_range_to = "to %s\n";
	too_long_line = "Too long line";
	too_many_arguments = "Too many arguments";
	unknown_command = "Unknown command";
	unsubscribed_from = "Addressee %s unsubscribed from %s\n";
	was_not_subscribed = "Addressee %s was not subscribed to %s\n";
	you_owner_of_lot = "You are the owner of lot %s\n";
	your_lot_expired = "Your lot %s automatically deleted by Trade server.\n";
	subject_lot_expired = "Your lot %s expired";
}

static setrus ()
{
	server_version = "Почтовый сервер Infosoft, версия 1.3";
	server_copyright = "Copyright (C) 1991 Сергей Вакуленко";

	already_subscribed = "Адресат %s уже подписан на %s\n";
	archive_of_lot = "Архив лота";
	archive_sent_to_you = "Архив лота %s выслан Вам отдельным письмом\n";
	bad_add_command = "Неверная команда ADD";
	bad_create_command = "Неверная команда CREATE";
	bad_delete_command = "Неверная команда DELETE";
	bad_get_command = "Неверная команда GET";
	bad_newindex_command = "Неверная команда NEWINDEX";
	bad_newstatus_command = "Неверная команда NEWSTATUS";
	bad_send_command = "Неверная команда SEND";
	cannot_create = "Не могу создать %s";
	cannot_create_file = "Не могу создать %s\n";
	cannot_link_to = "Не могу связать %s с %s\n";
	cannot_write = "Не могу записать в %s\n";
	cannot_write_to = "Не могу записать в %s";
	command_not_implemented = "Команда пока не реализована";
	end_of_file = "Конец файла";
	lot_already_exist = "Лот %s уже существует";
	lot_created = "Создан лот %s\n";
	lot_destroyed = "Уничтожен лот %s\n";
	lot_does_not_exist = "Лот %s не существует\n";
	lot_is_closed = "Лот %s закрыт";
	lot_not_exist = "Лот %s не существует";
	message_from = "Сообщение от %s по лоту %s:\n";
	message_sent_to = "Сообщение отослано по адресу %s\n";
	messages_are = "Сообщения:\n";
	no_access_to_server = "Адресат %s не имеет права доступа к серверу";
	no_help_available = "Нет информации";
	no_information_available = ": Нет информации\n";
	no_subscribers_available = ": Нет подписчиков\n";
	not_allowed_to_destroy = "Вы не имеете права уничтожать лот %s\n";
	not_allowed_to_get = "Вы не имеете права брать архив лота %s\n";
	permission_denied = "Вам нельзя\n";
	server_reply = "Ответ сервера";
	subscribed_to = "Адресат %s подписан на %s\n";
	subscribers_are = "Подписчики:\n";
	time_range_is = "Временной диапазон от %s ";
	time_range_to = "до %s\n";
	too_long_line = "Слишком длинная строка";
	too_many_arguments = "Слишком много аргументов";
	unknown_command = "Неверная команда";
	unsubscribed_from = "Адресат %s снят с подписки на %s\n";
	was_not_subscribed = "Адресат %s не был подписан на %s\n";
	you_owner_of_lot = "Вы владелец лота %s\n";
	your_lot_expired = "Ваш лот %s автоматически уничтожен сервером.\n";
	subject_lot_expired = "Срок хранения Вашего лота %s истек";
}

setlang (lang)
{
	if (lang == 'r')
		setrus ();
	else
		seteng ();
}
