#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present

#include <string.h>

/****************************************************************************
 * Function:        EthMACSetAddress
 *
 * PreCondition:    None
 *
 * Input:           bAddress - standard MAC address, 6 bytes, Network order!
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the MAC address.
 *
 * Note:            The default MAC address is loaded by the device at reset from the fuses.
 *****************************************************************************/
void EthMACSetAddress(unsigned char bAddress[6])
{
    union
    {
        double          align;          // 8 bytes aligned
        unsigned char   addr[6];        // MAC address, network order
    }uA;       // aligned MAC address

    memcpy(uA.addr, bAddress, sizeof(uA.addr)); // align properly
    unsigned short* pS=(unsigned short*)uA.addr;
	EMACxSA2=*pS++;
	EMACxSA1=*pS++;
	EMACxSA0=*pS;
}


/****************************************************************************
 * Function:        EthMACGetAddress
 *
 * PreCondition:    None
 *
 * Input:           bAddress - address to store a standard MAC address, 6 bytes, Network order!
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the current MAC address.
 *
 * Note:            None
 *****************************************************************************/
void EthMACGetAddress(unsigned char bAddress[6])
{
    union
    {
        double          align;          // 8 bytes aligned
        unsigned char   addr[6];        // MAC address, network order
    }uA;       // aligned MAC address

    unsigned short* pS=(unsigned short*)uA.addr;    // align properly
	*pS++=EMACxSA2;
	*pS++=EMACxSA1;
	*pS=EMACxSA0;

    memcpy(bAddress, uA.addr, sizeof(uA.addr));     // copy to user
}



/****************************************************************************
 * Function:        EthMACSetMaxFrame
 *
 * PreCondition:    Should be called after EthInit()
 *
 * Input:           maxFrmSz - maximum frame for the MAC to tx/rx
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the MAC maximum frame size.
 *
 * Note:            The default MAC frame size (0x600) is set by the EthInit() call.
 *****************************************************************************/
void EthMACSetMaxFrame(unsigned short maxFrmSz)
{

	EMACxMAXF=maxFrmSz;
}


#endif	// _ETH
