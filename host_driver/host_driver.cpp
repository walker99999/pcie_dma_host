#include "host_driver.h"

Driver::Driver()
	: fd(0)
{
	open_pci_driver();
}

Driver::~Driver()
{
	if (fd)
	{
		close(fd);
	}
	cout << "Exit driver !!!" << endl;
}

void Driver::open_pci_driver()
{
	fd = open(PCIE_DRIVER_FILE_PATH, O_RDWR);
	if (fd < 0)
	{
		cout << "PCIe Device Open Fail !!!" << endl;
		perror("open fail\n");
	}
	else
	{
		cout << "PCIe Device Open Success !!!" << endl;
	}
}

int Driver::getfd()
{
	return fd;
}