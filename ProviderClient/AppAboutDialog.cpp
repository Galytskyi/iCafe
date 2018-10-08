#include "AppAboutDialog.h"

#include <QApplication>
#include <QVBoxLayout>

// -------------------------------------------------------------------------------------------------------------------
//
// AppAboutDialog class implementation
//
// -------------------------------------------------------------------------------------------------------------------

AppAboutDialog::AppAboutDialog(QWidget* parent)
	: QDialog(parent)
{
	createInterface();
}

// -------------------------------------------------------------------------------------------------------------------

AppAboutDialog::~AppAboutDialog()
{
}

// -------------------------------------------------------------------------------------------------------------------

void AppAboutDialog::createInterface()
{
	// create elements of interface
	//

	QVBoxLayout *infoLayout = new QVBoxLayout;

	m_pLogoLabel = new QLabel(this);
	m_pLogoLabel->setPixmap(QPixmap(":/icons/Logo.png"));
	m_pLogoLabel->setAlignment(Qt::AlignCenter);
	m_pVersionLabel = new QLabel(tr("Версия %1").arg(qApp->applicationVersion()), this);
	m_pVersionLabel->setAlignment(Qt::AlignCenter);
	m_pMailLabel = new QLabel("<a href=\"mailto:iBookingRest@gmail.com\">iBookingRest@gmail.com</a>", this);
	m_pMailLabel->setOpenExternalLinks(true);
	m_pMailLabel->setAlignment(Qt::AlignCenter);
	m_pSiteLabel = new QLabel("<a href=\"http://BookingRest.com.ua\">BookingRest.com.ua</a>", this);
	m_pSiteLabel->setOpenExternalLinks(true);
	m_pSiteLabel->setAlignment(Qt::AlignCenter);

	infoLayout->addWidget(m_pLogoLabel);
	infoLayout->addWidget(m_pVersionLabel);
	infoLayout->addWidget(m_pMailLabel);
	infoLayout->addWidget(m_pSiteLabel);

	//
	//
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &AppAboutDialog::accept);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &AppAboutDialog::reject);

	//
	//
	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addLayout(infoLayout);
	mainLayout->addStretch();
	mainLayout->addWidget(m_buttonBox);

	setLayout(mainLayout);
}

// -------------------------------------------------------------------------------------------------------------------
