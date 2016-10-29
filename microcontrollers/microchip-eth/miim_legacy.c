#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


static const short _EthMIIClkDivTbl[]=
{
	4, 6, 8, 10, 14, 20, 28, 40
};	// divider values for the Host clock


// definitions
//

/****************************************************************************
 * Function:        mRegIxPhyAddToEMACxMADR
 *
 * PreCondition:    rIx a valid PHY register, 0-31
 *                  phyAdd a valid PHY address, 0-31
 *
 * Input:           rIx: 	PHY register to be accessed
 *                  phyAdd:	PHY to be accessed
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This macro converts a register index and PHY address to a EMACxMADR format;
 *
 * Note:            None
 *****************************************************************************/
#define		mRegIxPhyAddToEMACxMADR(rIx, phyAdd)	((rIx)<<_EMACxMADR_REGADDR_POSITION)|((phyAdd)<<_EMACxMADR_PHYADDR_POSITION)



// interface functions
//

/****************************************************************************
 * Function:        EthMIIMInit
 *
 * PreCondition:    None
 *
 * Input:           hostClock: the value of the host clock, Hz
 *                  miimClock: the maximum clock frequency that the PHY can use for the MIIM transactions
 *                  oFlags:    opening flags to select the 10/100 Mbps RMII/MII speed
 *                  isRmii:    boolean to select the RMII/MII operation mode
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function initializes the MIIM interface for access of a PHY.
 *
 * Note:            - Any previous MIIM operation will be aborted!
 *                  - only ETH_OPEN_RMII and ETH_OPEN_100/ETH_OPEN_10 flags are relevant.
 *****************************************************************************/
void EthMIIMInit(unsigned int hostClock, unsigned int miimClock, eEthOpenFlags oFlags, int isRmii)
{
	int		ix;

	if(isRmii)
	{
		EMACxSUPP=_EMACxSUPP_RESETRMII_MASK;	// reset the RMII module

		EMACxSUPP=(oFlags&ETH_OPEN_100)?_EMACxSUPP_SPEEDRMII_MASK:0;	// release the reset and configure RMII
	}

	EMACxMCFG=_EMACxMCFG_RESETMGMT_MASK;	// issue MIIM reset

	EMACxMCFG=0;	// clear reset

	for(ix=0; ix<sizeof(_EthMIIClkDivTbl)/sizeof(*_EthMIIClkDivTbl); ix++)
	{
		if(hostClock/_EthMIIClkDivTbl[ix]<=miimClock)
		{	// found it
			break;
		}
	}
	if(ix==sizeof(_EthMIIClkDivTbl)/sizeof(*_EthMIIClkDivTbl))
	{
		ix--;	// max divider; best we can do
	}

	EMACxMCFG=(ix+1)<<_EMACxMCFG_CLKSEL_POSITION;	// program the clock
}


/****************************************************************************
 * Function:        EthMIIMReadReg
 *
 * PreCondition:    rIx a valid PHY register, 0-31
 *                  phyAdd a valid PHY address, 0-31
 *
 * Input:           rIx: PHY register to be accessed
 *                  phyAdd: PHY address
 *
 * Output:          PHY register value
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a read operation from a PHY register using the MIIM interface.
 *
 * Note:            It assumes that no scan operation is in progress!
 *****************************************************************************/
unsigned short EthMIIMReadReg(unsigned int rIx, unsigned int phyAdd)
{
	while(EMACxMINDbits.MIIMBUSY);		// wait in case of some previous operation

	EMACxMADR=mRegIxPhyAddToEMACxMADR(rIx, phyAdd);
	EMACxMCMD=_EMACxMCMD_READ_MASK;			// read order
	__asm__ __volatile__ ("nop; nop; nop; nop;");	// there's 4 clock cycles till busy is set for a read op
	while(EMACxMINDbits.MIIMBUSY);			// wait op complete
	EMACxMCMD=0;
	return EMACxMRDD;		// the read register
}


/****************************************************************************
 * Function:        EthMIIMWriteReg
 *
 * PreCondition:    rIx a valid PHY register, 0-31
 *                  phyAdd a valid PHY address, 0-31
 *
 * Input:           rIx:    PHY register to be accessed
 *                  phyAdd: PHY address
 * 		    wData:  data to be written
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a write operation to a PHY register using the MIIM interface.
 *                  It doesn't wait for the write to take effect.
 *
 * Note:            It assumes that no scan operation is in progress!
 *****************************************************************************/
void EthMIIMWriteReg(unsigned int rIx, unsigned int phyAdd, unsigned short wData)
{
	while(EMACxMINDbits.MIIMBUSY);		// wait in case of some previous operation

	EMACxMADR=mRegIxPhyAddToEMACxMADR(rIx, phyAdd);
	EMACxMWTD=wData;
	__asm__ __volatile__ ("nop; nop;");	// there's 2 clock cycles till busy is set for a write op
                                        // make sure busy doesn't get cj=hecked too soon!
}


/****************************************************************************
 * Function:        EthMIIMWriteRegWait
 *
 * PreCondition:    rIx a valid PHY register, 0-31
 *                  phyAdd a valid PHY address, 0-31
 *
 * Input:           rIx: 	PHY register to be accessed
 *                  phyAdd: PHY address
 * 		    wData:	data to be written
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a write operation to a PHY register using the MIIM interface.
 *                  It waits for the write to take effect.
 *
 * Note:            It assumes that no scan operation is in progress!
 *****************************************************************************/
void EthMIIMWriteRegWait(unsigned int rIx, unsigned int phyAdd, unsigned short wData)
{
	while(EMACxMINDbits.MIIMBUSY);		// wait in case of some previous operation

	EMACxMADR=mRegIxPhyAddToEMACxMADR(rIx, phyAdd);
	EMACxMWTD=wData;
	__asm__ __volatile__ ("nop; nop;");	// there's 2 clock cycles till busy is set for a write op
	while(EMACxMINDbits.MIIMBUSY);		// wait op complete
}


/****************************************************************************
 * Function:        EthMIIMScanRead
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function reads the result of a previously initiated scan operation from a PHY register using the MIIM interface.
 *
 * Note:            It assumes that a scan operation is in progress!
 *****************************************************************************/
unsigned short EthMIIMScanRead(void)
{
	while(EMACxMINDbits.NOTVALID);	// wait data valid

	return EMACxMRDD;		// the scan register
}

/****************************************************************************
 * Function:        EthMIIMScanStopWait
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function stops a previously initiated scan operation from a PHY register using the MIIM interface.
 *                  It waits for the scan operation to finish.
 *
 * Note:            It assumes that a scan operation is in progress!
 *****************************************************************************/
void EthMIIMScanStopWait(void)
{
	EMACxMCMD=0;			// cancel scan
	while(EMACxMINDbits.MIIMBUSY);		// wait scan done
}


#endif /* _ETH */
