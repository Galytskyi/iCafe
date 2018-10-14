#include "UdpSocket.h"

#include "../lib/wassert.h"

namespace Udp
{
	// -------------------------------------------------------------------------------------------------------------------
	//
	// UdpRequest class implementation
	//
	// -------------------------------------------------------------------------------------------------------------------

	Request::Request()
	{
		memset(header(), 0, sizeof(RequestHeader)); // clear header of request
		header()->calcCRC();
	}

	// -------------------------------------------------------------------------------------------------------------------

	Request::Request(quint32 id)
	{
		memset(header(), 0, sizeof(RequestHeader)); // clear header of request

		header()->id = id;
		header()->calcCRC();
	}

	// -------------------------------------------------------------------------------------------------------------------

	Request::Request(const QHostAddress& senderAddress, qint16 senderPort, char* receivedData, quint32 receivedDataSize) :
		m_address(senderAddress),
		m_port(senderPort)
	{
		memset(header(), 0, sizeof(RequestHeader));	// clear header of request

		if (receivedData != nullptr && (receivedDataSize >= sizeof(RequestHeader) && receivedDataSize <= MAX_UDP_DATAGRAM_SIZE))
		{
			memcpy(m_rawData, receivedData, m_rawDataSize);

			m_rawDataSize = receivedDataSize;
		}
		else
		{
			wassert(false);
		}

		header()->calcCRC();
	}

	// -------------------------------------------------------------------------------------------------------------------

	Request::~Request()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	Request& Request::operator =(const Request& request)
	{
		m_address = request.address();
		m_port = request.port();

		m_rawDataSize = request.rawDataSize();

		if (m_rawDataSize < sizeof(RequestHeader))
		{
			wassert(m_rawDataSize >= sizeof(RequestHeader));
			m_rawDataSize = sizeof(RequestHeader);
		}
		if (m_rawDataSize > sizeof(m_rawData))
		{
			wassert(m_rawDataSize > sizeof(m_rawData));
			m_rawDataSize = sizeof(m_rawData);
		}

		memcpy(m_rawData, request.rawData(), m_rawDataSize);

		m_readDataIndex = request.m_readDataIndex;
		m_writeDataIndex = request.m_writeDataIndex;

		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Request::writeData(const char* data, quint32 dataSize)
	{
		if (data == nullptr || dataSize == 0)
		{
			wassert(0);
			return false;
		}

		if (m_rawDataSize + dataSize > MAX_UDP_DATAGRAM_SIZE)
		{
			wassert(0);
			return false;
		}

		memcpy(writeDataPtr(), data, dataSize);

		m_writeDataIndex += dataSize;
		m_rawDataSize += dataSize;

		header()->dataSize += dataSize;
		header()->calcCRC();

		return true;
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Request::writeData(const QByteArray& data)
	{
		return writeData(data.constData(), data.size());
	}

	// -------------------------------------------------------------------------------------------------------------------

//	bool Request::writeData(google::protobuf::Message& protobufMessage)
//	{
//		int messageSize = protobufMessage.ByteSize();

//		if (m_rawDataSize + messageSize > MAX_UDP_DATAGRAM_SIZE)
//		{
//			wassert(m_rawDataSize + messageSize <= MAX_UDP_DATAGRAM_SIZE);
//			return false;
//		}

//		protobufMessage.SerializeWithCachedSizesToArray(reinterpret_cast<google::protobuf::uint8*>(writeDataPtr()));

//		m_writeDataIndex += messageSize;

//		m_rawDataSize += messageSize;

//		header()->dataSize += messageSize;
//		header()->calcCRC();

//		return true;
//	}

	// -------------------------------------------------------------------------------------------------------------------

	quint32 Request::readDword()
	{
		if (readDataPtr() - data() + sizeof(quint32) > header()->dataSize)
		{
			wassert(readDataPtr() - data() + sizeof(quint32) <= header()->dataSize);
			return 0;
		}

		quint32 result = *reinterpret_cast<quint32*>(readDataPtr());

		m_readDataIndex += sizeof(quint32);

		return result;
	}


	// -------------------------------------------------------------------------------------------------------------------
	//
	// UdpSocket class implementation
	//
	// -------------------------------------------------------------------------------------------------------------------

	Socket::Socket(QObject *parent)
		: QObject(parent)
		, m_socket(this)
		, m_timer(this)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	Socket::~Socket()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Socket::slot_onThreadStarted()
	{
		onThreadStarted();
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Socket::slot_onThreadFinished()
	{
		onThreadFinished();
		deleteLater();
	}

	// -------------------------------------------------------------------------------------------------------------------
	//
	// UdpClientSocket
	//
	// -------------------------------------------------------------------------------------------------------------------

	ClientSocket::ClientSocket(const QHostAddress &serverAddress, quint16 port)
		: m_serverAddress(serverAddress)
		, m_port(port)
		, m_connect(false)
		, m_failAckCount(0)
		, m_maxFailAckCount(1)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	ClientSocket::~ClientSocket()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::onThreadStarted()
	{
		// generate unique clientID
		//
		//m_clientID = qHash(QUuid::createUuid());
		//m_clientID = 0;

		connect(this, &ClientSocket::sendRequestSignal, this, &ClientSocket::onSendRequest); // send request and wait ack
		connect(&m_socket, &QUdpSocket::readyRead, this, &ClientSocket::onReceiveAck); // receive ack

		m_timer.setSingleShot(true);
		connect(&m_timer, &QTimer::timeout, this, &ClientSocket::onAckTimerTimeout);	// if we did not get an answer

		onSocketThreadStarted();
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::onThreadFinished()
	{
		onSocketThreadFinished();
	}

	// -------------------------------------------------------------------------------------------------------------------

	const QHostAddress& ClientSocket::serverAddress() const
	{
		return m_serverAddress;
	}


	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::setServerAddress(const QHostAddress& serverAddress)
	{
		QMutexLocker locker(&m_mutex);

		m_serverAddress = serverAddress;
	}

	// -------------------------------------------------------------------------------------------------------------------

	quint16 ClientSocket::port() const
	{
		return m_port;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::setPort(quint16 port)
	{
		QMutexLocker locker(&m_mutex);

		m_port = port;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::setConnectState(bool connect)
	{
		if (connect == true)
		{
			m_failAckCount = 0;

			if (m_connect == false) // if old state == false
			{
				onSocketConnected();
				emit socketConnection(true);
			}
		}
		else
		{
			if (m_connect == true) // if old state == true
			{
				onSocketDisconnected();
				emit socketConnection(false);
			}
		}

		m_connect = connect;
	}


	// -------------------------------------------------------------------------------------------------------------------

	bool ClientSocket::isWaitingForAck() const
	{
		QMutexLocker locker(&m_mutex);

		bool result = m_state == WaitingForAck;

		return result;
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool ClientSocket::isReadyToSend() const
	{
		QMutexLocker locker(&m_mutex);

		bool result = m_state == ReadyToSend;

		return result;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::onSendRequest(Request request)
	{
		QMutexLocker locker(&m_mutex);

		//
		//
		if (m_state != State::ReadyToSend)
		{
			wassert(m_state == State::ReadyToSend);
			return;
		}

		//
		//
		m_request.setAddress(m_serverAddress);
		m_request.setPort(m_port);

		wassert(request.ID() != 0);
		m_request.setID(request.ID());

		m_request.setVersion(m_protocolVersion);
		m_request.setNumerator(m_requestNo);
		m_request.setErrorCode(0);

		m_request.initWrite();

		if (request.dataSize() > 0)
		{
			m_request.writeData(request.data(), request.dataSize());
		}

		//
		//
		qint64 sent = m_socket.writeDatagram(m_request.rawData(), m_request.rawDataSize(), m_serverAddress, m_port);
		if (sent == -1)
		{
			//wassert(false);
		}

		m_requestNo++;

		m_state = State::WaitingForAck;

		m_retryCtr = 0;

		m_timer.start(m_msTimeout);
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::sendRequest(const Request& udpRequest)
	{
		emit sendRequestSignal(udpRequest);
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::sendRequest(quint32 requestID)
	{
		Request request(requestID);

		emit sendRequestSignal(request);
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::sendRequest(quint32 requestID, const char* pData, quint32 dataSize)
	{
		Request request(requestID);

		if (pData == nullptr || dataSize == 0)
		{
			emit sendRequestSignal(request);
			return;
		}

		request.writeData(pData, dataSize);

		emit sendRequestSignal(request);
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::sendRequest(quint32 requestID, const sio_OrderWrap& wo)
	{
		sendRequest(requestID, (const char*) &wo, sizeof(sio_OrderWrap));
	}

	// -------------------------------------------------------------------------------------------------------------------

//	void ClientSocket::sendRequest(quint32 requestID, google::protobuf::Message& protobufMessage)
//	{

//		Request request;

//		request.setID(requestID);
//		request.writeData(protobufMessage);

//		emit sendRequestSignal(request);
//	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::retryRequest()
	{
		wassert(m_state == State::WaitingForAck);

		qint64 sent = m_socket.writeDatagram(m_request.rawData(), m_request.rawDataSize(), m_serverAddress, m_port);
		if (sent == -1)
		{
			//wassert(false);
		}

		m_timer.start(m_msTimeout);
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::onReceiveAck()
	{
		QMutexLocker locker(&m_mutex);

		QHostAddress clientAddress;
		quint16 port = 0;

		if (m_state != State::WaitingForAck)
		{
			qint64 skipedDataSize = m_socket.readDatagram(m_ack.rawData(), m_socket.pendingDatagramSize(), &clientAddress, &port);
			qDebug() << "skipped" << skipedDataSize << ", no" << m_ack.numerator();

			//wassert(m_state == State::WaitingForAck);
			return;
		}

		qint64 recevedDataSize = m_socket.readDatagram(m_ack.rawData(), MAX_UDP_DATAGRAM_SIZE, &clientAddress, &port);

		m_state = State::ReadyToSend;

		if (recevedDataSize == -1)
		{
			QAbstractSocket::SocketError err = m_socket.error();
			Q_UNUSED(err)

			return;
		}

		m_timer.stop();

		m_ack.setAddress(clientAddress);
		m_ack.setPort(port);
		m_ack.setRawDataSize(recevedDataSize);

		wassert(m_ack.dataSize() == m_ack.headerDataSize());

		m_ack.initRead();

		bool unknownAck = true;

		if (m_request.ID() == m_ack.ID() &&	m_request.numerator() == m_ack.numerator())
		{
			unknownAck = false;
		}

		if (unknownAck == true)
		{
			emit unknownAckReceived(m_ack);
		}
		else
		{
			wassert(m_ack.data() == m_ack.readDataPtr());
			setConnectState(true);
			emit ackReceived(m_ack);
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ClientSocket::onAckTimerTimeout()
	{
		// If we did not wait for an answer, for the specified part of the time,
		// send the request again m_retryCount times
		//
		QMutexLocker locker(&m_mutex);

		m_ackTimeoutCtr++;

		m_retryCtr++;

		if (m_retryCtr < m_retryCount)
		{
			qDebug() << "Retry request " << m_request.ID();

			retryRequest();
		}
		else
		{
		   m_retryCtr = 0;
		   m_state = State::ReadyToSend;

		   m_failAckCount++;

		   if (m_failAckCount >= m_maxFailAckCount)
		   {
			   setConnectState(false);
		   }

		   emit ackTimeout(m_request);

		   qDebug() << "Ack timeout: server " << m_request.address().toString() << " : " << m_request.port() << ", no" << m_request.numerator();
		}
	}

	// -------------------------------------------------------------------------------------------------------------------
	//
	// UdpServerSocket class implementation
	//
	// -------------------------------------------------------------------------------------------------------------------

	ServerSocket::ServerSocket(const QHostAddress &bindToAddress, quint16 port) :
		m_bindToAddress(bindToAddress),
		m_port(port)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	ServerSocket::~ServerSocket()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ServerSocket::onThreadStarted()
	{
		m_timer.start(1000);

		connect(&m_socket, &QUdpSocket::readyRead, this, &ServerSocket::onReceiveRequest);

		connect(&m_timer, &QTimer::timeout, this, &ServerSocket::onTimer);

		bind();

		onSocketThreadStarted();
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ServerSocket::onThreadFinished()
	{
		onSocketThreadFinished();
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ServerSocket::bind()
	{
		if (m_socket.state() == QAbstractSocket::BoundState)
		{
			return;
		}

		bool result = m_socket.bind(m_bindToAddress, m_port);

		if (result == true)
		{
			qDebug() << "UdpServerSocket::bind - " << m_bindToAddress.toString() << " : " << m_port;
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ServerSocket::sendAck(Request request)
	{
		qint64 sent = m_socket.writeDatagram(request.rawData(), request.rawDataSize(), request.address(), request.port());

		if (sent == -1)
		{
			wassert(false);
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ServerSocket::sendReply(Udp::Request request, const char* pData, quint32 dataSize)
	{
		request.initWrite();
		request.writeData(pData, dataSize);

		sendAck(request);
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ServerSocket::sendReply(const Udp::Request& request, const sio_OrderWrap& wo)
	{
		sendReply(request, (const char*) &wo, sizeof(sio_OrderWrap));
	}

	// -------------------------------------------------------------------------------------------------------------------

//	void ServerSocket::sendReply(Udp::Request request, google::protobuf::Message& protobufMessage)
//	{
//		request.initWrite();
//		request.writeData(protobufMessage);

//		sendAck(request);
//	}

	// -------------------------------------------------------------------------------------------------------------------

	void ServerSocket::onTimer()
	{
		bind();
	}

	// -------------------------------------------------------------------------------------------------------------------

	void ServerSocket::onReceiveRequest()
	{
		QHostAddress clientAddress;
		quint16 port = 0;

		qint64 recevedDataSize = m_socket.readDatagram(m_request.rawData(), MAX_UDP_DATAGRAM_SIZE, &clientAddress, &port);
		if (recevedDataSize == -1)
		{
			return;
		}

		m_request.setAddress(clientAddress);
		m_request.setPort(port);
		m_request.setRawDataSize(recevedDataSize);

		wassert(m_request.dataSize() == m_request.headerDataSize());

		m_request.initRead();

		emit requestReceived(m_request);
	}

	// -------------------------------------------------------------------------------------------------------------------
}
