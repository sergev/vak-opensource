#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


/****************************************************************************
 * Function:        EthFCSetPauseValue
 *
 * PreCondition:    EthInit should have been called.
 *                  pauseBytes > 64, pauseBytes < 4194304
 *
 * Input:           pauseBytes	- the number of bytes to pause
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the value of the pause value to be used with manual or auto flow control.
 *                  The pause time is calculated as being the time needed to transmit pauseBytes bytes.
 *
 * Note:            The pauseBytes value is relevant for full duplex mode only.
 *                  In half duplex, back pressure is applied.
 *                  Should be called only when receive is not enabled (i.e. before EthRxBuffersAppend() is called, for example).
 *****************************************************************************/
void EthFCSetPauseValue(unsigned int pauseBytes)
{
	ETHCON1CLR=_ETHCON1_PTV_MASK;		// PTV=0
	ETHCON1SET=((pauseBytes*8)/512)<<_ETHCON1_PTV_POSITION;		// transform to 512 bits time units
}



/***************************************************************************
 * Function:        EthFCEnable
 *
 * PreCondition:    EthMACOpen, EthFCSetPauseValue should have been called.
 *
 * Input:           fcType - type of the flow control desired: auto or software (manual)
 *                  enable - boolean to enable/disable the flow control
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        - When enabling software flow control:
 *                    In full duplex mode, this function immediately enables the manual flow control and sends a pause frame
 *                    with the desired pause time.
 *                    Further on, a pause frame is resent, every interval corresponding to pause time/2.
 *                    In half duplex mode, back pressure is applied.
 *                  - When disabling software flow control:
 *                    In full duplex, this function immediately disables the manual flow control and sends a pause frame with the pause time set to 0.
 *                    In half duplex, the back pressure is removed.
 *                  - When enabling the auto flow control:
 *                    As soon as the number of the unacknowledged received descriptors (BUFCNT=EthDescriptorsGetRxUnack()) becomes >= fullWM:
 *                      In full duplex mode a pause frame is sent with the specified pause time (EthFCSetPauseValue).
 *                      Further on, a pause frame is resent, every interval corresponding to pause time/2.
 *                      In half duplex mode, back pressure is applied.
 *                    As soon as the BUFCNT becomes = emptyWM:
 *                      In full duplex mode a pause frame is sent with the pause value set to 0.
 *                      In half duplex mode, the back pressure is removed.
 *                  - When disabling suto flow control:
 *                    In full duplex, this function immediately disables the flow control and sends a pause frame with the pause time set to 0.
 *                    In half duplex, the back pressure is removed.
 *
 * Note:            BUFCNT is decremented by a call to EthRxAckPacket().
 *****************************************************************************/
void EthFCEnable(eEthFc fcType, int enable)
{
	unsigned int fcMask;

	fcMask=(fcType==ETH_FC_AUTO)?_ETHCON1_AUTOFC_MASK:_ETHCON1_MANFC_MASK;

	if(enable)
	{
		ETHCON1SET=fcMask;
	}
	else
	{
		ETHCON1CLR=fcMask;
	}
}





/****************************************************************************
 * Function:        EthFCSetRxWMark
 *
 * PreCondition:    EthInit, should have been called.
 *                  0<= emptyWM < fullWM < 256
 *
 * Input:           fullWM	- the full watermark value
 *                  emptyWM	- the empty watermark value
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the values for the watermarks used in the automatic flow control.
 *
 * Note:            See the description of the automatic flow control in the EthFCEnable description.
 *                  To take into account the delays inherent to the traffic stop, the value for fullWM
 *                  should be set in such a way that the automatic flow control kicks in
 *                  when there's less than 3 frames (3*1536 bytes) available in the receive descriptors.
 *****************************************************************************/
void EthFCSetRxWMark(int fullWM, int emptyWM)
{
	unsigned int	wReg=(emptyWM<<_ETHRXWM_RXEWM_POSITION)|(fullWM<<_ETHRXWM_RXFWM_POSITION);
	ETHRXWM=wReg;
}



#endif	// _ETH
