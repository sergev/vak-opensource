#include "_eth_include.h"

#ifdef _ETH	// the Ethernet device is present

/****************************************************************************
 * Function:        EthInit
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          ETH_RES_OK
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the ETHC.
 *
 * Note:            This function should be called before the EthMACOpen()
 *****************************************************************************/
eEthRes EthInit(void)
{
	INTEnable(INT_ETHERNET, INT_DISABLED);	// disable ethernet interrupts

	ETHCON1CLR=_ETHCON1_ON_MASK|_ETHCON1_TXRTS_MASK|_ETHCON1_RXEN_MASK;	// reset everything

	while(ETHSTATbits.ETHBUSY);	// wait everything down
	ETHCON1SET=_ETHCON1_ON_MASK;	// turn device ON

	while(ETHSTATbits.BUFCNT)
	{
		ETHCON1SET=_ETHCON1_BUFCDEC_MASK;	// clear the ETHSTAT.BUFCNT and related interrupt flags
	}

	INTClearFlag(INT_ETHERNET);	// clear the interrupt controller flag
	ETHIRQCLR=0xffffffff;		// clear any existing interrupt event
	ETHTXSTCLR=0xffffffff;
	ETHRXSTCLR=0xffffffff;		// clear any start address

	// leave filtering and ETHIEN as they were

	_EthMacInit();

	return ETH_RES_OK;
}
#endif	// _ETH
