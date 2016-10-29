#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present

/*********************************************************************
 * Function:        void EthMACConfig(eEthMACConfig1Flags c1Flags, eEthMACConfig2Flags c2Flags, eEthMACConfigSuppFlags sFlags)
 *
 * PreCondition:    c1Flags, c2Flags, sFlags - valid configuration flags
 *
 * Input:           c1Flags, c2Flags, sFlags - configuration flags
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function gives access to low level Eth MAC configuration.
 *                  It configures the MAC.
 *
 * Note:            None
 *
 * Example:         EthMACConfig(ETH_MAC_CONFIG1_RXEN|ETH_MAC_CONFIG1_RX_PAUSE|ETH_MAC_CONFIG1_TX_PAUSE, ETH_MAC_CONFIG2_FDUPLEX|ETH_MAC_CONFIG2_HUGE_FRAME, ETH_MAC_CONFIG_SUPP_RMII_SPEED);
 ********************************************************************/
void EthMACConfig(eEthMACConfig1Flags c1Flags, eEthMACConfig2Flags c2Flags, eEthMACConfigSuppFlags sFlags)
{
	EMACxCFG1=c1Flags;
	EMACxCFG2=c2Flags;
    EMACxSUPP=sFlags;
}


/*********************************************************************
 * Function:        void EthMACConfigSetFlags(eEthMACConfig1Flags c1Flags, eEthMACConfig2Flags c2Flags, eEthMACConfigSuppFlags sFlags)
 *
 * PreCondition:    c1Flags, c2Flags, sFlags - valid configuration flags
 *
 * Input:           c1Flags, c2Flags, sFlags - configuration flags to be set
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function gives access to low level Eth MAC configuration.
 *                  Any flag set in the c1Flags, c2Flags, sFlags will be set, the other flags won't be touched.
 *
 * Note:            None
 *
 * Example:         EthMACConfigSetFlags(ETH_MAC_CONFIG1_RXEN|ETH_MAC_CONFIG1_RX_PAUSE|ETH_MAC_CONFIG1_TX_PAUSE, ETH_MAC_CONFIG2_FDUPLEX|ETH_MAC_CONFIG2_HUGE_FRAME, ETH_MAC_CONFIG_SUPP_RMII_RESET);
 ********************************************************************/
void EthMACConfigSetFlags(eEthMACConfig1Flags c1Flags, eEthMACConfig2Flags c2Flags, eEthMACConfigSuppFlags sFlags)
{
	EMACxCFG1SET=c1Flags;
	EMACxCFG2SET=c2Flags;
    EMACxSUPPSET=sFlags;
}


/*********************************************************************
 * Function:        void EthMACConfigClrFlags(eEthMACConfig1Flags c1Flags, eEthMACConfig2Flags c2Flags, eEthMACConfigSuppFlags sFlags)
 *
 * PreCondition:    c1Flags, c2Flags, sFlags - valid configuration flags
 *
 * Input:           c1Flags, c2Flags, sFlags - configuration flags to be cleared
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function gives access to low level Eth MAC configuration.
 *                  Any flag set in the c1Flags, c2Flags, sFlags will be cleared, the other flags won't be touched.
 *
 * Note:            None
 *
 * Example:         EthMACConfigClrFlags(ETH_MAC_CONFIG1_RXEN|ETH_MAC_CONFIG1_RX_PAUSE|ETH_MAC_CONFIG1_TX_PAUSE, ETH_MAC_CONFIG2_FDUPLEX|ETH_MAC_CONFIG2_HUGE_FRAME, ETH_MAC_CONFIG_SUPP_RMII_RESET);
 ********************************************************************/
void EthMACConfigClrFlags(eEthMACConfig1Flags c1Flags, eEthMACConfig2Flags c2Flags, eEthMACConfigSuppFlags sFlags)
{
	EMACxCFG1CLR=c1Flags;
	EMACxCFG2CLR=c2Flags;
    EMACxSUPPCLR=sFlags;
}

/*********************************************************************
 * Function:        void EthMACConfigB2BIpGap(unsigned int ipGap)
 *
 * PreCondition:    0 < ipGap < 128
 *
 * Input:           ipGap - the back to back inter packet gap
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function sets the back to back inter packet gap value (the nibble time offset
 *                  of the minimum possible period between the end of any transmitted packet to the beginning of the next).
 *
 * Note:            - In full duplex the recommended value is 0x15
 *                  - in half duplex the recommended value is 0x12
 *
 * Example:         EthMACConfigB2BIpGap(0x15);
 ********************************************************************/
void EthMACConfigB2BIpGap(unsigned int ipGap)
{
	EMACxIPGT=ipGap;
}

/*********************************************************************
 * Function:        void EthMACConfigNB2BIpGap(unsigned int ipgr1, unsigned int ipgr2)
 *
 * PreCondition:    0 < carrDefer < 128
 *                  0 < nB2BIpGap < 128
 *
 * Input:           carrDefer - the carrier deference
 *                  nB2BIpGap - non back to back inter packet gap
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function sets the non back to back inter packet gap value part 1 and 2.
 *
 *                  The ipgr1 represents the optional carrierSense window referenced in IEEE 802.3/4.2.3.2.1 ‘Carrier Deference’.
 *                  If carrier is detected during the timing of IPGR1, the MAC defers to carrier. If, however, carrier becomes after IPGR1,
 *                  the MAC continues timing IPGR2 and transmits, knowingly causing a collision, thus ensuring fair access to medium.
 *                  The ipgr2 represents the non back to back inter packet gap.
 *
 * Note:            - the ipgr1 recommended value is 0xc. Its range of values is 0x0 to ipgr2
 *                  - the ipgr2 recommended value is 0x12
 *
 * Example:         EthMACConfigNB2BIpGap(0xc, 0x12);
 ********************************************************************/
void EthMACConfigNB2BIpGap(unsigned int ipgr1, unsigned int ipgr2)
{
	__EMACxIPGRbits_t	ipgrB;

	ipgrB.w=0;
	ipgrB.NB2BIPKTGP2=ipgr2;
	ipgrB.NB2BIPKTGP1=ipgr1;

	EMACxIPGR=ipgrB.w;
}


/*********************************************************************
 * Function:        void EthMACConfigCollWinRetry(unsigned int collWin, unsigned int retMax)
 *
 * PreCondition:    0 < collWin < 64
 *                  0 < retMax < 16
 *
 * Input:           collWin - the collision window
 *                  retMax   - retransmission maximum
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function sets the collision window and the retransmission max.
 *
 *                  The collision window is a programmable field representing the slot time or collision window
 *                  during which collisions occur in properly configured networks.
 *                  Since the collision window starts at the beginning of transmission, the preamble and SFD is included.
 *
 *                  The retransmission maximum is a programmable field specifying the number of retransmission attempts
 *                  following a collision before aborting the packet due to excessive collisions.
 *
 * Note:            - the collWin recommended value is 0x37 which corresponds to the count of frame bytes
 *                    at the end of the window.
 *                  - the retMax recommended value is 0xf
 *
 * Example:         EthMACConfigCollWinRetry(0x37, 0xf);
 ********************************************************************/
void EthMACConfigCollWinRetry(unsigned int collWin, unsigned int retMax)
{
	__EMACxCLRTbits_t clrtB;

	clrtB.w=0;
	clrtB.CWINDOW=collWin;
	clrtB.RETX=retMax;

	EMACxCLRT=clrtB.w;
}



#endif	// _ETH
