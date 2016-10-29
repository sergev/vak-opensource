#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


/****************************************************************************
 * Function:        _EthTxSchedBuffer
 *
 * PreCondition:    pBuff, nBytes, pList - valid
 *
 * Input:           pBuff   - adress of buffer to be sent
 *                  nBytes  - size of the buffer in bytes
 *                  pList   - list to append to
 *
 * Output:          ETH_RES_OK for success,
 *                  an error code otherwise
 *
 * Side Effects:    None
 *
 * Overview:        This function schedules the supplied buffer for transmission.
 *
 * Note:            None
 *****************************************************************************/
static eEthRes _EthTxSchedBuffer(const void* pBuff, unsigned short int nBytes, sEthDcptList* pList)
{
	sEthDNode	*pEDcpt;
	eEthRes		res;

	if(!IS_KVA(pBuff))
	{
		return ETH_RES_USPACE_ERR;
	}

	pEDcpt=SlRemoveHead(&_EthTxFreeList);

	if(pEDcpt)
	{	// ok valid descriptor
		// pas it to hw, always use linked descriptors, set proper size
		pEDcpt->hwDcpt.pEDBuff=(unsigned char*)KVA_TO_PA(pBuff);
		pEDcpt->hwDcpt.hdr.w=_SDCPT_HDR_NPV_MASK_|_SDCPT_HDR_EOWN_MASK_|(nBytes<<_SDCPT_HDR_BCOUNT_POS_);	// hw owned

		if(IS_KVA0(pBuff))
		{
			pEDcpt->hwDcpt.hdr.kv0=1;
		}
		SlAddTail(pList, pEDcpt);
		res=ETH_RES_OK;
	}
	else
	{	// we've run out of descriptors...
		res=ETH_RES_NO_DESCRIPTORS;
	}

	return res;
}

/****************************************************************************
 * Function:        _EthTxSchedList
 *
 * PreCondition:    pList - valid
 *
 * Input:           pBuff   - adress of buffer to be sent
 *                  nBytes  - size of the buffer in bytes
 *                  pList   - list to append to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function schedules the supplied list for transmission.
 *
 * Note:            None
 *****************************************************************************/
static void _EthTxSchedList(sEthDcptList* pList)
{
	if(!SlIsEmpty(pList))
	{
		(pList->head)->hwDcpt.hdr.SOP=1;
		(pList->tail)->hwDcpt.hdr.EOP=1;
		_EthAppendBusyList(&_EthTxBusyList, pList, 0);

		if(ETHTXST==0)
		{	// 1st time transmission!
			ETHTXST=KVA_TO_PA(&_EthTxBusyList.head->hwDcpt);
		}
		ETHCON1SET=_ETHCON1_TXRTS_MASK;		// and we're running!
	}

}



/****************************************************************************
 * Function:        EthTxSendBuffer
 *
 * PreCondition:    EthMACOpen, EthDescriptorsAdd should have been called.
 *                  buffers specified should be <2048 bytes in size
 *
 * Input:           pBuff   - adress of buffer to be sent
 *                  nBytes  - size of the buffer in bytes
 *
 * Output:          ETH_RES_OK for success,
 *                  an error code otherwise
 *
 * Side Effects:    None
 *
 * Overview:        This function schedules the supplied buffer for transmission.
 *                  To transmit this buffer an associated transmit descriptor is needed.
 *                  Therefore, a transmit descriptor should be available for this function to succeed.
 *                  Once a transmission is scheduled and done, EthTxGetBufferStatus() can be called to check the transmission result.
 *
 * Note:            - Not multithreaded safe. Don't call from from both ISR -non ISR code or multiple ISR's!
 *                  - This function enables the Ethernet transmission.
 *                  - This function is for transmission of packets completely assembled in one buffer.
 *****************************************************************************/
eEthRes EthTxSendBuffer(const void* pBuff, unsigned short int nBytes)
{
	eEthRes			res;
	sEthDcptList		newList={0, 0};

    res=_EthTxSchedBuffer(pBuff, nBytes, &newList);
    if(res==ETH_RES_OK)
    {
        _EthTxSchedList(&newList);
    }
    else if(!SlIsEmpty(&newList))
    {	// we failed, put back the removed nodes
        SlAppendTail(&_EthTxFreeList, &newList);
    }

	return res;

}



/****************************************************************************
 * Function:        EthTxSendPacket
 *
 * PreCondition:    EthMACOpen, EthDescriptorsAdd should have been called.
 *                  buffers specified should be <2048 bytes in size
 *
 * Input:           pPkt   - packet descriptor consisting of a list of buffer descriptors, terminated by a NULL pointer.
 *
 * Output:          ETH_RES_OK for success,
 *                  an error code otherwise
 *
 * Side Effects:    None
 *
 * Overview:        This function schedules the supplied packet for transmission.
 *                  Each packet can consist of multiple buffers, each having a different size, as specified in the packet descriptor.
 *                  Each buffer will need an associated transmit descriptor. Therefore, the number of needed
 *                  transmit descriptors should be available for this function to succeed.
 *                  Once a transmission is scheduled and done, EthGetTxPktStatus() can be called to check the transmission result.
 *
 * Note:            - Not multithreaded safe. Don't call from from both ISR -non ISR code or multiple ISR's!
 *                  - This function enables the Ethernet transmission.
 *                  - The buffers assembled for the packet stops when a NULL buffer descriptor is retrieved or the list ends
 *****************************************************************************/
eEthRes EthTxSendPacket(const sEthPktDcpt* pPkt)
{
	eEthRes			res;
	sEthDcptList    newList={0, 0};

	res=ETH_RES_OK;
	while(pPkt!=0 && pPkt->pBuff!=0 && pPkt->nBytes!=0 && res==ETH_RES_OK)
	{

		res=_EthTxSchedBuffer(pPkt->pBuff, pPkt->nBytes, &newList);
		pPkt=pPkt->next;	// next buffer in packet
	}


	if(res==ETH_RES_OK)
	{	// all's well
		_EthTxSchedList(&newList);
	}
	else if(!SlIsEmpty(&newList))
	{	// we failed, put back the removed nodes
		SlAppendTail(&_EthTxFreeList, &newList);
	}

	return res;
}



/****************************************************************************
 * Function:        EthTxGetBufferStatus
 *
 * PreCondition:    EthTxSendPacket should have been called.
 *                  pBuff valid pointer
 *
 * Input:           pBuff   - buffer to query the status for
 *                  pTxStat - address to store the pointer to the packet status or NULL if status not needed
 *
 * Output:          ETH_RES_OK if the packet was transmitted OK,
 *                  ETH_RES_PACKET_QUEUED if the packet is not done yet
 *                  ETH_RES_NO_PACKET if no such packet exists
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the status of a transmitted buffer/packet.
 *                  The transmission of this packet has to have been completed otherwise the call will fail.
 *
 * Note:            The returned pointer to the packet status is invalid after the EthTxAckPacket() is called.
 *                  pBuff must be the pointer to the first buffer in the packet, if the packet spans multiple buffers.
 *****************************************************************************/
eEthRes EthTxGetBufferStatus(const void* pBuff, const sEthTxPktStat** pTxStat)
{
	sEthDNode	*pHead;
	eEthRes		res;


	if(pTxStat)
	{
		*pTxStat=0;
	}

	if((pHead=_EthFindPacket(pBuff, &_EthTxBusyList)))
	{
		if(pHead->hwDcpt.hdr.EOWN)
		{
			res=ETH_RES_PACKET_QUEUED;	// not done
		}
		else
		{	// that's it, got it! The 1st descriptor to be updated by the hw is the packet header!
			if(pTxStat)
			{
				*pTxStat=(const sEthTxPktStat*)&((sEthTxDNode*)pHead)->hwDcpt.stat;
			}
			res=ETH_RES_OK;
		}
	}
	else
	{
		res=ETH_RES_NO_PACKET;
	}

	return res;
}


/****************************************************************************
 * Function:        _EthFindPacket
 *
 * PreCondition:    None
 *
 * Input:           pBuff	- packet to search
 *                  pList	- list to look into
 *
 * Output:          packet pointer or 0
 *
 * Side Effects:    None
 *
 * Overview:        This function is a helper to find a specific packet in a list.
 *
 * Note:            None
 *****************************************************************************/
sEthDNode* _EthFindPacket(const void* pBuff, sEthDcptList* pList)
{
	sEthDNode*	pEDcpt;
	const void*	pPhysPkt;

	pPhysPkt=(const void*)KVA_TO_PA(pBuff);

	for(pEDcpt=pList->head; pEDcpt!=0; pEDcpt=pEDcpt->next)
	{
		if(pEDcpt->hwDcpt.hdr.SOP)
		{ // found the beg of the packet
			if(pPhysPkt==pEDcpt->hwDcpt.pEDBuff)
			{	// ok, found it
				break;
			}
		}
	}

	return pEDcpt;
}



#endif	// _ETH
