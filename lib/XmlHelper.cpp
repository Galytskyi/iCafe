#include "XmlHelper.h"

#include "../lib/wassert.h"

// -------------------------------------------------------------------------------------
//
// XmlWriteHelper class implementation
//
// -------------------------------------------------------------------------------------

XmlWriteHelper::XmlWriteHelper(QXmlStreamWriter& xmlWriter) :
	m_xmlWriter(&xmlWriter)
{
}

XmlWriteHelper::XmlWriteHelper(QByteArray* data)
{
	m_xmlLocalWriter = new QXmlStreamWriter(data);
	m_xmlWriter = m_xmlLocalWriter;
}

XmlWriteHelper::~XmlWriteHelper()
{
	if (m_xmlLocalWriter != nullptr)
	{
		delete m_xmlLocalWriter;
	}
}

QXmlStreamWriter* XmlWriteHelper::xmlStreamWriter() const
{
	return m_xmlWriter;
}

void XmlWriteHelper::setAutoFormatting(bool autoFormatting)
{
	m_xmlWriter->setAutoFormatting(autoFormatting);
}

void XmlWriteHelper::writeStartDocument()
{
	m_xmlWriter->writeStartDocument();
}

void XmlWriteHelper::writeEndDocument()
{
	m_xmlWriter->writeEndDocument();
}

void XmlWriteHelper::writeStartElement(const QString& name)
{
	m_xmlWriter->writeStartElement(name);
}

void XmlWriteHelper::writeEndElement()
{
	m_xmlWriter->writeEndElement();
}

void XmlWriteHelper::writeStringAttribute(const QString& name, const QString& value)
{
	m_xmlWriter->writeAttribute(name, value);
}

void XmlWriteHelper::writeIntAttribute(const QString& name, int value, bool hex)
{
	if (hex == true)
	{
		m_xmlWriter->writeAttribute(name, "0x" + QString::number(value, 16).toUpper());
	}
	else
	{
		m_xmlWriter->writeAttribute(name, QString::number(value));
	}
}

void XmlWriteHelper::writeBoolAttribute(const QString& name, bool value)
{
	writeStringAttribute(name, value ? "true" : "false");
}

void XmlWriteHelper::writeUInt64Attribute(const QString& name, quint64 value, bool hex)
{
	QString valueStr;

	if (hex == true)
	{
		valueStr = "0x" + QString::number(static_cast<qulonglong>(value), 16).toUpper();
	}
	else
	{
		valueStr = QString::number(static_cast<qulonglong>(value));
	}

	m_xmlWriter->writeAttribute(name, valueStr);
}

void XmlWriteHelper::writeUInt32Attribute(const QString& name, quint32 value, bool hex)
{
	QString valueStr;

	if (hex == true)
	{
		valueStr = "0x" + QString::number(static_cast<ulong>(value), 16).toUpper();
	}
	else
	{
		valueStr = QString::number(static_cast<ulong>(value));
	}

	m_xmlWriter->writeAttribute(name, valueStr);
}

void XmlWriteHelper::writeDoubleAttribute(const QString& name, double value)
{
	writeStringAttribute(name, QString::number(value));
}

void XmlWriteHelper::writeDoubleAttribute(const QString& name, double value, int decimalPlaces)
{
	writeStringAttribute(name, QString::number(value, 'f', decimalPlaces));
}

void XmlWriteHelper::writeFloatAttribute(const QString& name, float value)
{
	writeStringAttribute(name, QString::number(value));
}

void XmlWriteHelper::writeString(const QString& str)
{
	m_xmlWriter->writeCharacters(str);
}

void XmlWriteHelper::writeStringElement(const QString& name, const QString& value)
{
	m_xmlWriter->writeTextElement(name, value);
}

void XmlWriteHelper::writeIntElement(const QString& name, int value)
{
	m_xmlWriter->writeTextElement(name, QString::number(value));
}

void XmlWriteHelper::writeBoolElement(const QString& name, bool value)
{
	m_xmlWriter->writeTextElement(name, value == true ? "true" : "false");
}

// -------------------------------------------------------------------------------------
//
// XmlReadHelper class implementation
//
// -------------------------------------------------------------------------------------

XmlReadHelper::XmlReadHelper(QXmlStreamReader& xmlReader) :
	m_xmlReader(&xmlReader)
{
}

XmlReadHelper::XmlReadHelper(const QByteArray& data)
{
	m_xmlLocalReader = new QXmlStreamReader(data);
	m_xmlReader = m_xmlLocalReader;
}

XmlReadHelper::~XmlReadHelper()
{
	if (m_xmlLocalReader != nullptr)
	{
		delete m_xmlLocalReader;
	}
}

bool XmlReadHelper::readNextStartElement()
{
	return m_xmlReader->readNextStartElement();
}

void XmlReadHelper::skipCurrentElement()
{
	m_xmlReader->skipCurrentElement();
}

QString XmlReadHelper::name()
{
	return m_xmlReader->name().toString();
}

bool XmlReadHelper::atEnd()
{
	return m_xmlReader->atEnd();
}

bool XmlReadHelper::readIntAttribute(const QString& name, int* value)
{
	if(value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (m_xmlReader->attributes().hasAttribute(name) == false)
	{
		return false;
	}

	bool result = false;

	QString str = m_xmlReader->attributes().value(name).toString();

	*value = str.toInt(&result, 0);

	return result;
}

bool XmlReadHelper::readDoubleAttribute(const QString& name, double* value)
{
	if(value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (m_xmlReader->attributes().hasAttribute(name) == false)
	{
		return false;
	}

	bool result = false;

	*value = m_xmlReader->attributes().value(name).toDouble(&result);

	return result;
}

bool XmlReadHelper::readFloatAttribute(const QString& name, float* value)
{
	if(value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (m_xmlReader->attributes().hasAttribute(name) == false)
	{
		return false;
	}

	bool result = false;

	*value = m_xmlReader->attributes().value(name).toFloat(&result);

	return result;
}

bool XmlReadHelper::readUInt64Attribute(const QString& name, qulonglong *value)
{
	if(value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (m_xmlReader->attributes().hasAttribute(name) == false)
	{
		return false;
	}

	QString str;

	bool result = true;

	str = m_xmlReader->attributes().value(name).toString();

	*value = str.toULongLong(&result, 0);

	return result;
}

bool XmlReadHelper::readUInt32Attribute(const QString& name, quint32 *value)
{
	if(value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (m_xmlReader->attributes().hasAttribute(name) == false)
	{
		return false;
	}

	QString str;

	bool result = true;

	str = m_xmlReader->attributes().value(name).toString();

	*value = str.toULong(&result, 0);

	return result;
}

bool XmlReadHelper::readBoolAttribute(const QString& name, bool* value)
{
	if(value == nullptr)
	{
		wassert(false);
		return false;
	}

	QString boolStr;

	bool result = readStringAttribute(name, &boolStr);

	if (result == false)
	{
		return false;
	}

	if (boolStr == "true")
	{
		*value = true;
	}
	else
	{
		if (boolStr == "false")
		{
			*value = false;
		}
		else
		{
			wassert(false);
			return false;
		}
	}

	return true;
}

bool XmlReadHelper::readStringAttribute(const QString& name, QString* value)
{
	if(value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (m_xmlReader->attributes().hasAttribute(name) == false)
	{
		return false;
	}

	*value = m_xmlReader->attributes().value(name).toString();

	return true;
}

bool XmlReadHelper::readStringElement(const QString& elementName, QString* value, bool find)
{
	if (value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (find == true)
	{
		if (findElement(elementName) == false)
		{
			return false;
		}
	}

	if (checkElement(elementName) == false)
	{
		return false;
	}

	QString str = m_xmlReader->readElementText();

	*value = str;

	return true;
}

bool XmlReadHelper::readIntElement(const QString& elementName, int* value, bool find)
{
	if (value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (find == true)
	{
		if (findElement(elementName) == false)
		{
			return false;
		}
	}

	if (checkElement(elementName) == false)
	{
		return false;
	}

	QString str = m_xmlReader->readElementText();

	bool ok = true;

	*value = str.toInt(&ok);

	return ok;
}

bool XmlReadHelper::readBoolElement(const QString& elementName, bool* value, bool find)
{
	if (value == nullptr)
	{
		wassert(false);
		return false;
	}

	if (find == true)
	{
		if (findElement(elementName) == false)
		{
			return false;
		}
	}

	if (checkElement(elementName) == false)
	{
		return false;
	}

	QString str = m_xmlReader->readElementText();

	if (str == "true")
	{
		*value = true;
	}
	else
	{
		if (str == "false")
		{
			*value = false;
		}
		else
		{
			wassert(false);
			return false;
		}
	}

	return true;
}

bool XmlReadHelper::findElement(const QString& elementName)
{
	while(m_xmlReader->atEnd() == false)
	{
		if (m_xmlReader->readNextStartElement() == false)
		{
			continue;
		}

		if (name() == elementName)
		{
			return true;
		}
	}

	//qDebug() << "XmlReadHelper: element is not found -" << elementName;

	return false;
}

bool XmlReadHelper::checkElement(const QString& elementName)
{
	if (name() == elementName)
	{
		return true;
	}

	//qDebug() << "XmlReadHelper: element does not match. Current - " << name() << ", required -" << elementName;

	return false;
}

// -------------------------------------------------------------------------------------
