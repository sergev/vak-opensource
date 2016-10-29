#include "_eth_include.h"

#ifdef _ETH	// the Ethernet device is present

/****************************************************************************
 * Function:        EthClose
 *
 * PreCondition:    None
 *
 * Input:           cFlags - closing flags
 *
 * Output:          ETH_RES_OK
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the abort of any current Eth transaction.
 *                  It disables the Eth controller and the interrupts.
 *                  It should be called after the PHY close/reset procedure.
 *
 * Note:            It does not free any allocated memory.
 *****************************************************************************/
eEthRes EthClose(eEthCloseFlags cFlags)
{
	// disable Rx, Tx, Eth controller itself

	INTEnable(INT_ETHERNET, INT_DISABLED);	// disable ethernet interrupts

	if(cFlags&ETH_CLOSE_GRACEFUL)
	{
		ETHCON1CLR=_ETHCON1_TXRTS_MASK;	// stop tx
		while(ETHSTATbits.TXBUSY);	// wait tx down
		while(ETHSTATbits.RXBUSY);	// wait rx down
	}

	ETHCON1CLR=_ETHCON1_TXRTS_MASK|_ETHCON1_RXEN_MASK;	// stop rx/tx

	_EthMacReset();

	ETHCON1CLR=_ETHCON1_ON_MASK;	// disable it
	while(ETHSTATbits.ETHBUSY);	// wait everything down

	INTClearFlag(INT_ETHERNET);	// clear the interrupt controller flag
	ETHIRQCLR=0xffffffff;		// clear any existing interrupt event

	return ETH_RES_OK;
}
#endif	// _ETH
