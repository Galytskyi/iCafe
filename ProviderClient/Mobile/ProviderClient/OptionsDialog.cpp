#include "OptionsDialog.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>

// -------------------------------------------------------------------------------------------------------------------

OptionsDialog::OptionsDialog(QWidget* parent)
	: QDialog(parent)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

OptionsDialog::OptionsDialog(const ProviderDataOption& providerData, QWidget* parent)
	: QDialog(parent)
	, m_providerData(providerData)
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
	QVBoxLayout *providerIdLayout = new QVBoxLayout;

	m_pProviderIdLabel = new QLabel(tr("ProviderID:"), this);
	m_pProviderIdEdit = new QLineEdit(this);

	providerIdLayout->addWidget(m_pProviderIdLabel);
	providerIdLayout->addWidget(m_pProviderIdEdit);

	//
	//
	QVBoxLayout *requestProviderTimeLayout = new QVBoxLayout;

	m_pRequestProviderTimeLabel = new QLabel(tr("Provider request time (ms):"), this);
	m_pRequestProviderTimeEdit = new QLineEdit(this);

	requestProviderTimeLayout->addWidget(m_pRequestProviderTimeLabel);
	requestProviderTimeLayout->addWidget(m_pRequestProviderTimeEdit);

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
	QVBoxLayout *serverProviderPortLayout = new QVBoxLayout;

	m_pServerProviderPortLabel = new QLabel(tr("Server provider port:"), this);
	m_pServerProviderPortEdit = new QLineEdit(this);

	serverProviderPortLayout->addWidget(m_pServerProviderPortLabel);
	serverProviderPortLayout->addWidget(m_pServerProviderPortEdit);

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


	mainLayout->addLayout(providerIdLayout);
	mainLayout->addLayout(requestProviderTimeLayout);
	mainLayout->addLayout(requestCustomerTimeLayout);
	mainLayout->addLayout(serverIPLayout);
	mainLayout->addLayout(serverProviderPortLayout);
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

	m_pProviderIdEdit->setText(QString::number(m_providerData.providerID()));
	m_pRequestProviderTimeEdit->setText(QString::number(m_providerData.requestProviderTime()));
	m_pRequestCustomerTimeEdit->setText(QString::number(m_providerData.requestCustomerTime()));
	m_pServerIPEdit->setText(m_providerData.serverIP());
	m_pServerProviderPortEdit->setText(QString::number(m_providerData.serveProviderPort()));
	m_pServerCustomerPortEdit->setText(QString::number(m_providerData.serverCustomerPort()));
}

// -------------------------------------------------------------------------------------------------------------------

void OptionsDialog::onOk()
{
	QString providerID = m_pProviderIdEdit->text();
	QString requestProviderTime = m_pRequestProviderTimeEdit->text();
	QString requestCustomerTime = m_pRequestCustomerTimeEdit->text();
	QString serverIP = m_pServerIPEdit->text();
	QString serverProviderPort = m_pServerProviderPortEdit->text();
	QString serverCustomerPort = m_pServerCustomerPortEdit->text();

	m_providerData.setProviderID(providerID.toUInt());
	m_providerData.setRequestProviderTime(requestProviderTime.toUInt());
	m_providerData.setRequestCustomerTime(requestCustomerTime.toUInt());
	m_providerData.setServerIP(serverIP);
	m_providerData.setServerProviderPort(serverProviderPort.toUInt());
	m_providerData.setServerCustomerPort(serverCustomerPort.toUInt());

	theOptions.providerData() = m_providerData;
	theOptions.save();

	accept();
}

// -------------------------------------------------------------------------------------------------------------------
