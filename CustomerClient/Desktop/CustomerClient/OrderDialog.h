#ifndef ORDERDIALOG_H
#define ORDERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>

#include "Options.h"

#include "../../../lib/Order.h"

// ==============================================================================================

class OrderDialog : public QDialog
{
	Q_OBJECT

public:

	OrderDialog(const QString& providerName, const CustomerDataOption& customerData, QWidget* parent = 0);
	virtual ~OrderDialog();

private:

	QString					m_providerName;
	CustomerDataOption		m_customerData;

	// elements of interface - Menu
	//
	QLabel*				m_pProviderLabel = nullptr;
	QLineEdit*			m_pProviderEdit = nullptr;

	QLabel*				m_pPhoneLabel = nullptr;
	QLineEdit*			m_pPhoneEdit = nullptr;

	QLabel*				m_pTimeLabel = nullptr;
	QLineEdit*			m_pTimeEdit = nullptr;

	QLabel*				m_pPeopleLabel = nullptr;
	QLineEdit*			m_pPeopleEdit = nullptr;

	QDialogButtonBox*	m_buttonBox = nullptr;

	void				createInterface();
	void				initDialog();

public:

	CustomerDataOption	customerData() const { return m_customerData; }

signals:

private slots:

	void				onOk();
};

// ==============================================================================================

#endif // ORDERDIALOG_H
