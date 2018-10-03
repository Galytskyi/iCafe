#ifndef PROVIDERDIALOG_H
#define PROVIDERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>

#include "Options.h"

#include "../../lib/Provider.h"

// ==============================================================================================

class ProviderDialog : public QDialog
{
	Q_OBJECT

public:

	explicit ProviderDialog(QWidget* parent = 0);
	ProviderDialog(const Provider::Item& provider, QWidget* parent = 0);
	virtual ~ProviderDialog();

private:

	Provider::Item		m_provider;

	// elements of interface - Menu
	//
	QLabel*				m_pProviderIDLabel = nullptr;
	QLineEdit*			m_pProviderIDEdit = nullptr;

	QLabel*				m_pProviderActiveLabel = nullptr;
	QCheckBox*			m_pProviderActiveCheck = nullptr;

	QLabel*				m_pNameLabel = nullptr;
	QLineEdit*			m_pNameEdit = nullptr;

	QLabel*				m_pAddressLabel = nullptr;
	QLineEdit*			m_pAddressEdit = nullptr;

	QLabel*				m_pPhoneLabel = nullptr;
	QLineEdit*			m_pPhoneEdit = nullptr;

	QDialogButtonBox*	m_buttonBox = nullptr;

	void				createInterface();

public:

	Provider::Item		provider() const { return m_provider; }
	void				setProvider(const Provider::Item& provider) { m_provider = provider; }

	void				initDialog();

private slots:

	void				onOk();
};

// ==============================================================================================

#endif // ORDERDIALOG_H
