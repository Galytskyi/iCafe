TEMPLATE = subdirs

CONFIG += ordered

win32:SUBDIRS += Protobuf

SUBDIRS += qtservice \
    qtpropertybrowser \
	OrderServer \
	ProviderClient\Desktop\ProviderClient \
	CustomerClient\Desktop\CustomerClient
