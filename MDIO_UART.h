#pragma once
#define MDIO_CMD_WRITE 1
#define MDIO_CMD_READ 2
#pragma pack (push,1)
struct MdioUartStruct
{
unsigned	phyAddr : 5;
unsigned	regAddr : 5;
unsigned	reserved : 4;
unsigned 	cmd :2;
unsigned	data : 16;
};
#pragma pack (pop)