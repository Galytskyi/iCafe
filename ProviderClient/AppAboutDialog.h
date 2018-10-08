#ifndef APPABOUTDIALOG_H
#define APPABOUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>

// ==============================================================================================

class AppAboutDialog : public QDialog
{
	Q_OBJECT

public:

	explicit AppAboutDialog(QWidget* parent = 0);
	virtual ~AppAboutDialog();

private:

	// elements of interface - Menu
	//
	QLabel*				m_pLogoLabel = nullptr;
	QLabel*				m_pVersionLabel = nullptr;
	QLabel*				m_pMailLabel = nullptr;
	QLabel*				m_pSiteLabel = nullptr;

	QDialogButtonBox*	m_buttonBox = nullptr;

	void				createInterface();
};

// ==============================================================================================

#endif // ORDERDIALOG_H
