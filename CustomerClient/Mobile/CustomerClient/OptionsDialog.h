#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>

#include "Options.h"

// ==============================================================================================

class OptionsDialog : public QDialog
{
	Q_OBJECT

public:

	explicit OptionsDialog(QWidget* parent = 0);
	explicit OptionsDialog(const CustomerDataOption& providerData, QWidget* parent = 0);
	virtual ~OptionsDialog();

private:

	CustomerDataOption	m_customerData;

	// elements of interface - Menu
	//
	QLabel*				m_pLogoLabel = nullptr;
	QLabel*				m_pRequestConfigTimeLabel = nullptr;
	QLineEdit*			m_pRequestConfigTimeEdit = nullptr;

	QLabel*				m_pRequestCustomerTimeLabel = nullptr;
	QLineEdit*			m_pRequestCustomerTimeEdit = nullptr;

	QLabel*				m_pServerIPLabel = nullptr;
	QLineEdit*			m_pServerIPEdit = nullptr;

	QLabel*				m_pServerConfigPortLabel = nullptr;
	QLineEdit*			m_pServerConfigPortEdit = nullptr;

	QLabel*				m_pServerCustomerPortLabel = nullptr;
	QLineEdit*			m_pServerCustomerPortEdit = nullptr;


	QDialogButtonBox*	m_buttonBox = nullptr;

	void				createInterface();

public:

	CustomerDataOption	customerData() const { return m_customerData; }
	void				setCustomerData(const CustomerDataOption& cd) { m_customerData = cd; }

	void				initDialog();

private slots:

	void				onOk();
};

// ==============================================================================================

#endif // OPTIONSDIALOG_H
