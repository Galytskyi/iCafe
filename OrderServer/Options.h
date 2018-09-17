#ifndef OPTIONS_H
#define OPTIONS_H

#include <QObject>
#include <QMutex>

// ==============================================================================================

#define					DATABASE_OPTIONS_REG_KEY		"Options/Database/"

// ----------------------------------------------------------------------------------------------

const char* const		DatabaseParam[] =
{
						QT_TRANSLATE_NOOP("Options.h", "Path"),
						QT_TRANSLATE_NOOP("Options.h", "Type"),
};

const int				DBO_PARAM_COUNT	= sizeof(DatabaseParam)/sizeof(DatabaseParam[0]);

const int				DBO_PARAM_PATH	= 0,
						DBO_PARAM_TYPE	= 1;

// ----------------------------------------------------------------------------------------------

const char* const		DatabaseType[] =
{
						QT_TRANSLATE_NOOP("Options.h", "SQLite"),
};

const int				DATABASE_TYPE_COUNT		= sizeof(DatabaseType)/sizeof(DatabaseType[0]);

const int				DATABASE_TYPE_SQLITE	= 0;


// ----------------------------------------------------------------------------------------------

class DatabaseOption : public QObject
{
	Q_OBJECT

public:

	explicit DatabaseOption(QObject *parent = 0);
	explicit DatabaseOption(const DatabaseOption& from, QObject *parent = 0);
	virtual ~DatabaseOption();

private:

	QString				m_path;
	int					m_type;

public:

	QString				path() const { return m_path; }
	void				setPath(const QString& path) { m_path = path; }

	int					type() const { return m_type; }
	void				setType(int type) { m_type = type; }


	bool				create();
	void				remove();

	void				load();
	void				save();

	DatabaseOption&		operator=(const DatabaseOption& from);
};

// ==============================================================================================

class Options : public QObject
{
	Q_OBJECT

public:

	explicit	Options(QObject *parent = 0);
				Options(const Options& from, QObject *parent = 0);
	virtual		~Options();

private:

	QMutex				m_mutex;

	DatabaseOption		m_database;

public:

	DatabaseOption&		database() { return m_database; }
	void				setDatabase(const DatabaseOption& database) { m_database = database; }

	void				load();
	void				save();
	void				unload();

	bool				readFromXml();

	Options&			operator=(const Options& from);
};

// ==============================================================================================

extern Options			theOptions;

// ==============================================================================================

#endif // OPTIONS_H
