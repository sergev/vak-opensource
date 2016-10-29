#include <QtGui>
#include <QtNetwork>
#include <stdint.h>
#include "diag.h"
#include "plotter.h"
#include "seismonet.h"

#define TITLE	"Seismonet Diag"

TopWindow::TopWindow (QWidget *parent)
	: QDialog (parent)
{
	/* Размещение элементов на основном окне делаем посредством QGridLayout. */
	QGridLayout *grid = new QGridLayout;
	int grid_row = 0;

	/* Создаём поле для ввода IP-адреса сейсмометра. */
	host_label = new QLabel (tr("IP-адрес устройства:"));
	hostLineEdit = new QLineEdit ("192.168.20.222");
	host_label->setBuddy (hostLineEdit);
	grid->addWidget (host_label, grid_row, 0);
	grid->addWidget (hostLineEdit, grid_row, 1, 1, 2);
	grid_row++;

	/* Создаём поле для ввода номера TCP-порта сейсмометра. */
	port_label = new QLabel (tr("Номер порта TCP:"));
	portLineEdit = new QLineEdit ("2222");
	portLineEdit->setValidator (new QIntValidator (1, 65535, this));
	port_label->setBuddy (portLineEdit);
	grid->addWidget (port_label, grid_row, 0);
	grid->addWidget (portLineEdit, grid_row, 1, 1, 2);
	grid_row++;

	/* Создаём поле для строки сообщений программы. */
	status_label = new QLabel (tr("Необходимо запустить устройство Seismonet XYZ-E10 "
		"на указанном адресе IP."));
	grid->addWidget (status_label, grid_row++, 0, 1, -1);

	/* Создаём ряд кнопок управления программой. */
	startButton = new QPushButton (tr("Старт"));
	startButton->setDefault (true);
	startButton->setEnabled (true);

	quitButton = new QPushButton (tr("Выход"));

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton (startButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton (quitButton, QDialogButtonBox::RejectRole);

	grid->addWidget (buttonBox, grid_row++, 0, 1, -1);

	/* Создаём поле для рисования графика. */
	plot = new Plotter;
	grid->addWidget (plot, grid_row++, 0, 1, -1);

	/* Создаём поля статистики. */
	stat_label[0] = new QLabel (tr("LAPB: передано пакетов:"));
	stat_label[2] = new QLabel (tr("Принято пакетов:"));
	stat_label[4] = new QLabel (tr("Потеряно пакетов:"));
	stat_label[6] = new QLabel (tr("Ошибки приёма:"));
	stat_label[8] = new QLabel (tr("Переполнения:"));
	stat_label[10] = new QLabel (tr("Ошибки кадров:"));
	stat_label[12] = new QLabel (tr("Ошибки CRC пакетов:"));
	stat_label[14] = new QLabel (tr("Ошибки CRC данных:"));
	stat_label[16] = new QLabel (tr("Прерывания Ethernet:"));

	stat_label[1] = new QLabel (tr("Местоположение:"));
	stat_label[3] = new QLabel (tr("Время жизни:"));
	stat_label[5] = new QLabel (tr("Частота процессора:"));
	stat_label[7] = new QLabel (tr("Свободная память:"));
	stat_label[9] = new QLabel (tr("Статус GPS:"));
	stat_label[11] = new QLabel (tr("Время UTC:"));
	stat_label[13] = new QLabel (tr("Дата:"));
	stat_label[15] = new QLabel (tr("Широта, долгота:"));
	stat_label[17] = new QLabel (tr("Высота:"));

	for (int i=0; i<NSTATS; i+=2) {
		stat_value[i] = new QLabel ("-");
		stat_value[i+1] = new QLabel ("-");
		stat_label[i]->setAlignment (Qt::AlignRight);
		stat_label[i+1]->setAlignment (Qt::AlignRight);
		grid->addWidget (stat_label[i], grid_row, 0);
		grid->addWidget (stat_value[i], grid_row, 1);
		grid->addWidget (stat_label[i+1], grid_row, 2);
		grid->addWidget (stat_value[i+1], grid_row, 3);
		grid_row++;
	}

	/* Создаём поле с текстом для сообщений программы. */
	text = new QTextEdit;
	text->setReadOnly (true);
	text->setPlainText ("");

	QFont font ("Courier New", -1, QFont::Normal);
	text->setFont (font);

	grid->addWidget (text, grid_row++, 0, 1, -1);

	/* Элементы главного окна готовы. */
	setLayout (grid);
	setWindowTitle (tr(TITLE));
	portLineEdit->setFocus();

	/* Подготавливаем сокет TCP. */
	tcpSocket = new QTcpSocket (this);
	data_crc_errors = 0;

	/* Привязываем события. */
	connect (hostLineEdit, SIGNAL (textChanged (const QString &)),
		this, SLOT (enable_start_button()));
	connect (portLineEdit, SIGNAL (textChanged (const QString &)),
		this, SLOT (enable_start_button()));
	connect (startButton, SIGNAL (clicked()),
		this, SLOT (request_connection()));
	connect (quitButton, SIGNAL (clicked()), this, SLOT (close()));
	connect (tcpSocket, SIGNAL (readyRead()), this, SLOT (read_data()));
	connect (tcpSocket, SIGNAL (error (QAbstractSocket::SocketError)),
		this, SLOT (display_error (QAbstractSocket::SocketError)));
}

void TopWindow::enable_start_button()
{
	startButton->setEnabled (! hostLineEdit->text().isEmpty() &&
		! portLineEdit->text().isEmpty());
}

void TopWindow::request_connection()
{
	startButton->setEnabled (false);
	tcpSocket->abort();
	ptr = (char*) data;
	data_len = 2;
	packets_received = 0;
	tcpSocket->connectToHost (hostLineEdit->text(),
		portLineEdit->text().toInt());
	status_label->setText (tr("Соединение с устройством..."));
	expected_filename = 0;
	expected_counter = 0;
}

void TopWindow::display_error (QAbstractSocket::SocketError socketError)
{
	status_label->setText ("");
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
		break;
	case QAbstractSocket::HostNotFoundError:
		QMessageBox::information (this, tr(TITLE),
			tr("Устройство не найдено."
			   "Проверьте установки адреса и номера порта."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::information(this, tr(TITLE),
			tr("Соединение отклонено устройством. "
			   "Проверьте установки адреса и номера порта."));
		break;
	default:
		QMessageBox::information(this, tr(TITLE),
			tr("Произошла ошибка: %1.")
				.arg (tcpSocket->errorString()));
		break;
	}
	startButton->setEnabled (true);
}

void TopWindow::read_data()
{
	QDataStream in (tcpSocket);
	in.setVersion (QDataStream::Qt_4_0);
again:
	/* Принимаем данные по TCP. */
	int n = in.readRawData (ptr, (char*) data + data_len - ptr);
	if (n <= 0)
		return;

	/* Заполняем блок данных. */
	ptr += n;
	if (ptr < (char*) data + data_len)
		return;
	if (data_len == 2) {
		if (! (data[0] & 0x80)) {
			QString message;
			message.sprintf ("Invalid packet header: %02x-%02x",
				data[0], data[1]);
			text->append (message);
			return;
		}
		data_len = (data[0] << 8 | data[1]) & 0x7fff;
		goto again;
	}

	++packets_received;
	QString message = tr("Получено пакетов: ");
	QTextStream (&message) << packets_received;
	status_label->setText (message);

	if (data_len == 514)
		process_data (data + 2);
	else if (data_len == 2 + sizeof (xyze10_stat_t))
		process_statistics ((xyze10_stat_t*) (data + 2));
	else {
		QString message;
		message.sprintf ("Invalid packet length: %u bytes", data_len);
		text->append (message);
	}
	ptr = (char*) data;
	data_len = 2;
}

/*
 * Calculate a new sum given the current sum and the new data.
 */
static unsigned crc16_bigendian (unsigned short sum,
	unsigned const char *buf, unsigned len)
{
	/* Checksum lookup table, from RFC-1662.
	 * CRC-CCITT = x^16 + x^12 + x^5 + 1
	 * MSB first. */
	static const unsigned poly_tab [256] = {
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
		0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
		0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
		0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
		0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
		0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
		0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
		0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
		0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
		0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
		0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
		0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
		0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
		0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
		0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
		0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
		0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
		0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
		0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
		0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
		0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
		0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
		0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
		0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
		0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
	};

	if (len) {
		do {
			sum = (sum << 8) ^ poly_tab [*buf++ ^ (sum >> 8)];
		} while (--len);
	}
	return sum;
}

void TopWindow::process_data (unsigned char *p)
{
	QString message;

	unsigned counter = p[0] << 8 | p[1];
	unsigned number = p[2] << 24 | p[3] << 16 | p[508] << 8 | p[509];
	unsigned label = number & 511;
	unsigned filename = (number >> 9) - counter;

	if (expected_counter > 0 && (filename != expected_filename ||
	    counter != expected_counter)) {
		//text->append ("---");
		plot->add_sample (0, 0, 512/9);
	}
	message.sprintf ("%4u, %2u, %3d, %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x...",
		filename, counter, label,
		p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12]);

	/* Проверяем контрольную сумму. */
	unsigned sum = crc16_bigendian (0, p, 512);
	if (sum != 0) {
		++data_crc_errors;
		stat_value[14]->setText (QString().sprintf ("%lu", data_crc_errors));
		QTextStream (&message) << tr(" - ошибка CRC");
		text->append (message);
		return;
	}

	if (counter == 0) {
		/* Печатаем заголовок файла. */
		//unsigned date = p[0xA] << 8 | p[0xB];
		double sec = 0.000025 * ((unsigned long) p[0xC] << 24 |
			(unsigned long) p[0xD] << 16 | p[0xE] << 8 | p[0xF]);
		QTextStream (&message) << tr(" - время ") << sec
			/*<< tr(", дата ") << date*/;
		if (label == 0xff)
			plot->draw_time (sec);
	} else {
		int n = 0;
		for (unsigned i=4; i<512-9; i+=9, n++) {
			qint32 z = p[i] << 24 | p[i+1] << 16 | p[i+2] << 8;
			//qint32 x = p[i+3] << 24 | p[i+4] << 16 | p[i+5] << 8;
			//qint32 y = p[i+6] << 24 | p[i+7] << 16 | p[i+8] << 8;
			double percent = z;
#define MAXVAL 33000
			if (z >= MAXVAL)
				percent = 100;
			else if (z <= -MAXVAL)
				percent = -100;
			else
				percent *= 100.0 / MAXVAL;

			plot->add_sample ((int) percent, n, 512/9);
			if (i == label)
				plot->draw_label (n, 512/9);
		}
	}
	text->append (message);

	if (label == 0xff) {
		expected_filename = filename + 1;
		expected_counter = 0;
	} else {
		expected_filename = filename;
		expected_counter = (counter == 32) ? 0 : counter+1;
	}
}

void TopWindow::print_data (const char *title, unsigned char *data, unsigned len)
{
	QString message;
	message.sprintf ("%s: %02x", title, data[0]);
	unsigned i;
	for (i=1; i<len; ++i)
		message.append (QString().sprintf ("-%02x", data[i]));
	text->append (message);
}

/*
 * Преобразование времени (секунды от 00:00:00 1.1.70)
 * в текстовое представление.  Возвращается указатель
 * на внутренний statiс буфер.
 * Представление времени: dd.mm.yy/hh:mm:ss
 */
void TopWindow::show_time_date (unsigned long seconds, unsigned week)
{
	unsigned hms, sec, min, hour, year, month, day, dysize;
	static const unsigned char days_per_month [24] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
		31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
	};
	const unsigned char *dmsize = days_per_month;

	day = seconds / 86400L;
	hms = seconds - day * 86400L;
	day += week * 7 + 5;

	sec = hms % 60;
	hms /= 60;
	min = hms % 60;
	hour = hms / 60;

	/* TODO: сделать началом не 1980-й, а 2010-й год. */
	for (year=1980; ; ++year) {
#define LEAP_YEAR(x)	((x & 3) == 0)
		dysize = LEAP_YEAR(year) ? 366 : 365;
		if (day < dysize)
			break;
		day -= dysize;
	}

	if (LEAP_YEAR (year))
		dmsize += 12;
	for (month=0; day >= dmsize[month]; month++)
		day -= dmsize[month];

	QString message;
	message.sprintf ("%02d.%02d.%02d", day+1, month+1, year);
	stat_value[13]->setText (message);

	message.sprintf ("%02d:%02d:%02d", hour, min, sec);
	stat_value[11]->setText (message);
}

/*
 * Преобразование 32-битного вещественного числа в длинное целое.
 * Исходный массив содержит 4 байта числа (старшим байтом вперёд)
 * в формате IEEE 754 single precision.
 */
//static float fetch_float (const unsigned char *p)
float TopWindow::fetch_float (const unsigned char *p)
{
	union {
		qint32 i;
		float f;
	} val;

	val.i = qFromBigEndian (*(qint32*) p);
	//text->append (QString().sprintf ("float %08x -> %08x", *(qint32*) p, val.i));
	return val.f;
}

/*
 * Преобразование 64-битного вещественного числа в длинное целое.
 * Исходный массив содержит 8 байт числа (старшим байтом вперёд)
 * в формате IEEE 754 double precision.
 */
//static double fetch_double (const unsigned char *p)
double TopWindow::fetch_double (const unsigned char *p)
{
	union {
		qint64 i;
		double d;
	} val;

	val.i = qFromBigEndian (*(qint64*) p);
	//text->append (QString().sprintf ("double %016llx -> %016llx", *(qint64*) p, val.i));
	return val.d;
}

void TopWindow::process_statistics (xyze10_stat_t *st)
{
	/* Статистика LAPB. */
	stat_value[0]->setText (QString().sprintf ("%lu", st->lapb_out_packets));
	stat_value[2]->setText (QString().sprintf ("%lu", st->lapb_in_packets));
	stat_value[4]->setText (QString().sprintf ("%lu", st->lapb_in_discards));
	stat_value[6]->setText (QString().sprintf ("%lu", st->lapb_in_errors));
	stat_value[8]->setText (QString().sprintf ("%lu", st->lapb_in_overruns));
	stat_value[10]->setText (QString().sprintf ("%lu", st->lapb_in_framing_errors));
	stat_value[12]->setText (QString().sprintf ("%lu", st->lapb_in_crc_errors));
	stat_value[16]->setText (QString().sprintf ("%lu", st->eth_intr));

	stat_value[1]->setText (QString::fromUtf8 ((char*) st->location));
	stat_value[3]->setText (QString::number (st->msec / 1000.0, 'f', 3) + tr(" сек"));
	stat_value[5]->setText (QString::number (st->pps) + tr(" Гц"));
	stat_value[7]->setText (QString::number (st->free_bytes) + tr(" байтов"));

	static gps_report_t old;
	if (st->gps.r41_counter != old.r41_counter) {
		//print_data ("41", st->gps.r41_data, REPORT_41_LEN);
		long gps_seconds = fetch_float (st->gps.r41_data);
		if (gps_seconds > 0) {
			unsigned gps_week = st->gps.r41_data[4] << 8 | st->gps.r41_data[5];
			unsigned gps_leaps = fetch_float (st->gps.r41_data + 6);
			show_time_date (gps_seconds - gps_leaps, gps_week);
		}
	}
	if (st->gps.r46_counter != old.r46_counter) {
		static const char *status_message [] = {
			"Doing position fixes",		/* 0 */
			"Do not have GPS time yet",	/* 1 */
			0,				/* 2 */
			"PDOP is too high",		/* 3 */
			"The chosen SV is unusable",	/* 4 */
			0,				/* 5 */
			0,				/* 6 */
			0,				/* 7 */
			"No usable satellites",		/* 8 */
			"Only 1 usable satellite",	/* 9 */
			"Only 2 usable satellites",	/* 10 */
			"Only 3 usable satellites",	/* 11 */
			"Fix criteria not met",		/* 12 и дальше */
		};
		//print_data ("46", st->gps.r46_data, REPORT_46_LEN);
		int status = st->gps.r46_data[0];
		if (status > 12)
			status = 12;
		stat_value[9]->setText (status_message [status]);
		if (st->gps.r46_data[1] & 0x10) {
			text->append (QString().sprintf ("Antenna feedline fault: %s",
				(st->gps.r46_data[1] & 0x20) ?
				"Short detected" : "Open detected"));
		}
	}
	if (st->gps.r84_counter != old.r84_counter) {
		//print_data ("84", st->gps.r84_data, REPORT_84_LEN);
		double gps_latitude = fetch_double (st->gps.r84_data) * 180 / M_PI;
		double gps_longitude = fetch_double (st->gps.r84_data + 8) * 180 / M_PI;
		double gps_altitude = fetch_double (st->gps.r84_data + 16);

		int lat_deg, lat_min, lat_sec;
		lat_deg = gps_latitude;
		if (gps_latitude >= 0) {
			lat_min = (gps_latitude - lat_deg) * 60;
			lat_sec = ((gps_latitude - lat_deg) * 60 - lat_min) * 60;
		} else {
			lat_min = (- gps_latitude + lat_deg) * 60;
			lat_sec = ((- gps_latitude + lat_deg) * 60 - lat_min) * 60;
		}

		int long_deg, long_min, long_sec;
		long_deg = gps_longitude;
		if (gps_longitude >= 0) {
			long_min = (gps_longitude - long_deg) * 60;
			long_sec = ((gps_longitude - long_deg) * 60 - long_min) * 60;
		} else {
			long_min = (- gps_longitude + long_deg) * 60;
			long_sec = ((- gps_longitude + long_deg) * 60 - long_min) * 60;
		}
		stat_value[15]->setText (QString::number (lat_deg) + tr("°") +
			QString().sprintf ("%02d'%02d\", %d",
				lat_min, lat_sec, long_deg) + tr("°") +
			QString().sprintf ("%02d'%02d\"", long_min, long_sec));
		stat_value[17]->setText (QString().sprintf ("%.0f", gps_altitude) +
			tr(" метров"));
	}
	//if (st->gps.r56_counter != old.r56_counter)
	//	print_data ("56", st->gps.r56_data, REPORT_56_LEN);
	//if (st->gps.r6d_counter != old.r6d_counter)
	//	print_data ("6d", st->gps.r6d_data, st->gps.r6d_len);
	//if (st->gps.r82_counter != old.r82_counter)
	//	print_data ("82", st->gps.r82_data, REPORT_82_LEN);
	old = st->gps;
}
