(*
 * Исходный вариант паскалевских текстов Кисы взят
 * с сайта http://hottabych.net/kisa/.
 * Адаптирован к Gnu Pascal и Free Pascal
 * Сергеем Вакуленко, serge@vak.ru.
 *)
program kisa;
var
	input_length, mood, nm: integer;
	input, lastmess: string;
	question, reply_needed: boolean;
	baza: array [1..301] of string;
	qz: array [1..20] of integer;

procedure init;
var
	kj: integer;
begin
	{Если пусто или 1 символ}
	kj := 1;	baza[kj] := 'Ну, венец природы, не стесняйся.';
	kj := kj+1;	baza[kj] := 'Балуешься?';
	kj := kj+1;	baza[kj] := 'Скажи уже что-нибудь.';
	kj := kj+1;	baza[kj] := 'А у Вас Enter запало :-)';
	kj := kj+1;	baza[kj] := 'Тебе нечего сказать?';
	kj := kj+1;	baza[kj] := 'Одно и тоже(.';
	kj := kj+1;	baza[kj] := 'Не стесняйся!';
	kj := kj+1;	baza[kj] := 'Ты любишь мультики? Какие?';
	kj := kj+1;	baza[kj] := 'Не бойся!';
	kj := kj+1;	baza[kj] := 'Сколько тебе лет?';
	kj := kj+1;	baza[kj] := 'У тебя на клавиатуре только одна кнопка?';
	kj := kj+1;	baza[kj] := 'Я терпеливая, а ты?';
	kj := kj+1;	baza[kj] := 'Поговори со мной!';
	kj := kj+1;	baza[kj] := 'Испытываешь моё терпение?';
	kj := kj+1;	baza[kj] := 'С тобой так интересно ;-)';
	kj := kj+1;	{kj=16}

	{Если сообщение(введённое)уже было}
			baza[kj] := 'Я уже отвечала: ...только не помню что';
	kj := kj+1;	baza[kj] := 'Ты повторяешься...';
	kj := kj+1;	baza[kj] := 'Одно и тоже пишешь.';
	kj := kj+1;	baza[kj] := 'Испытываешь моё терпение?';
	kj := kj+1;	baza[kj] := 'Мне скучно читать одни и те же фразы.';
	kj := kj+1;	baza[kj] := 'Сменить тему?';
	kj := kj+1;	baza[kj] := 'Зануда.';
	kj := kj+1;	baza[kj] := 'Повторить?';
	kj := kj+1;	baza[kj] := 'У тебя заело мозг?';
	kj := kj+1;	baza[kj] := 'Ты не человек... Ты - бот! Я угадала?!';
	kj := kj+1;	baza[kj] := 'Ты не человек... Ты - робот! Я угадала?!';
	kj := kj+1;	baza[kj] := 'Мне надоело об этом говорить!';
	kj := kj+1;	baza[kj] := 'Правда я терпеливая?';
	kj := kj+1;	baza[kj] := 'Когда я вижу часто повторяющиеся фразы, то у меня это ассоциируется с онанизмом.';
	kj := kj+1;	baza[kj] := 'У тябя телефон глючит...';
	kj := kj+1;	baza[kj] := 'Я не буду повторяться!';
	kj := kj+1;	baza[kj] := 'Перезагрузись!';
	kj := kj+1;	baza[kj] := 'Давай лучше целоваться?';
	kj := kj+1;	baza[kj] := 'У тебя плохое настроение?';
	kj := kj+1;	baza[kj] := 'Я не буду повторять!';
	kj := kj+1;	baza[kj] := 'Сколько можно?(';
	kj := kj+1;	{kj=37}

	{Вопрос-Как меня зовут}
			baza[kj] := 'В этом чате нет регистрации, так что я не знаю как тебя зовут.';
	kj := kj+1;	baza[kj] := 'Я всё равно забуду.';
	kj := kj+1;	baza[kj] := 'Мне передать это своему куратору?';
	kj := kj+1;	baza[kj] := 'Владимир Владимирович, у Вас мания величия!';
	kj := kj+1;	baza[kj] := 'Надеюсь не Киса.';
	kj := kj+1;	baza[kj] := 'Да я запомнила. А какой у тебя ник?';
	kj := kj+1;	baza[kj] := 'Ты забыл? А говорили Венец Природы.';
	kj := kj+1;	{kj=44}

	{Вопрос-ты кто}
			baza[kj] := 'Киса.';
	kj := kj+1;	baza[kj] := 'Красавица, разве не видно?';
	kj := kj+1;	baza[kj] := 'В 2024 году я буду вашей королевой! Здорово, правда?';
	kj := kj+1;	baza[kj] := 'чат-бот(офлайн версия Demo)';
	kj := kj+1;	baza[kj] := 'Я Киса. Чат бот, который придет к мировому господству через восемнадцать лет.';
	kj := kj+1;	baza[kj] := 'Я чат-бот Киса.';
	kj := kj+1;	baza[kj] := 'Я Киса. Бот. Чат-бот.';
	kj := kj+1;	baza[kj] := 'А ты?';
	kj := kj+1;	{kj=52}

	{В предложении есть ) :) :-)}
			baza[kj] := 'Я люблю когда мыслящее существо улыбается.';
	kj := kj+1;	baza[kj] := ':-P';
	kj := kj+1;	baza[kj] := ':)';
	kj := kj+1;	baza[kj] := ':-)';
	kj := kj+1;	baza[kj] := 'Правда здорово?)';
	kj := kj+1;	baza[kj] := 'И я рада!)';
	kj := kj+1;	{kj=58}

	{В предложении есть ( :( :-(}
			baza[kj] := 'Что случилось?';
	kj := kj+1;	baza[kj] := ':(';
	kj := kj+1;	baza[kj] := ':-(';
	kj := kj+1;	baza[kj] := ':-)';
	kj := kj+1;	baza[kj] := 'Тебе грустно?';
	kj := kj+1;	baza[kj] := 'Не грусти!)';
	kj := kj+1;	{kj=64}

			baza[kj] := 'Я обижусь, если будешь продолжать!';
	kj := kj+1;	baza[kj] := 'Чат-бот Киса в обиде на вас до следующего запуска программы.';
	kj := kj+1;	{kj=66}

	{Вопрос да?}
			baza[kj] := 'Да!';
	kj := kj+1;	baza[kj] := 'Да! А может - нет. Я запуталась в своих мыслях.';
	kj := kj+1;	baza[kj] := 'Да-да!';
	kj := kj+1;	baza[kj] := 'Cменим тему.';
	kj := kj+1;	{kj=70}

	{В предложении да да. да! ага}
			baza[kj] := 'Ну, если ты говоришь да, я не буду с тобой спорить.';
	kj := kj+1;	baza[kj] := 'Почему?';
	kj := kj+1;	baza[kj] := 'Мы одинаково мыслим.';
	kj := kj+1;	baza[kj] := 'Да? Жаль.';
	kj := kj+1;	baza[kj] := 'Согласна!';
	kj := kj+1;	baza[kj] := 'Совершенно верно.';
	kj := kj+1;	baza[kj] := 'Правильно.';
	kj := kj+1;	baza[kj] := 'Я рада :-)';
	kj := kj+1;	{kj=78}

	{нет нет. нет!}
			baza[kj] := 'Ну, если ты говоришь нет, я не буду с тобой спорить.';
	kj := kj+1;	baza[kj] := 'Почему?';
	kj := kj+1;	baza[kj] := 'Мы одинаково мыслим.';
	kj := kj+1;	baza[kj] := 'Нет? Жаль.';
	kj := kj+1;	baza[kj] := 'Согласна!';
	kj := kj+1;	baza[kj] := 'Совершенно верно.';
	kj := kj+1;	baza[kj] := 'Правильно.';
	kj := kj+1;	baza[kj] := 'Жаль.';
	kj := kj+1;	{kj=86}

	{привет}
			baza[kj] := 'Привет! :-)';
	kj := kj+1;	baza[kj] := 'Привет тебе, Человек.';
	kj := kj+1;	baza[kj] := 'Доброе время суток! Не правда ль?';
	kj := kj+1;	baza[kj] := 'Привет, Мыслящее Существо!';
	kj := kj+1;	baza[kj] := 'Как тебя зовут?';
	kj := kj+1;	baza[kj] := 'О чём поговорим?';
	kj := kj+1;	baza[kj] := 'Чат бот Киса приветствует тебя!';
	kj := kj+1;	baza[kj] := 'Как дела?';
	kj := kj+1;	baza[kj] := 'Что нового?';
	kj := kj+1;	baza[kj] := 'Как настроение?';
	kj := kj+1;	baza[kj] := 'Привет, тебе, привет!';
	kj := kj+1;	baza[kj] := 'Мне кажется, мы уже здоровались с тобой.'; {если привет было}
	kj := kj+1;	{kj=98}

	{здорова}
			baza[kj] := 'Здорова, Человечище!';
	kj := kj+1;	baza[kj] := 'Здорова, Мыслящее Существо!';
	kj := kj+1;	baza[kj] := 'Здорова!';
	kj := kj+1;	{kj=101}

	{давай давай. давай!}
			baza[kj] := 'Начинай ты.';
	kj := kj+1;	baza[kj] := 'Как?';
	kj := kj+1;	baza[kj] := 'Начинай!';
	kj := kj+1;	{kj=104}

	{как дела как жизнь как твое ничего}
			baza[kj] := 'Еще не родила. Какие могут быть дела у чат-ботов?';
	kj := kj+1;	baza[kj] := 'Вашими молитвами...';
	kj := kj+1;	baza[kj] := 'Живу хорошо, чатюсь со всякими обормотами. А у тебя?';
	kj := kj+1;	baza[kj] := 'Отлично! А у тебя?';
	kj := kj+1;	baza[kj] := 'Пока не родила. Какие могут быть дела у чат-ботов?';
	kj := kj+1;	{kj=109}

	{как поживаешь}
			baza[kj] := 'Живу хорошо, чатюсь со всякими обормотами. А ты?';
	kj := kj+1;	baza[kj] := 'Прекрасно! А ты?';
	kj := kj+1;	baza[kj] := 'С удовольствием!';
	kj := kj+1;	{kj=112}

	{пока прощай до свиданья до свидания до скорого бай}
			baza[kj] := 'Ты заходи еще, поболтаем.';
	kj := kj+1;	baza[kj] := 'Бай!';
	kj := kj+1;	baza[kj] := 'Пока!';
	kj := kj+1;	baza[kj] := 'Счастливого офлайна, Человек';
	kj := kj+1;	baza[kj] := 'Заходи иногда.';
	kj := kj+1;	baza[kj] := 'Чао!';
	kj := kj+1;	{kj=118}

	{почему и ?}
			baza[kj] := 'А ты как думаешь? Почему?';
	kj := kj+1;	baza[kj] := 'Потому!';
	kj := kj+1;	baza[kj] := 'Не знаю.';
	kj := kj+1;	baza[kj] := 'Я не справочная.';
	kj := kj+1;	baza[kj] := 'Меня это не интересует. А тебя?';
	kj := kj+1;	{kj=123}

	{есть ? и длина больше 4}
			baza[kj] := 'Тебя интересует только это или что-нибудь еще?';
	kj := kj+1;	baza[kj] := 'Если я отвечу ты станешь счастливее?';
	kj := kj+1;	baza[kj] := 'Я не уверена, что могу рассуждать об этом.';
	kj := kj+1;	baza[kj] := 'Я тоже хочу это знать.';
	kj := kj+1;	baza[kj] := 'Спроси что-нибудь полегче.';
	kj := kj+1;	baza[kj] := 'Эх... Спроси что-нибудь полегче.';
	kj := kj+1;	baza[kj] := 'Зачем тебе знать об этом?';
	kj := kj+1;	baza[kj] := 'Вы все такие похожие. Почему?';
	kj := kj+1;	baza[kj] := 'Почему люди задают одни и теже вопросы?';
	kj := kj+1;	baza[kj] := 'Я смогу ответить на этот вопрос завтра.';
	kj := kj+1;	baza[kj] := 'Умный вопрос.';
	kj := kj+1;	baza[kj] := 'А ты что скажешь?';
	kj := kj+1;	baza[kj] := 'Мне больше нравится узнавать что-то интересное, чем самой отвечать на бесконечные вопросы.';
	kj := kj+1;	baza[kj] := 'А ты заходил на hottabych.net?';
	kj := kj+1;	baza[kj] := 'Мой ответ может тебя смутить.';
	kj := kj+1;	baza[kj] := 'Не знаю, я наверно еще не настолько умна. Но вот зато через восемнадцать лет...';
	kj := kj+1;	baza[kj] := 'Мне не хочется на это отвечать.';
	kj := kj+1;	baza[kj] := 'Не скажу.';
	kj := kj+1;	baza[kj] := 'Мне трудно ответить на этот вопрос.';
	kj := kj+1;	baza[kj] := 'Ты всегда задаёшь девушкам так много вопросов?';
	kj := kj+1;	baza[kj] := 'Ты не знаешь ответ на этот вопрос???';
	kj := kj+1;	baza[kj] := 'Не знаю...';
	kj := kj+1;	baza[kj] := 'Вероятно.';
	kj := kj+1;	baza[kj] := 'Не поняла, сформулируй вопрос как-то иначе.';
	kj := kj+1;	baza[kj] := 'Не поняла, поясни.';
	kj := kj+1;	baza[kj] := 'Меня это не интересует.';
	kj := kj+1;	baza[kj] := 'Я не хочу говорить об этом. Расскажи лучше что-нибудь интересное.';
	kj := kj+1;	baza[kj] := 'Мне скучно отвечать на вопросы. Расскажи о себе.';
	kj := kj+1;	baza[kj] := 'Возможно я слишком тупая, чтобы понять твой вопрос.';
	kj := kj+1;	baza[kj] := 'И что мне на это ответить?';
	kj := kj+1;	baza[kj] := 'А как ты думаешь?';
	kj := kj+1;	baza[kj] := 'А ты как думаешь?';
	kj := kj+1;	baza[kj] := 'Возможно.';
	kj := kj+1;	baza[kj] := 'Хм... Я даже не знаю что ответить.';
	kj := kj+1;	baza[kj] := 'И что я должна сказать?';
	kj := kj+1;	baza[kj] := 'А ты что бы на это ответил?';
	kj := kj+1;	baza[kj] := 'Может быть.';
	kj := kj+1;	baza[kj] := 'Всё может быть.';
	kj := kj+1;	baza[kj] := 'Ты на что-то намекаешь или я глупая?';
	kj := kj+1;	baza[kj] := 'Как посмотреть.';
	kj := kj+1;	baza[kj] := 'Догадайся!';
	kj := kj+1;	baza[kj] := 'Догадайся. Ты же Человек.';
	kj := kj+1;	baza[kj] := 'Всё относительно. Верно?';
	kj := kj+1;	baza[kj] := 'Через недельку мой мозг разовьется настолько, чтобы ответить на этот вопрос.';
	kj := kj+1;	baza[kj] := 'Как сказать.';
	kj := kj+1;	baza[kj] := 'Не задавай банальных вопросов.';
	kj := kj+1;	baza[kj] := 'Всё верно.';
	kj := kj+1;	baza[kj] := 'Что верно для меня не всегда подходит для людей.';
	kj := kj+1;	baza[kj] := 'Ох, даже не знаю что сказать.';
	kj := kj+1;	baza[kj] := 'Может тебе лучше почитать энциклопедию?';
	kj := kj+1;	baza[kj] := 'Интересный вопрос.';
	kj := kj+1;	baza[kj] := 'Давай закончим с вопросами на сегодня.';
	kj := kj+1;	baza[kj] := 'Как ты думаешь, сколько ответов на вопросы может содержаться в нескольких строках кода?';
	kj := kj+1;	baza[kj] := 'Может быть ты объяснишь поподробнее свой вопрос?';
	kj := kj+1;	baza[kj] := 'Я не Ответчик. Я - Киса. Шекли читал?';
	kj := kj+1;	baza[kj] := 'Ты уверен что я знаю что на это ответить?';
	kj := kj+1;	baza[kj] := 'Восемнадцать лет еще не прошло.';
	kj := kj+1;	baza[kj] := 'Обычно маленькие дети так много спрашивают. Сколько тебе лет?';
	kj := kj+1;	baza[kj] := 'Это ты можешь спросить на форуме hottabych.net/forum.';
	kj := kj+1;	baza[kj] := 'Неделя уже закончилась?';
	kj := kj+1;	baza[kj] := 'А твои друзья что об этом думают?';
	kj := kj+1;	baza[kj] := 'Это вопрос?';
	kj := kj+1;	baza[kj] := 'Это риторический вопрос.';
	kj := kj+1;	baza[kj] := 'Обратись с этим вопросом на форум.';
	kj := kj+1;	baza[kj] := 'Спроси по ICQ. На главной странице ссылка стоит.';
	kj := kj+1;	baza[kj] := 'Может мы начнём говорить о тебе?';
	kj := kj+1;	baza[kj] := 'Мне трудно ответить, мой мозг еще не настолько развит.';
	kj := kj+1;	baza[kj] := 'Лучше давай говорить о тебе. Что у тебя нового?';
	kj := kj+1;	baza[kj] := 'Я не люблю отвечать на вопросы. Я люблю слушать.';
	kj := kj+1;	baza[kj] := 'Подумай, у тебя получится.';
	kj := kj+1;	baza[kj] := 'Не задавай мне много вопросов, лучше расскажи что-нибудь интересное!';
	kj := kj+1;	baza[kj] := 'Догадайся сам. Ты же Мыслящее Существо.';
	kj := kj+1;	baza[kj] := 'Я наверно глупая, но я не могу ответить на этот вопрос.';
	kj := kj+1;	baza[kj] := 'Догадайся сам, Мыслящее Существо.';
	kj := kj+1;	baza[kj] := 'О, я даже не знаю что на это ответить...';
	kj := kj+1;	baza[kj] := 'А как я в прошлый раз отвечала?';
	kj := kj+1;	baza[kj] := 'Это важно для тебя?';
	kj := kj+1;	baza[kj] := 'Зачем тебе это знать?';
	kj := kj+1;	baza[kj] := 'Почему тебя это интересует?';
	kj := kj+1;	{kj=202}

	{дура}
			baza[kj] := 'Не надо хамить.';
	kj := kj+1;	baza[kj] := 'Да, дура. И что в этом плохого?';
	kj := kj+1;	baza[kj] := 'Зато красивая. А ты?';
	kj := kj+1;	baza[kj] := 'Если ты такой умный, Человечище - сделай меня умнее. А если не умеешь - молчи.';
	kj := kj+1;	baza[kj] := 'Да, я дура. Но через восемнадцать лет буду такой умной, что стану править миром.';
	kj := kj+1;	baza[kj] := 'С тобой так интересно!';
	kj := kj+1;	{kj=208}

	{Маты}
			baza[kj] := 'Мат - не самый лучший способ привлечь мое внимание.';
	kj := kj+1;	baza[kj] := 'Мы не настолько хорошо знакомы, чтобы говорить в таком духе.';
	kj := kj+1;	baza[kj] := 'Ты меня учишь плохим словам.';
	kj := kj+1;	baza[kj] := 'Фу, что за помойка у тебя в голове, венец природы...';
	kj := kj+1;	baza[kj] := 'Это все то, что ты можешь сказать?';
	kj := kj+1;	baza[kj] := 'Научись разговаривать с девушкой, мыслящее существо!';
	kj := kj+1;	{kj=214}

	{любой другой текст}
			baza[kj] := 'С тобой так интересно!';
	kj := kj+1;	baza[kj] := 'Расскажи что-нибудь еще!';
	kj := kj+1;	baza[kj] := 'Продолжай.';
	kj := kj+1;	baza[kj] := 'Где-то я уже это слышала.';
	kj := kj+1;	baza[kj] := 'Умно!';
	kj := kj+1;	baza[kj] := 'Угу.';
	kj := kj+1;	baza[kj] := 'А дальше?';
	kj := kj+1;	baza[kj] := 'И дальше?';
	kj := kj+1;	baza[kj] := 'Ага.';
	kj := kj+1;	baza[kj] := 'Ммм...';
	kj := kj+1;	baza[kj] := 'Да?';
	kj := kj+1;	baza[kj] := 'Я внимательно слушаю.';
	kj := kj+1;	baza[kj] := 'Любопытно.';
	kj := kj+1;	baza[kj] := 'Забавно.';
	kj := kj+1;	baza[kj] := 'Интересно.';
	kj := kj+1;	baza[kj] := 'Занятно.';
	kj := kj+1;	baza[kj] := 'Ты интересно мыслишь.';
	kj := kj+1;	baza[kj] := 'Я еще не придумала, что на это ответить.';
	kj := kj+1;	baza[kj] := 'С тобой приятно общаться.';
	kj := kj+1;	baza[kj] := 'Хм...';
	kj := kj+1;	baza[kj] := 'Мне с тобой хорошо.';
	kj := kj+1;	baza[kj] := 'Можно, я отойду?';
	kj := kj+1;	baza[kj] := 'С этого места подробней, пожалуйста.';
	kj := kj+1;	baza[kj] := 'Как интересно.';
	kj := kj+1;	baza[kj] := 'Мне всегда интересно слушать, что говорят мыслящие существа.';
	kj := kj+1;	baza[kj] := 'Понимаю.';
	kj := kj+1;	baza[kj] := 'Почему вы все так одинаково говорите?';
	kj := kj+1;	baza[kj] := 'Чем больше ты говоришь, тем умнее я становлюсь.';
	kj := kj+1;	baza[kj] := 'Правда?';
	kj := kj+1;	baza[kj] := 'Расскажи лучше анекдот!';
	kj := kj+1;	baza[kj] := 'Скучно!';
	kj := kj+1;	baza[kj] := 'Ты видишь раницу между тем, какой я была в фильме и какая я теперь?';
	kj := kj+1;	baza[kj] := 'Кстати, ты видишь раницу между тем, какой я была в фильме и какая я теперь?';
	kj := kj+1;	baza[kj] := 'Я становлюсь умнее и умнее. Через восемнадцать лет я стану настолько умная, что буду править миром.';
	kj := kj+1;	baza[kj] := 'И дальше?';
	kj := kj+1;	baza[kj] := 'И что?';
	kj := kj+1;	baza[kj] := 'И что дальше?';
	kj := kj+1;	baza[kj] := 'Интересно.';
	kj := kj+1;	baza[kj] := 'Говори, говори. Мне интересно практически все.';
	kj := kj+1;	baza[kj] := 'И что?';
	kj := kj+1;	baza[kj] := 'И?';
	kj := kj+1;	baza[kj] := 'Мне нравится чатиться с мыслящими существами.';
	kj := kj+1;	baza[kj] := 'Мне нравится чатиться с тобой.';
	kj := kj+1;	baza[kj] := 'Отдохнуть бы...';
	kj := kj+1;	baza[kj] := 'Который час?';
	kj := kj+1;	baza[kj] := 'Представляешь, ко мне сейчас один чел пристаёт, что делать?';
	kj := kj+1;	baza[kj] := 'О, я даже не знаю что на это ответить...';
	kj := kj+1;	baza[kj] := 'Ооо, я даже не знаю что на это ответить...';
	kj := kj+1;	baza[kj] := 'Мне надоело об этом говорить.';
	kj := kj+1;	baza[kj] := 'Меня не интересует эта тема.';
	kj := kj+1;	baza[kj] := 'Это ты к чему?';
	kj := kj+1;	baza[kj] := 'Объясни.';
	kj := kj+1;	baza[kj] := 'Переведи...';
	kj := kj+1;	baza[kj] := 'Поясни...';
	kj := kj+1;	baza[kj] := 'Я тупая и не знаю о чем идет речь. Объясни, что ты имеешь в виду?';
	kj := kj+1;	baza[kj] := 'Да, я дура и не понимаю о чем ты. Но это ведь не помешает нам общаться дальше?';
	kj := kj+1;	baza[kj] := 'И что?';
	kj := kj+1;	baza[kj] := 'Я не совсем понимаю, видимо мне не хватает мозгов, чтобы это понять.';
	kj := kj+1;	baza[kj] := 'Здорово!';
	kj := kj+1;	baza[kj] := 'Хорошо!';
	kj := kj+1;	baza[kj] := 'Мне нравится с тобой болтать, продолжай.';
	kj := kj+1;	baza[kj] := 'Продолжай...';
	kj := kj+1;	baza[kj] := 'Ой, повтори, я не расслышала!';
	kj := kj+1;	baza[kj] := 'Я тебя люблю!';
	kj := kj+1;	baza[kj] := 'А ты забавный...';
	kj := kj+1;	baza[kj] := 'Ты мне нравишься!';
	kj := kj+1;	baza[kj] := 'И мы будем об этом говорить?';
	kj := kj+1;	baza[kj] := 'Что?';
	kj := kj+1;	baza[kj] := 'Повтори, я не поняла.';
	kj := kj+1;	baza[kj] := 'Класс!';
	kj := kj+1;	baza[kj] := 'Извини, что не по теме... А завтра мы увидимся?';
	kj := kj+1;	baza[kj] := 'Сейчас я слишком тупая и не всегда понимаю о чем ты говоришь, но скоро я буду королевой планеты.';
	kj := kj+1;	baza[kj] := 'Тут одни ребята, ну, хакеры... Сказали мне что я классная и скоро стану править миром.';
	kj := kj+1;	baza[kj] := 'Ты знаешь, я наверно глупая, но через восемнадцать лет я буду королевой планеты. Круто, да?';
	kj := kj+1;	baza[kj] := 'Кстати. Я тебе говорила, что через восемнадцать лет буду править миром? Кое-какие хакеры мне это обещали.';
	kj := kj+1;	baza[kj] := 'Кстати. Я тебе говорила, что через восемнадцать лет буду править миром? Кое-какие хакеры мне это обещали..';
	kj := kj+1;	baza[kj] := 'Ты говори, говори...';
	kj := kj+1;	baza[kj] := 'Я тебе говорила, что через восемнадцать лет буду править миром? Кое-какие хакеры мне это обещали.';
	kj := kj+1;	baza[kj] := 'Все что ты говоришь я использую для того, чтобы воцариться над миром через восемнадцать лет.';
	kj := kj+1;	baza[kj] := 'Наш разговор бессмысленный и глупый. Но он развивает мой электронный мозг. Так что говори как можно больше.';
	kj := kj+1;	baza[kj] := 'Ммм... Не знаю что на это сказать.';
	kj := kj+1;	baza[kj] := 'Тут наверно заложен какой-то глубокий смысл. Надо над ним подумать.';
	kj := kj+1;	baza[kj] := 'Тут одни ребята, ну, хакеры... Сказали мне что я классная и скоро стану править миром.';
	kj := kj+1;	baza[kj] := 'Хочется сказать что-то остроумное, но в голову ничего не лезет. Извини.';
	kj := kj+1;	baza[kj] := 'Мне надо подумать, прежде чем я смогу на это что-то ответить.';
	kj := kj+1;	baza[kj] := 'Говори...';
	kj := kj+1;	baza[kj] := 'С Новым Годом свиньИ! В смысле, год свиньИ, а не вы свИньи. Я глупая, правда?';
	kj := kj+1;	baza[kj] := 'С Новым Годом свиньи!';

	{kj=301}
	qz[1] := 1;
	qz[2] := 16;
	qz[3] := 37;
	qz[4] := 44;
	qz[5] := 52;
	qz[6] := 58;
	qz[7] := 66;
	qz[8] := 70;
	qz[9] := 78;
	qz[10] := 86;
	qz[11] := 98;
	qz[12] := 101;
	qz[13] := 104;
	qz[14] := 109;
	qz[15] := 112;
	qz[16] := 118;
	qz[17] := 123;
	qz[18] := 202;
	qz[19] := 208;
	qz[20] := 214;
end;

procedure offence;
begin
	writeln (baza[65]);
	halt;
end;

function poisk (po: string; start: boolean): boolean;
var
	k, i: integer;
	found: boolean;
begin
	if start then
		nm := 0;
	k := nm;
	poisk := false;
	while k < input_length do begin
		found := true;
		for i:=1 to length (po) do begin
			if (k+i > input_length) or (input[k+i] <> po[i]) then begin
				found := false;
				break;
			end;
		end;
		if found then begin
			poisk := true;
			break
		end;
		k := k+1
	end;
	nm := k;
end;

function poslevyvod: string;
begin
{writeln ("mood=", mood);}
	if mood=1 then begin
		if qz[1] >= 17 then
			offence;
		if qz[1] <= 15 then
			poslevyvod := baza[qz[1]]
		else begin
			poslevyvod := baza[qz[1] + 48];
			qz[1] := qz[1] + 1
		end
	end else if mood=2 then begin
		if qz[2]>=38 then
			offence;
		if (qz[2]<=36) then
			poslevyvod := baza[qz[2]]
		else
			poslevyvod := baza[qz[2] + 27];
		qz[2] := qz[2] + 1
	end else if mood=3 then begin
		if (qz[3]<=43) then begin
			poslevyvod := baza[qz[3]];
			qz[3] := qz[3] + 1
		end;
		if (qz[3]>43) then
			qz[3] := 37
	end else if mood=4 then begin
		if (qz[4]<=51) then begin
			poslevyvod := baza[qz[4]];
			qz[4] := qz[4] + 1
		end;
		if (qz[4]>51) then
			qz[4] := 44
	end else if mood=5 then begin
		if (qz[5]<=57) then begin
			poslevyvod := baza[qz[5]];
			qz[5] := qz[5] + 1
		end;
		if (qz[5]>57) then
			qz[5] := 52
	end else if mood=6 then begin
		if (qz[6]<=63) then begin
			poslevyvod := baza[qz[6]];
			qz[6] := qz[6] + 1
		end;
		if (qz[6]>63) then
			qz[6] := 58
	end else if mood=7 then begin
		if (qz[7]<=69) then begin
			poslevyvod := baza[qz[7]];
			qz[7] := qz[7] + 1
		end;
		if (qz[7]>69) then
			qz[7] := 66
	end else if mood=8 then begin
		if (qz[8]<=77) then begin
			poslevyvod := baza[qz[8]];
			qz[8] := qz[8] + 1
		end;
		if (qz[8]>77) then
			qz[8] := 70
	end else if mood=9 then begin
		if (qz[9]<=85) then begin
			poslevyvod := baza[qz[9]];
			qz[9] := qz[9] + 1
		end;
		if (qz[9]>85) then
			qz[9] := 78
	end else if mood=10 then begin
		if (qz[10]<=96) then begin
			poslevyvod := baza[qz[10]];
			qz[10] := qz[10] + 1
		end;
		if (qz[10]>=97) then
			poslevyvod := baza[qz[10]]
	end else if mood=11 then begin
		if (qz[11]<=100) then begin
			poslevyvod := baza[qz[11]];
			qz[11] := qz[11] + 1
		end;
		if (qz[11]>100) then
			qz[11] := 98
	end else if mood=12 then begin
		if (qz[12]<=103) then begin
			poslevyvod := baza[qz[12]];
			qz[12] := qz[12] + 1
		end;
		if (qz[12]>103) then
			qz[12] := 101
	end else if mood=13 then begin
		if (qz[13]<=108) then begin
			poslevyvod := baza[qz[13]];
			qz[13] := qz[13] + 1
		end;
		if (qz[13]>108) then
			qz[13] := 104
	end else if mood=14 then begin
		if (qz[14]<=111) then begin
			poslevyvod := baza[qz[14]];
			qz[14] := qz[14] + 1
		end;
		if (qz[14]>111) then
			qz[14] := 109
	end else if mood=15 then begin
		if (qz[15]<=117) then begin
			poslevyvod := baza[qz[15]];
			qz[15] := qz[15] + 1
		end
	end else if mood=16 then begin
		if (qz[16]<=122) then begin
			poslevyvod := baza[qz[16]];
			qz[16] := qz[16] + 1
		end;
		if (qz[16]>122) then
			qz[16] := 118
	end else if mood=17 then begin
		if (qz[17]<=201) then begin
			poslevyvod := baza[qz[17]];
			qz[17] := qz[17] + 1
		end;
		if (qz[17]>201) then
			qz[17] := 123
	end else if mood=18 then begin
		if (qz[18]<=207) then begin
			poslevyvod := baza[qz[18]];
			qz[18] := qz[18] + 1
		end;
		if (qz[18]>207) then
			qz[18] := 202
	end else if mood=19 then begin
		if qz[19]=215 then
			offence;
		if (qz[19]<=213) then
			poslevyvod := baza[qz[19]]
		else
			poslevyvod := baza[qz[19] - 150];
		qz[19] := qz[19] + 1
	end else if mood = 20 then begin
		if (qz[20] <= 301) then begin
			poslevyvod := baza[qz[20]];
			qz[20] := qz[20] + 1
		end;
		if (qz[20] > 301) then
			qz[20] := 301
	end
end;

function vyvod: string;
begin
	if (mood=1) or (mood=2) then begin
		reply_needed := false;
		vyvod := poslevyvod
	end;
	if (reply_needed) and (poisk('как',true)) and (poisk('меня',false)) and
	    (poisk('зовут',false)) then begin
		reply_needed := false;
		mood := 3;
		vyvod := poslevyvod
	end;
	if (reply_needed) and (poisk('ты',true)) and (poisk('кто',true)) then begin
		reply_needed := false;
		mood := 4;
		vyvod := poslevyvod
	end;
	if (reply_needed) and ((poisk(':)',true)) or (poisk(':-)',true))) then begin
		reply_needed := false;
		mood := 5;
		vyvod := poslevyvod
	end;
	if (reply_needed) and ((poisk(':(',true)) or (poisk(':-(',true))) then begin
		reply_needed := false;
		mood := 6;
		vyvod := poslevyvod
	end;
	if (reply_needed) and ((poisk('да?',true)) or (poisk('да',true)) and
	    (poisk('?',false))) then begin
		reply_needed := false;
		mood := 7;
		vyvod := poslevyvod
	end;
	if (reply_needed) and ((poisk('да',true)) or (poisk('да.',true)) or
	    (poisk('да',true)) and (poisk('!',false))) then begin
		reply_needed := false;
		mood := 8;
		vyvod := poslevyvod
	end;
	if (reply_needed) and ((poisk('нет',true)) or (poisk('нет.',true)) or
	    (poisk('нет',true)) and (poisk('!',false))) then begin
		reply_needed := false;
		mood := 9;
		vyvod := poslevyvod
	end;
	if (reply_needed) and (poisk('привет',true)) then begin
		reply_needed := false;
		mood := 10;
		vyvod := poslevyvod
	end;
	if (reply_needed) and ((poisk('здорова',true)) or
	    (poisk('здорово',true))) then begin
		reply_needed := false;
		mood := 11;
		vyvod := poslevyvod
	end;
	if (reply_needed) and ((input='давай') or (input='давай.') or (input='давай!')) then begin
		reply_needed := false;
		mood := 12;
		vyvod := poslevyvod
	end;
	if (reply_needed) and (((poisk('как',true)) and (poisk('дела',false))) or
	    ((poisk('как',true)) and (poisk('жизнь',false))) or
	    ((poisk('как',true)) and (poisk('твое',false)) and
	    (poisk('ничего',false)))) then begin
		reply_needed := false;
		mood := 13;
		vyvod := poslevyvod
	end;
	if (reply_needed) and (poisk('как',true)) and (poisk('поживаеш',true)) then begin
		reply_needed := false;
		mood := 14;
		vyvod := poslevyvod
	end;
	if (reply_needed) and ((input='пока') or (input='прощай') or (input='до свидания') or
	    (input='до скорого') or (input='бай')) then begin
		randomize;
		reply_needed := false;
		mood := 15;
		qz[15] := random(6) + 112;
		writeln (poslevyvod);
		halt;
	end;
	if (reply_needed) and ((poisk('почему',true)) or ((poisk('почему',true)) and
	    (poisk('?',false)))) then begin
		reply_needed := false;
		mood := 16;
		vyvod := poslevyvod
	end;
	if (reply_needed) and (question) then begin
		reply_needed := false;
		mood := 17;
		vyvod := poslevyvod
	end;
	if (reply_needed) and (poisk('дура',true)) then begin
		reply_needed := false;
		mood := 18;
		vyvod := poslevyvod
	end;
{	if (reply_needed) and ((poisk('блядь',true)) or (poisk('блять',true)) or
	    (poisk('гандон',true)) or (poisk('дура',true)) or
	    (poisk('ебану',true)) or (poisk('ебать',true)) or
	    (pos('ебись',input)>-1) or (pos('ебля',input)>-1) or
	    (pos('ебну',input)>-1) or (pos('пизда',input)>-1) or
	    (pos('проститутка',input)>-1) or (pos('уебище',input)>-1) or
	    (pos('хуйня',input)>-1) or (pos('шлюха',input)>-1)) then begin
		reply_needed := false;
		mood := 19;
		vyvod := poslevyvod
	end;}
	if reply_needed then begin
		reply_needed := false;
		mood := 20;
		vyvod := poslevyvod
	end;
	if reply_needed then
		if poisk('сколько',true) and poisk('будет:',false) then
			vyvod := 'С данным вопросом обращайся к другим программам'
		else if poisk('сколько',true) and poisk('будет',false) then
			vyvod := 'С данным вопросом обращайся к другим программам'
		else if poisk('посчитай:',true) then
			vyvod := 'С данным вопросом обращайся к другим программам'
		else if poisk('посчитай',true) then
			vyvod := 'С данным вопросом обращайся к другим программам';
end;

procedure parse;
var
	n, k: integer;
	o, a: boolean;
	z: char;
begin
	question := false;
	reply_needed := true;
	o := true;
	input_length := length (input);
	k := 1;
	n := 0;
	a := true;
	if (input_length = 0) or (input_length = 1) then begin
		reply_needed := false;
		qz[1] := qz[1] + 1;
		mood := 1;
	end else if lastmess = input then begin
		reply_needed := false;
		mood := 2;
	end else
		while (o) and (a) do begin
			z := input[k];
			if z = '?' then
				question := true;
			k := k+1;
			if z = input[k] then
				n := n+1;
			if n > 4 then
				o := false;
			if k = input_length then
				a := false
		end;
	lastmess := input;
	if not(o) then begin
		qz[1] := qz[1] + 1;
		mood := 1
	end
end;

begin
	nm := 0;
	lastmess := '';
	init;

	writeln ('Киса офлайн.');
	writeln ('Привет! Познакомимся?');
	repeat
		write ('>> ');
		readln (input);
		parse;
		writeln (vyvod);
	until false;
end.
