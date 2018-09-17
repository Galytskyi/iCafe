#ifndef MYSERVICE_H
#define MYSERVICE_H


#include "qtservice.h"

class MyService : public QtService<QCoreApplication>
{

public:

	MyService(int argc, char **argv);

	~MyService();

protected:

	void start();

	void pause();

	void resume();
};

#endif // MYSERVICE_H
