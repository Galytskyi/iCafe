TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += qtservice \
    qtpropertybrowser \
	OrderServer \
	ProviderClient/Desktop/ProviderClient \
	ProviderClient/Mobile/ProviderClient \
	CustomerClient/Desktop/CustomerClient \
	CustomerClient/Mobile/CustomerClient
