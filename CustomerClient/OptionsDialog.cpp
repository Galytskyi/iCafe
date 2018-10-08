#include "OptionsDialog.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>

#include "MainWindow.h"

// -------------------------------------------------------------------------------------------------------------------
//
// OptionsDialog class implementation
//
// -------------------------------------------------------------------------------------------------------------------

OptionsDialog::OptionsDialog(QWidget* parent)
	: QDialog(parent)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

OptionsDialog::OptionsDialog(const CustomerDataOption& providerData, QWidget* parent)
	: QDialog(parent)
	, m_customerData(providerData)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

OptionsDialog::~OptionsDialog()
{
}

// -------------------------------------------------------------------------------------------------------------------

void OptionsDialog::createInterface()
{
	// create elements of interface
	//

	QVBoxLayout *requestConfigTimeLayout = new QVBoxLayout;

	m_pLogoLabel = new QLabel(this);
	m_pLogoLabel->setPixmap(QPixmap(":/icons/Settings.png"));
	m_pLogoLabel->setAlignment(Qt::AlignCenter);

	m_pRequestConfigTimeLabel = new QLabel(tr("Config request time (ms):"), this);
	m_pRequestConfigTimeEdit = new QLineEdit(this);

	requestConfigTimeLayout->addWidget(m_pLogoLabel);
	requestConfigTimeLayout->addWidget(m_pRequestConfigTimeLabel);
	requestConfigTimeLayout->addWidget(m_pRequestConfigTimeEdit);

	//
	//
	QVBoxLayout *requestCustomerTimeLayout = new QVBoxLayout;

	m_pRequestCustomerTimeLabel = new QLabel(tr("Customer request time (ms):"), this);
	m_pRequestCustomerTimeEdit = new QLineEdit(this);

	requestCustomerTimeLayout->addWidget(m_pRequestCustomerTimeLabel);
	requestCustomerTimeLayout->addWidget(m_pRequestCustomerTimeEdit);

	//
	//
	QVBoxLayout *serverIPLayout = new QVBoxLayout;

	m_pServerIPLabel = new QLabel(tr("Server IP:"), this);
	m_pServerIPEdit = new QLineEdit(this);

	serverIPLayout->addWidget(m_pServerIPLabel);
	serverIPLayout->addWidget(m_pServerIPEdit);

	//
	//
	QVBoxLayout *serverConfigPortLayout = new QVBoxLayout;

	m_pServerConfigPortLabel = new QLabel(tr("Server config port:"), this);
	m_pServerConfigPortEdit = new QLineEdit(this);

	serverConfigPortLayout->addWidget(m_pServerConfigPortLabel);
	serverConfigPortLayout->addWidget(m_pServerConfigPortEdit);

	//
	//
	QVBoxLayout *serverCustomerPortLayout = new QVBoxLayout;

	m_pServerCustomerPortLabel = new QLabel(tr("Server сustomer port:"), this);
	m_pServerCustomerPortEdit = new QLineEdit(this);

	serverCustomerPortLayout->addWidget(m_pServerCustomerPortLabel);
	serverCustomerPortLayout->addWidget(m_pServerCustomerPortEdit);

	//
	//
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &OptionsDialog::onOk);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &OptionsDialog::reject);

	//
	//
	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addLayout(requestConfigTimeLayout);
	mainLayout->addLayout(requestCustomerTimeLayout);
	mainLayout->addLayout(serverIPLayout);
	mainLayout->addLayout(serverConfigPortLayout);
	mainLayout->addLayout(serverCustomerPortLayout);
	mainLayout->addWidget(m_buttonBox);

	setLayout(mainLayout);

	initDialog();
}

// -------------------------------------------------------------------------------------------------------------------

void OptionsDialog::initDialog()
{
	// init elements of interface
	//
	m_pRequestConfigTimeEdit->setText(QString::number(m_customerData.requestConfigTime()));
	m_pRequestCustomerTimeEdit->setText(QString::number(m_customerData.requestCustomerTime()));
	m_pServerIPEdit->setText(m_customerData.serverIP());
	m_pServerConfigPortEdit->setText(QString::number(m_customerData.serverConfigPort()));
	m_pServerCustomerPortEdit->setText(QString::number(m_customerData.serverCustomerPort()));
}

// -------------------------------------------------------------------------------------------------------------------

void OptionsDialog::onOk()
{
	QString requestConfigTime = m_pRequestConfigTimeEdit->text();
	QString requestCustomerTime = m_pRequestCustomerTimeEdit->text();
	QString serverIP = m_pServerIPEdit->text();
	QString serverConfigPort = m_pServerConfigPortEdit->text();
	QString serverCustomerPort = m_pServerCustomerPortEdit->text();

	m_customerData.setRequestConfigTime(requestConfigTime.toUInt());
	m_customerData.setRequestCustomerTime(requestCustomerTime.toUInt());
	m_customerData.setServerIP(serverIP);
	m_customerData.setServerConfigPort(serverConfigPort.toUInt());
	m_customerData.setServerCustomerPort(serverCustomerPort.toUInt());

	theOptions.customerData() = m_customerData;
	theOptions.save();

	accept();
}

// -------------------------------------------------------------------------------------------------------------------


