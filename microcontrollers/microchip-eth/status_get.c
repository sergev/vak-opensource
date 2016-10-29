#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


/****************************************************************************
 * Function:        EthStatusGet
 *
 * PreCondition:    EthInit should have been called.
 *
 * Input:           None
 *
 * Output:          current ethernet controller status
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the current controller status.
 *
 * Note:            None
 *****************************************************************************/
eEthStat EthStatusGet(void)
{
	return (ETHSTAT & (_ETHSTAT_RXBUSY_MASK | _ETHSTAT_TXBUSY_MASK | _ETHSTAT_ETHBUSY_MASK))
            >> _ETHSTAT_RXBUSY_POSITION;
}


#endif	// _ETH
