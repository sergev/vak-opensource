#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present

// local prototypes
//
static int	_EthDescriptorsCount(sEthDcptList* pList, int isHwCtrl);



/****************************************************************************
 * Function:        EthDescriptorsQuery
 *
 * PreCondition:    - EthMACOpen should have been called.
 *                  - pQuery valid pointer
 *
 * Input:           dType   - tx/rx descriptors requested
 *                  pQuery  - address to store the result of the query
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function takes a snapshot of the current status of RX/TX descriptors.
 *                  It returns the query result.
 *                  If dType == ETH_DCPT_TYPE_RX
 *                         dIdle: returns the number of unused (idle) descriptors
 *                                   available in the pool of RX descriptors.
 *                         dUnack: returns the number of descriptors that received a valid
 *                                   packet but have not been acknowledged yet
 *                         dQueued: returns the number of descriptors that are just queued for
 *                                   receiving, without a valid load.
 *                  If dType == ETH_DCPT_TYPE_TX
 *                         dIdle: returns the number of unused (idle) descriptors
 *                                   available in the pool of TX descriptors.
 *                         dUnack: returns the number of descriptors that have transmitted
 *                                   packets but have not been acknowledged yet
 *                         dQueued: returns the number of descriptors that are just queued for
 *                                   transmission, but not transmitted yet
 *
 * Note:            - The returned numbers are info only. They can dynamically change.
 *                  - No protection against interrupts, multithreading, etc.
 *                  - ETH_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has to be used.
 *                  - Whenever the automatic flow control is turned on, the reception will come to a halt once the number of
 *                    unacknowledged RX descriptors (dUnack for query using ETH_DCPT_TYPE_RX) reaches 256.
 *                    Also the number of currently received and unacknowledged descriptors
 *                    control the behavior of the automatic flow control.
 *                    See EthFCEnable() for details.
 *****************************************************************************/
void EthDescriptorsQuery(eEthDcptType dType, sEthDcptQuery* pQuery)
{

	if(dType == ETH_DCPT_TYPE_RX)
	{
		pQuery->dUnack=ETHSTATbits.BUFCNT;
		pQuery->dQueued=_EthDescriptorsCount(&_EthRxBusyList, 1);	// count busy RX descriptors under hw control
		pQuery->dIdle=SlCount(&_EthRxFreeList);
	}
	else if (dType == ETH_DCPT_TYPE_TX)
	{
		pQuery->dUnack=_EthDescriptorsCount(&_EthTxBusyList, 0);	// count busy TX descriptors under sw control
		pQuery->dQueued=_EthDescriptorsCount(&_EthTxBusyList, 1);	// count busy TX descriptors under hw control
		pQuery->dIdle=SlCount(&_EthTxFreeList);
	}
	else
	{
		pQuery->dUnack=pQuery->dQueued=pQuery->dIdle=0;
	}

}


/****************************************************************************
 * Function:        EthDescriptorsGetSize
 *
 * PreCondition:    None
 *
 * Input:           dType    - tx/rx/all descriptors requested
 *
 * Output:          the size of a descriptor
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the number of bytes needed for the storage
 *                  of a descriptor in the transmit/receive engine.
 *
 * Note:            If ETH_DCPT_ALL supplied the function will return them size that accomodates both descriptors (max).
 *****************************************************************************/
int EthDescriptorsGetSize(eEthDcptType dType)
{
	int	txSize=0;
	int	rxSize=0;

	if(dType&ETH_DCPT_TYPE_TX)
	{
		txSize=sizeof(sEthTxDNode);
	}

	if(dType&ETH_DCPT_TYPE_RX)
	{
		rxSize=sizeof(sEthRxDNode);
	}

	return (txSize>rxSize)?txSize:rxSize;
}


/***********************************
 *     local functions
 ***********************************/

/****************************************************************************
 * Function:        _EthDescriptorsCount
 *
 * PreCondition:    None.
 *
 * Input:           pList  - a list (busy RX/TX) to perform the search into
 *                  isHwCtrl - boolean to specify if looking for descriptors under hw control or not
 *
 * Output:          the number of matching descriptors
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the number of matching descriptors in the selected TX/RX busy list.
 *
 * Note:            None
 *****************************************************************************/
static int _EthDescriptorsCount(sEthDcptList* pList, int isHwCtrl)
{
	sEthDNode	*pEDcpt;
	int	 	nDcpts=0;


	for(pEDcpt=pList->head; pEDcpt!=0 && pEDcpt->next!=0; pEDcpt=pEDcpt->next)
	{	// don't count the ending dummy descriptor
		if(pEDcpt->hwDcpt.hdr.EOWN == isHwCtrl)
		{
			nDcpts++;
		}
	}

	return nDcpts;
}


#endif	// _ETH
