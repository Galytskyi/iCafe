#ifndef CANCELORDERDIALOG_H
#define CANCELORDERDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

#include "Options.h"

// ==============================================================================================

const int			MAX_CANCEL_CODE_LENGTH = 4;

// ==============================================================================================

const char* const coKeys[] =
{
	"1",		"2",	"3",
	"4",		"5",	"6",
	"7",		"8",	"9",
	"Reset",	"0",	"<",
};

// ==============================================================================================

class CanceOrderDialog : public QDialog
{
	Q_OBJECT

public:

	CanceOrderDialog(QWidget* parent = 0);
	explicit CanceOrderDialog(int code, QWidget* parent = 0);
	virtual ~CanceOrderDialog();

private:

	int					m_cancelCode;

	// elements of interface - Menu
	//
	QLabel*				m_pCodeLabel = nullptr;
	QLineEdit*			m_pCodeEdit = nullptr;
	QPushButton*		m_pKeyboardButton = nullptr;
	QWidget*			m_pVirtualKeyboad = nullptr;

	QDialogButtonBox*	m_buttonBox = nullptr;

	void				createInterface();

public:

	int					cancelCode() const { return m_cancelCode; }
	void				setCancelCode(int code) { m_cancelCode = code; }

	void				initDialog();

signals:

private slots:

	void				onShowKeyboard();
	void				onPressKey();
	void				onOk();
};

// ==============================================================================================

#endif // CANCELCODEDIALOG_H
