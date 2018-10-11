#include "ProviderDialog.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QValidator>
#include <QDateTime>

// -------------------------------------------------------------------------------------------------------------------
//
// ProviderDialog class implementation
//
// -------------------------------------------------------------------------------------------------------------------

ProviderDialog::ProviderDialog(QWidget* parent)
	: QDialog(parent)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

ProviderDialog::ProviderDialog(const Provider::Item& provider, QWidget* parent)
	: QDialog(parent)
	, m_provider(provider)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

ProviderDialog::~ProviderDialog()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderDialog::createInterface()
{
	setMinimumSize(400, 200);


	// create elements of interface
	//
	QVBoxLayout *activeLayout = new QVBoxLayout;

	m_pProviderActiveCheck = new QCheckBox(tr("Activate this provider"), this);
	m_pEnableDinnerCheck = new QCheckBox(tr("Enable dinner for this provider"), this);

	activeLayout->addWidget(m_pProviderActiveCheck);
	activeLayout->addWidget(m_pEnableDinnerCheck);

	//
	//
	QHBoxLayout *nameLayout = new QHBoxLayout;

	m_pNameLabel = new QLabel(tr("Provider name"), this);
	//m_pNameLabel->setAlignment(Qt::AlignCenter);
	m_pNameEdit = new QLineEdit(this);

	nameLayout->addWidget(m_pNameLabel);
	nameLayout->addWidget(m_pNameEdit);

	//
	//
	QHBoxLayout *addressLayout = new QHBoxLayout;

	m_pAddressLabel = new QLabel(tr("Address"), this);
	//m_pAddressLabel->setAlignment(Qt::AlignCenter);
	m_pAddressEdit = new QLineEdit(this);

	addressLayout->addWidget(m_pAddressLabel);
	addressLayout->addWidget(m_pAddressEdit);

	//
	//
	QHBoxLayout *phoneLayout = new QHBoxLayout;

	m_pPhoneLabel = new QLabel(tr("Phone number (+380**********)"), this);
	//m_pPhoneLabel->setAlignment(Qt::AlignCenter);
	m_pPhoneEdit = new QLineEdit(this);
	m_pPhoneEdit->setValidator(new QRegExpValidator(QRegExp("\\+38[0-9]{10}"), this));

	phoneLayout->addWidget(m_pPhoneLabel);
	phoneLayout->addWidget(m_pPhoneEdit);

	//
	//
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &ProviderDialog::onOk);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &ProviderDialog::reject);
	//
	//
	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addLayout(activeLayout);
	mainLayout->addLayout(nameLayout);
	mainLayout->addLayout(addressLayout);
	mainLayout->addLayout(phoneLayout);
	mainLayout->addWidget(m_buttonBox);

	setLayout(mainLayout);

	initDialog();
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderDialog::initDialog()
{
	// init elements of interface
	//
	m_pProviderActiveCheck->setChecked(m_provider.isActive());
	m_pEnableDinnerCheck->setChecked(m_provider.enableDinner());
	m_pNameEdit->setText(m_provider.name());
	m_pAddressEdit->setText(m_provider.address());
	m_pPhoneEdit->setText(m_provider.phone());
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderDialog::onOk()
{
	bool active = m_pProviderActiveCheck->checkState() == Qt::Checked;
	bool enableDinner = m_pEnableDinnerCheck->checkState() == Qt::Checked;

	QString nameStr = m_pNameEdit->text();

	if (nameStr.isEmpty() == true)
	{
		QMessageBox::information(this, windowTitle(), tr("Please, input name!"));
		m_pNameEdit->setFocus();
		return;
	}

	QString addresStr = m_pAddressEdit->text();

	if (addresStr.isEmpty() == true)
	{
		QMessageBox::information(this, windowTitle(), tr("Please, input address!"));
		m_pAddressEdit->setFocus();
		return;
	}

	QString phoneStr = m_pPhoneEdit->text();

	if (phoneStr.isEmpty() == true || phoneStr.length() != 13)
	{
		QMessageBox::information(this, windowTitle(), tr("Please, input your phone number!"));
		m_pPhoneEdit->setFocus();
		return;
	}

	QString activeTime;

	if (active == true)
	{
		QDateTime ct = QDateTime::currentDateTime();
		activeTime = QString().sprintf("%02d-%02d-%04d %02d:%02d:%02d", ct.date().day(), ct.date().month(), ct.date().year(), ct.time().hour(), ct.time().minute(), ct.time().second());
	}

	m_provider.setActive(active);
	m_provider.setEnableDinner(enableDinner);
	m_provider.setActiveTime(activeTime);
	m_provider.setName(nameStr);
	m_provider.setAddress(addresStr);
	m_provider.setPhone(phoneStr);

	accept();
}

// -------------------------------------------------------------------------------------------------------------------
