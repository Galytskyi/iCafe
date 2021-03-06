#include "CustomerOrderSocket.h"

#include "Options.h"

#include "../lib/wassert.h"
#include "../lib/SocketIO.h"
#include "../lib/Provider.h"

// -------------------------------------------------------------------------------------------------------------------
//
// OrderCustomerSocket class implementation
//
// -------------------------------------------------------------------------------------------------------------------

CustomerOrderSocket::CustomerOrderSocket(const QHostAddress &serverAddress, quint16 port) :
	Udp::ServerSocket(serverAddress, port)
{
}

// -------------------------------------------------------------------------------------------------------------------

CustomerOrderSocket::~CustomerOrderSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::onSocketThreadStarted()
{
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "started");

	connect(this, &Udp::ServerSocket::requestReceived, this, &CustomerOrderSocket::processRequest, Qt::QueuedConnection);

	qsrand(QTime::currentTime().msecsSinceStartOfDay());
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::onSocketThreadFinished()
{
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "finished");
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::processRequest(Udp::Request request)
{
	if(request.headerCrcOk() == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "SIO_ERROR_INCCORECT_CRC32");
		request.setErrorCode(SIO_ERROR_INCCORECT_CRC32);
		sendAck(request);
		return;
	}

	switch(request.ID())
	{
		case CLIENT_GET_CUSTOMER_UDP_OPTION:
			replyCustomerUdpOption(request);
			break;

		case CLIENT_CREATE_ORDER:
			replyCreateOrder(request);
			break;

		case CLIENT_GET_ORDER_STATE:
			replyGetOrderState(request);
			break;

		case CLIENT_REMOVE_ORDER:
			replyRemoveOrder(request);
			break;

		default:
			emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Unknown request.ID(): %1").arg(request.ID()));
			request.setErrorCode(SIO_ERROR_INCCORECT_REQUEST_ID);
			sendAck(request);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyCustomerUdpOption(const Udp::Request& request)
{
	sio_UdpOption uo;

	uo.version = REPLY_UDP_OPTION_VERSION;
	uo.requestTime = theOptions.udp().requestCustomerTime();
	uo.waitReplyTime = theOptions.udp().waitReplyCustomerTime();

	sendReply(request, (const char*) &uo, sizeof(sio_UdpOption));
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyCreateOrder(const Udp::Request& request)
{
	//char* pData = (char*) const_cast<const Udp::Request&>(request).data();

	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{
		wo.state = Order::STATE_INCORRECT_PARSE_ORDERWRAP;
		sendReply(request, wo);

		wassert(0);
		return;
	}

	if (wo.state != Order::STATE_CUSTOMER_CREATING_ORDER)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_REQUEST_INCORRECT");

		wo.state = Order::STATE_INCORRECT_REQUEST;
		sendReply(request, wo);

		return;
	}

	quint32 providerID = ((Order::Handle) wo.orderID).providerID;
	if (providerID == -1)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_PROVIDER_NOT_FOUND");

		wo.state = Order::STATE_PROVIDER_NOT_FOUND;
		sendReply(request, wo);

		return;
	}

	Provider::Item* pProvider = theProviderBase.providerPtr( providerID );
	if (pProvider == nullptr)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_PROVIDER_NOT_FOUND");

		wo.state = Order::STATE_PROVIDER_NOT_FOUND;
		sendReply(request, wo);

		return;
	}

	if (pProvider->isActive() == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_PROVIDER_NOT_ACTIVE");

		wo.state = Order::STATE_PROVIDER_NOT_ACTIVE;
		sendReply(request, wo);

		return;
	}

	if (pProvider->enableAcceptOrder() == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_WARNING, __FUNCTION__, "Order::STATE_PROVIDER_DONT_TAKE_ORDER");

		wo.state = Order::STATE_PROVIDER_DONT_TAKE_ORDER;
		sendReply(request, wo);

		return;
	}


	if (pProvider->orderBase().isExist(wo.orderID) == true)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_WARNING, __FUNCTION__, "Order::STATE_ORDER_ALREADY_EXIST");

		wo = pProvider->orderBase().order(wo.orderID).toWrap();
		wo.state = Order::STATE_ORDER_ALREADY_EXIST;
		sendReply(request, wo);

		return;
	}


	Order::Item order(wo);

	order.setAddress(request.address());
	order.setPort(request.port());
	order.setState(Order::STATE_SERVER_CREATED_ORDER);
	order.orderTime().fillCurrentDate();
	order.setRemoveTime();
	order.createCancelCode();

	//
	//
	QString msgStr = QString("Created OrderID: %1, Customer +380%2, ProviderID %3, order of %4 for %5 people at the %6, cancel code %7").
					 arg(order.handle().ID).
					 arg(order.phone()).
					 arg(order.providerID()).
					 arg(order.typeStr()).
					 arg(order.people()).
					 arg(QString().sprintf("%02d:%02d", order.orderTime().hour, order.orderTime().minute)).
					 arg(order.cancelCode());
	emit appendMessageToArch(ARCH_MSG_TYPE_ORDER, __FUNCTION__, msgStr);

	//
	//
	pProvider->orderBase().append(order);

	//
	//
	wo = order.toWrap();
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyGetOrderState(const Udp::Request& request)
{
	sio_RequestGetOrderState* ptr_rgos_in = (sio_RequestGetOrderState*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rgos_in->version < 1 || ptr_rgos_in->version > REQUEST_GET_ORDER_STATE_VERSION)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Wrong reply version: %1").arg(ptr_rgos_in->version));
		return;
	}

	sio_RequestGetOrderState rgos_out;

	rgos_out.version = REQUEST_GET_ORDER_STATE_VERSION;
	rgos_out.count = 0;

	int orderCount = ptr_rgos_in->count;
	for(int i  = 0; i < orderCount; i++)
	{
		quint64 orderID = ptr_rgos_in->orderState[i].orderID;

		rgos_out.orderState[rgos_out.count].orderID = orderID;

		quint32 providerID = ((Order::Handle) orderID).providerID;
		if (providerID == -1)
		{
			emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_PROVIDER_NOT_FOUND");

			rgos_out.orderState[rgos_out.count].state = Order::STATE_PROVIDER_NOT_FOUND;
			rgos_out.count++;

			continue;
		}

		Provider::Item* pProvider = theProviderBase.providerPtr(providerID);
		if (pProvider == nullptr)
		{
			emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_PROVIDER_NOT_FOUND");

			rgos_out.orderState[rgos_out.count].state = Order::STATE_PROVIDER_NOT_FOUND;
			rgos_out.count++;

			continue;
		}

		if (pProvider->orderBase().isExist(orderID) == false)
		{
			emit appendMessageToArch(ARCH_MSG_TYPE_WARNING, __FUNCTION__, "Order::STATE_ORDER_NOT_FOUND");

			rgos_out.orderState[rgos_out.count].state = Order::STATE_ORDER_NOT_FOUND;
			rgos_out.count++;

			continue;
		}

		rgos_out.orderState[rgos_out.count].state = pProvider->orderBase().orderState(orderID);
		rgos_out.count++;
	}

	sendReply(request, (const char*) &rgos_out, sizeof(sio_RequestGetOrderState));
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyRemoveOrder(const Udp::Request& request)
{
	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{

		wo.state = Order::STATE_INCORRECT_PARSE_ORDERWRAP;
		sendReply(request, wo);

		wassert(0);
		return;
	}

	if (wo.state != Order::STATE_CUSTOMER_REMOVING_ORDER)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_REQUEST_INCORRECT");

		wo.state = Order::STATE_INCORRECT_REQUEST;
		sendReply(request, wo);

		return;
	}

	quint32 providerID = ((Order::Handle) wo.orderID).providerID;
	if (providerID == -1)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_PROVIDER_NOT_FOUND");

		wo.state = Order::STATE_PROVIDER_NOT_FOUND;
		sendReply(request, wo);

		return;
	}

	Provider::Item* pProvider = theProviderBase.providerPtr( providerID );
	if (pProvider == nullptr)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_PROVIDER_NOT_FOUND");

		wo.state = Order::STATE_PROVIDER_NOT_FOUND;
		sendReply(request, wo);

		return;
	}

	if (pProvider->orderBase().isExist(wo.orderID) == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_ORDER_NOT_FOUND");

		wo.state = Order::STATE_ORDER_NOT_FOUND;
		sendReply(request, wo);

		return;
	}

	if (pProvider->orderBase().remove(wo.orderID) == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_ORDER_NOT_REMOVED");

		wo.state = Order::STATE_ORDER_NOT_REMOVED;
		sendReply(request, wo);

		return;
	}

	//
	//
	Order::Item order(wo);
	QString msgStr = QString("Removed OrderID: %1, Customer +380%2, ProviderID %3").
					 arg(order.handle().ID).
					 arg(order.phone()).
					 arg(order.providerID());
	emit appendMessageToArch(ARCH_MSG_TYPE_ORDER, __FUNCTION__, msgStr);

	//
	//
	wo.state = Order::STATE_SERVER_REMOVED_ORDER;
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------

