#include "InputCodeDialog.h"

#include <QMessageBox>
#include <QValidator>

// -------------------------------------------------------------------------------------------------------------------
//
// CanceOrderDialog class implementation
//
// -------------------------------------------------------------------------------------------------------------------

InputCodeDialog::InputCodeDialog(QWidget* parent)
	: QDialog(parent)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

InputCodeDialog::InputCodeDialog(int code, QWidget* parent)
	: QDialog(parent)
	, m_cancelCode(code)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

InputCodeDialog::~InputCodeDialog()
{
}

// -------------------------------------------------------------------------------------------------------------------

void InputCodeDialog::createInterface()
{
	QFont* listFont = new QFont("Arial", 14, 2);

	// create elements of interface
	//
	QHBoxLayout *codeLayout = new QHBoxLayout;

	m_pCodeLabel = new QLabel(tr("Код отмены:"), this);
	m_pCodeEdit = new QLineEdit(this);

	QPixmap keyboardPix(":/icons/Keyboard.png");
	m_pKeyboardButton = new QPushButton(QString(), this);
	m_pKeyboardButton->setIcon(keyboardPix);
	m_pKeyboardButton->setIconSize(keyboardPix.size());

	m_pCodeEdit->setFont(*listFont);
	m_pCodeEdit->setAlignment(Qt::AlignCenter);

	QValidator *validator = new QIntValidator(1, 9999, this);
	m_pCodeEdit->setValidator(validator);

	m_pKeyboardButton->setCheckable(true);
	m_pKeyboardButton->setChecked(theOptions.providerData().showKeyboard());
	connect(m_pKeyboardButton, &QPushButton::clicked, this, &InputCodeDialog::onShowKeyboard);

	codeLayout->addWidget(m_pCodeLabel);
	codeLayout->addWidget(m_pCodeEdit);
	codeLayout->addWidget(m_pKeyboardButton);

	//
	//
	m_pVirtualKeyboad = new QWidget(this);

		QVBoxLayout* keyLayout = new QVBoxLayout;

		for(int r = 0; r < 4; r++)
		{
			QHBoxLayout *keyRowLayout = new QHBoxLayout;

			for(int c = 0; c < 3; c++)
			{
				QPushButton* pButton = new QPushButton(coKeys[c + r*3], this);
				pButton->setFont(*listFont);
				connect(pButton, &QPushButton::clicked, this, &InputCodeDialog::onPressKey);

				keyRowLayout->addWidget(pButton);
			}

			keyLayout->addLayout(keyRowLayout);
		}

	m_pVirtualKeyboad->setLayout(keyLayout);

	//
	//
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &InputCodeDialog::onOk);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &InputCodeDialog::reject);

	//
	//
	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addLayout(codeLayout);
	mainLayout->addWidget(m_pVirtualKeyboad);
	mainLayout->addStretch();
	mainLayout->addWidget(m_buttonBox);

	setLayout(mainLayout);

	initDialog();
}

// -------------------------------------------------------------------------------------------------------------------

void InputCodeDialog::initDialog()
{
	if (m_pCodeEdit == nullptr)
	{
		return;
	}

	// init elements of interface
	//
	m_pCodeEdit->setText(QString());
	m_pCodeEdit->setFocus();

	onShowKeyboard();
}

// -------------------------------------------------------------------------------------------------------------------

void InputCodeDialog::onShowKeyboard()
{
	if (m_pVirtualKeyboad == nullptr)
	{
		return;
	}

	theOptions.providerData().setShowKeyboard(m_pKeyboardButton->isChecked());

	if (theOptions.providerData().showKeyboard() == true)
	{
		m_pVirtualKeyboad->show();
	}
	else
	{
		m_pVirtualKeyboad->hide();
	}

	theOptions.providerData().save();
}

// -------------------------------------------------------------------------------------------------------------------

void InputCodeDialog::onPressKey()
{
	QPushButton* pButton = (QPushButton*) sender();
	if (pButton == nullptr)
	{
		return;
	}

	QString keyStr = pButton->text();
	if (keyStr == "Reset")
	{
		m_pCodeEdit->setText(QString());
		return;
	}

	QString codeStr = m_pCodeEdit->text();

	if (keyStr == "<")
	{
		codeStr.remove(codeStr.length() - 1, 1);
	}
	else
	{
		if (codeStr.length() < MAX_CANCEL_CODE_LENGTH)
		{
			codeStr.append(keyStr);
		}
	}

	m_pCodeEdit->setText(codeStr);
}

// -------------------------------------------------------------------------------------------------------------------

void InputCodeDialog::onOk()
{
	int code = m_pCodeEdit->text().toInt();

	if (code != m_cancelCode)
	{
		QMessageBox::information(this, tr("Код отмены:"), tr("Вы ввели не верный код отмены!\nПопробуйте снова."));
		m_pCodeEdit->setText(QString());
		m_pCodeEdit->setFocus();
		return;
	}

	accept();
}

// -------------------------------------------------------------------------------------------------------------------
