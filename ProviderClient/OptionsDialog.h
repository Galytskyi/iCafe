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

	OptionsDialog(QWidget* parent = 0);
	explicit OptionsDialog(const ProviderDataOption& providerData, QWidget* parent = 0);
	virtual ~OptionsDialog();

private:

	ProviderDataOption	m_providerData;

	// elements of interface - Menu
	//
	QLabel*				m_pLogoLabel = nullptr;
	QLabel*				m_pProviderIdLabel = nullptr;
	QLineEdit*			m_pProviderIdEdit = nullptr;

	QLabel*				m_pRequestProviderTimeLabel = nullptr;
	QLineEdit*			m_pRequestProviderTimeEdit = nullptr;

	QLabel*				m_pRequestCustomerTimeLabel = nullptr;
	QLineEdit*			m_pRequestCustomerTimeEdit = nullptr;

	QLabel*				m_pServerIPLabel = nullptr;
	QLineEdit*			m_pServerIPEdit = nullptr;

	QLabel*				m_pServerProviderPortLabel = nullptr;
	QLineEdit*			m_pServerProviderPortEdit = nullptr;

	QLabel*				m_pServerCustomerPortLabel = nullptr;
	QLineEdit*			m_pServerCustomerPortEdit = nullptr;

	QDialogButtonBox*	m_buttonBox = nullptr;

	void				createInterface();

public:

	ProviderDataOption	providerData() const { return m_providerData; }
	void				setProviderData(const ProviderDataOption& pd) { m_providerData = pd; }

	void				initDialog();

signals:

private slots:

	void				onOk();
};

// ==============================================================================================

#endif // OPTIONSDIALOG_H
