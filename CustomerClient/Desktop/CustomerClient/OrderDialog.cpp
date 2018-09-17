#include "OrderDialog.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>

// -------------------------------------------------------------------------------------------------------------------

OrderDialog::OrderDialog(const QString& providerName, const CustomerDataOption& customerData, QWidget* parent)
	: QDialog(parent)
	, m_providerName(providerName)
	, m_customerData(customerData)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

OrderDialog::~OrderDialog()
{
}

// -------------------------------------------------------------------------------------------------------------------

void OrderDialog::createInterface()
{
	// create elements of interface
	//
	QVBoxLayout *providerLayout = new QVBoxLayout;

	m_pProviderLabel = new QLabel(tr("Do you want to order in the"), this);
	m_pProviderEdit = new QLineEdit(this);
	m_pProviderEdit->setReadOnly(true);

	providerLayout->addWidget(m_pProviderLabel);
	providerLayout->addWidget(m_pProviderEdit);

	//
	//
	QVBoxLayout *phoneLayout = new QVBoxLayout;

	m_pPhoneLabel = new QLabel(tr("Your phone number"), this);
	m_pPhoneEdit = new QLineEdit(this);

	phoneLayout->addWidget(m_pPhoneLabel);
	phoneLayout->addWidget(m_pPhoneEdit);

	//
	//
	QVBoxLayout *timeLayout = new QVBoxLayout;

	m_pTimeLabel = new QLabel(tr("Time of you order"), this);
	m_pTimeEdit = new QLineEdit(this);

	timeLayout->addWidget(m_pTimeLabel);
	timeLayout->addWidget(m_pTimeEdit);

	//
	//
	QVBoxLayout *peopleCntLayout = new QVBoxLayout;

	m_pPeopleLabel = new QLabel(tr("Number of people"), this);
	m_pPeopleEdit = new QLineEdit(this);

	peopleCntLayout->addWidget(m_pPeopleLabel);
	peopleCntLayout->addWidget(m_pPeopleEdit);

	//
	//
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

	//
	//
	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addLayout(providerLayout);
	mainLayout->addLayout(phoneLayout);
	mainLayout->addLayout(timeLayout);
	mainLayout->addLayout(peopleCntLayout);
	mainLayout->addWidget(m_buttonBox);

	setLayout(mainLayout);

	initDialog();
}

// -------------------------------------------------------------------------------------------------------------------

void OrderDialog::initDialog()
{
	// init elements of interface
	//
	setWindowFlags(Qt::Drawer);

	Order::Time32 orderTime = m_customerData.orderTime();

	m_pProviderEdit->setText(m_providerName);
	m_pPhoneEdit->setText(QString::number(m_customerData.phone()));
	m_pTimeEdit->setText(QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute));
	m_pPeopleEdit->setText(QString::number(theOptions.customerData().people()));

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &OrderDialog::onOk);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &OrderDialog::reject);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderDialog::onOk()
{
	QString phoneStr = m_pPhoneEdit->text();

	if (phoneStr.isEmpty() == true)
	{
		QMessageBox::information(this, windowTitle(), tr("Please, input your phone number!"));
		m_pPhoneEdit->setFocus();
		return;
	}

	qulonglong phone = phoneStr.toULongLong();

	if (phone == 0)
	{
		QMessageBox::information(this, windowTitle(), tr("Please, input your phone number!"));
		m_pPhoneEdit->setFocus();
		return;
	}

	QString timeStr = m_pTimeEdit->text();


	if (timeStr.isEmpty() == true)
	{
		QMessageBox::information(this, windowTitle(), tr("Please, input order time!"));
		m_pTimeEdit->setFocus();
		return;
	}

	QString hourStr;
	int begPos;

	begPos = timeStr.indexOf(':');
	if (begPos == -1)
	{
		return;
	}

	hourStr = timeStr.left(begPos);
	timeStr.remove(0, begPos + 1);

	Order::Time32 orderTime;

	orderTime.hour = hourStr.toInt();
	orderTime.minute = timeStr.toInt();

	QString peopleStr = m_pPeopleEdit->text();

	if (peopleStr.isEmpty() == true)
	{
		QMessageBox::information(this, windowTitle(), tr("Please, input number of people!"));
		m_pPeopleEdit->setFocus();
		return;
	}

	unsigned int people = peopleStr.toUInt();

	if (people == 0)
	{
		QMessageBox::information(this, windowTitle(), tr("Please, input number of people!"));
		m_pPeopleEdit->setFocus();
		return;
	}


	m_customerData.setPhone(phone);
	m_customerData.setOrderTime(orderTime.time);
	m_customerData.setPeople(people);

	accept();
}

// -------------------------------------------------------------------------------------------------------------------
