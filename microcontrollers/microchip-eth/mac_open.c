#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


/****************************************************************************
 * Function:        EthMACOpen
 *
 * PreCondition:    EthInit called!
 *
 * Input:           oFlags - open flags
 *                  pauseType - pause type to be supported by the MAC
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the open and configuration of the Eth MAC.
 *                  It needs to be called after an EthInit() was called and the PHY initialized.
 *
 *
 * Note:            - If no auto-negotiation is performed, the EthMACOpen() could be called immediately after EthInit() and PHY initialization.
 *                  - Otherwise the open flags and the pause type should be the ones obtained as a result of the PHY negotiation procedure!
 *****************************************************************************/
void EthMACOpen(eEthOpenFlags oFlags, eEthMacPauseType pauseType)
{
	unsigned int	cfg1;

	cfg1=_EMACxCFG1_RXENABLE_MASK|((oFlags&ETH_OPEN_MAC_LOOPBACK)?_EMACxCFG1_LOOPBACK_MASK:0);

	if(oFlags&ETH_OPEN_FDUPLEX)
	{
		if(pauseType&ETH_MAC_PAUSE_TYPE_EN_TX)
		{
			cfg1|=_EMACxCFG1_TXPAUSE_MASK;
		}
		if(pauseType&ETH_MAC_PAUSE_TYPE_EN_RX)
		{
			cfg1|=_EMACxCFG1_RXPAUSE_MASK;
		}
	}

	EMACxCFG1=cfg1;

	EMACxCFG2 = _EMACxCFG2_EXCESSDFR_MASK |
                _EMACxCFG2_AUTOPAD_MASK |
                _EMACxCFG2_PADENABLE_MASK |
                _EMACxCFG2_CRCENABLE_MASK |
                ((oFlags & ETH_OPEN_HUGE_PKTS) ?
                    _EMACxCFG2_HUGEFRM_MASK : 0) |
                _EMACxCFG2_LENGTHCK_MASK |
                ((oFlags & ETH_OPEN_HDUPLEX) ?
                    0 : _EMACxCFG2_FULLDPLX_MASK);


	EMACxIPGT=(oFlags&ETH_OPEN_HDUPLEX)?0x12:0x15;
	EMACxIPGR=(0xc<<_EMACxIPGR_NB2BIPKTGP1_POSITION)|(0x12<<_EMACxIPGR_NB2BIPKTGP2_POSITION);
	EMACxCLRT=(0x37<<_EMACxCLRT_CWINDOW_POSITION)|(0xf<<_EMACxCLRT_RETX_POSITION);


	if(oFlags&ETH_OPEN_RMII)
	{
		EMACxSUPP=_EMACxSUPP_RESETRMII_MASK;	// reset the RMII module

		EMACxSUPP=(oFlags&ETH_OPEN_100)?_EMACxSUPP_SPEEDRMII_MASK:0;	// release the reset and configure RMII
	}
}

#endif	// _ETH
