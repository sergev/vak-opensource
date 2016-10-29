#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;
class QTextEdit;
QT_END_NAMESPACE

struct _xyze10_stat_t;

class Plotter;

class TopWindow : public QDialog
{
	Q_OBJECT

public:
	TopWindow (QWidget *parent = 0);

private slots:
	void request_connection ();
	void read_data ();
	void display_error (QAbstractSocket::SocketError socketError);
	void enable_start_button ();

private:
	QLabel *host_label;
	QLabel *port_label;
	QLineEdit *hostLineEdit;
	QLineEdit *portLineEdit;
	QLabel *status_label;
	QPushButton *startButton;
	QPushButton *quitButton;
	QDialogButtonBox *buttonBox;
	Plotter *plot;
	QTextEdit *text;
	static const int NSTATS = 18;
	QLabel *stat_label[NSTATS], *stat_value[NSTATS];

	QTcpSocket *tcpSocket;
	unsigned char data [514];
	unsigned data_len;
	char *ptr;
	unsigned packets_received;
	unsigned expected_filename;
	unsigned expected_counter;
	unsigned long data_crc_errors;

	void process_data (unsigned char *p);
	void process_statistics (_xyze10_stat_t *st);
	void print_data (const char *title, unsigned char *data, unsigned len);
	void show_time_date (unsigned long seconds, unsigned week);
	void draw_sample (int z, int x, int y);
	float fetch_float (const unsigned char *p);
	double fetch_double (const unsigned char *p);
};

#endif
