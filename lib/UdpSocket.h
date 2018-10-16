#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>

#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QMutex>

#include "../lib/Crc32.h"
#include "../lib/SocketIO.h"

namespace Udp
{
	// ==============================================================================================

	#pragma pack(push, 1)

	struct RequestHeader
	{
		quint32 id = 0;
		quint32 version = UDP_REQUEST_HEADER_VERSION;
		quint32 numerator = 0;
		quint32 errorCode = SIO_ERROR_NONE;
		quint32 dataSize = 0;
		quint32 crc32 = 0xFFFFFFFF;

		void calcCRC() { crc32 = ::CalcCRC32(reinterpret_cast<const char*>(this), sizeof(RequestHeader) - sizeof(quint32)); }
		bool checkCRC() { return ::CalcCRC32(reinterpret_cast<const char*>(this), sizeof(RequestHeader) - sizeof(quint32)) == crc32; }
	};

	#pragma pack(pop)

	// ==============================================================================================
	//
	//	UdpRequest
	//
	// ==============================================================================================
	//
	// every udp request must contain header

	class Request
	{

	public:

		Request();
		explicit Request(quint32 id);
		Request(const QHostAddress& senderAddress, qint16 senderPort, char* receivedData, quint32 receivedDataSize);
		virtual ~Request();

	private:

		QHostAddress	m_address;
		quint16			m_port = 0;

		char			m_rawData[MAX_UDP_DATAGRAM_SIZE];
		quint32			m_rawDataSize = sizeof(RequestHeader);

		unsigned int	m_writeDataIndex = 0;
		unsigned int	m_readDataIndex = 0;

		quint32			m_crc32 = 0;

	private:

		RequestHeader*	header() { return reinterpret_cast<RequestHeader*>(m_rawData); }
		char*			data() { return m_rawData + sizeof(RequestHeader); }							// return pointer on request data after header
		char*			rawData() { return m_rawData; }													// return pointer on request header

		char*			writeDataPtr() { return m_rawData + sizeof(RequestHeader) + m_writeDataIndex; }	// pointer for write request data
		char*			readDataPtr() { return m_rawData + sizeof(RequestHeader) + m_readDataIndex; }	// pointer for read request data

		void			setRawDataSize(quint32 rawDataSize) { m_rawDataSize = rawDataSize; }

		friend class	ClientSocket;
		friend class	ServerSocket;

	public:

		Request&		operator = (const Request& request);

		// change request
		//
		QHostAddress	address() const { return m_address; }
		void			setAddress(const QHostAddress& address) { m_address = address; }

		quint16			port() const { return m_port; }
		void			setPort(quint16 port) { m_port = port; }

		const char*		rawData() const { return m_rawData; }														// return pointer on request header
		const RequestHeader* header() const { return reinterpret_cast<const RequestHeader*>(m_rawData); }
		const char*		data() const { return m_rawData + sizeof(RequestHeader); }									// return pointer on request data after header

		quint32			rawDataSize() const { return m_rawDataSize; }												// full request length with header
		quint32			dataSize() const { return m_rawDataSize - sizeof(RequestHeader); }							// request length without header

		// change header
		//
		quint32			ID() const { return header()->id; }
		void			setID(quint32 id) { header()->id = id; }

		quint32			version() const { return header()->version; }
		void			setVersion(quint32 version) { header()->version = version; }

		quint32			numerator() const { return header()->numerator; }
		void			setNumerator(quint32 numerator) { header()->numerator = numerator; }

		quint32			errorCode() const { return header()->errorCode; }
		void			setErrorCode(quint32 errorCode) { header()->errorCode = errorCode; }

		quint32			headerDataSize() const { return header()->dataSize; }
		quint32			headerCrc() const { return header()->crc32; }
		bool			headerCrcOk() { return header()->checkCRC(); }

		// append to data
		//
		void initWrite()
		{
			m_writeDataIndex = 0;
			m_rawDataSize = sizeof(RequestHeader);

			header()->dataSize = 0;
			header()->calcCRC();
		}

		bool writeData(const char* data, quint32 dataSize);
		bool writeData(const QByteArray& data);
		//bool writeData(google::protobuf::Message& protobufMessage);

		// read data
		//
		void initRead()
		{
			m_readDataIndex = 0;
		}

		quint32 readDword();
	};

	// ==============================================================================================
	//
	//	UdpSocket
	//
	// ==============================================================================================

	class Socket : public QObject
	{
		Q_OBJECT

	public:

		explicit Socket(QObject *parent = nullptr);
		virtual ~Socket();

	protected:

		QUdpSocket		m_socket;
		QTimer			m_timer;

		Request			m_request;
		Request			m_ack;

		virtual void	onThreadStarted() {}
		virtual void	onThreadFinished() {}

	signals:

	public slots:

		void			slot_onThreadStarted();
		void			slot_onThreadFinished();
	};

	// ==============================================================================================
	//
	//	UdpClientSocket
	//
	// ==============================================================================================
	//
	// Client send Request and wait Ack
	//

	// ==============================================================================================

	const int WAIT_ACK_TIMEOUT = 300; // ms

	// ==============================================================================================

	class ClientSocket : public Socket
	{
		Q_OBJECT

	public:

		ClientSocket(const QHostAddress& serverAddress, quint16 port);
		virtual ~ClientSocket();

	private:

		enum State
		{
			ReadyToSend,
			WaitingForAck
		};

		mutable QMutex		m_mutex;

		QHostAddress		m_serverAddress;
		qint16				m_port = 0;

		State				m_state = State::ReadyToSend;
		quint32				m_requestNo = 1;
		quint32				m_protocolVersion = 1;

		int					m_waitAckTimeout = WAIT_ACK_TIMEOUT;
		int					m_retryCount = 0;
		int					m_retryCtr = 0;
		quint32				m_ackTimeoutCtr = 0;

		bool				m_connect = false;
		int					m_failAckCount  = 0;
		int					m_maxFailAckCount = 1;

	protected:

		//
		//
		const QHostAddress&	serverAddress() const;
		void				setServerAddress(const QHostAddress& serverAddress);

		quint16				port() const;
		void				setPort(quint16 port);

		// net connection
		//
		bool				isConnected() const { return m_connect; }
		void				setConnectState(bool connect);

		int					failAckCount() const { return m_failAckCount; }

		int					maxFailAckCount() const { return m_maxFailAckCount; }
		void				setMaxFailAckCount(int count) { m_maxFailAckCount = count; }

		//
		//
		bool				isWaitingForAck() const;
		bool				isReadyToSend() const;

		//
		//
		void				setProtocolVersion(quint32 version) { m_protocolVersion = version; }
		void				setWaitAckTimeout(int ms) { m_waitAckTimeout = ms; }
		void				setRetryCount(int retryCount) { m_retryCount = retryCount; }

		void				sendRequest(const Request& udpRequest);
		void				sendRequest(quint32 requestID);
		void				sendRequest(quint32 requestID, const char* pData, quint32 dataSize);
		void				sendRequest(quint32 requestID, const sio_OrderWrap& wo);
		//void				sendRequest(quint32 requestID, google::protobuf::Message& protobufMessage);
		void				retryRequest();

		virtual void		onThreadStarted();
		virtual void		onThreadFinished();

		virtual void		onSocketThreadStarted() {}
		virtual void		onSocketThreadFinished() {}

		virtual void		onSocketConnected() {}
		virtual void		onSocketDisconnected() {}

	signals:

		void				socketConnection(bool connect);

		void				sendRequestSignal(const Request& udpRequest);

		void				ackReceived(const Request& udpRequest);
		void				unknownAckReceived(const Request& udpRequest);

		void				ackTimeout(const Request& udpRequest);		// did not wait for an ack

	private slots:

		void				onSendRequest(Request request);
		void				onReceiveAck();

		void				onAckTimerTimeout();				// did not wait for an ack
	};

	// ==============================================================================================
	//
	//	UdpServerSocket
	//
	// ==============================================================================================
	//
	// Server receive Request and send Ack
	//

	class ServerSocket : public Socket
	{
		Q_OBJECT

	public:
		ServerSocket(const QHostAddress& bindToAddress, quint16 port);
		virtual ~ServerSocket();

	private:

		QHostAddress	m_bindToAddress;
		qint16			m_port = 0;

		void			bind();

		virtual void	onThreadStarted();
		virtual void	onThreadFinished();

		virtual void	onSocketThreadStarted() {}
		virtual void	onSocketThreadFinished() {}

	signals:

		void			requestReceived(Request request);

	public slots:

		void			sendAck(Request m_request);

		void			sendReply(Udp::Request request, const char* pData, quint32 dataSize);
		void			sendReply(const Request& request, const sio_OrderWrap& wo);
		//void			sendReply(Udp::Request request, google::protobuf::Message& protobufMessage);

	private slots:

		void			onTimer();
		void			onReceiveRequest();
	};

	// ==============================================================================================
}

// ==============================================================================================

Q_DECLARE_METATYPE(Udp::Request)	// for type QVariant

// ==============================================================================================

#endif // UDPSOCKET_H
