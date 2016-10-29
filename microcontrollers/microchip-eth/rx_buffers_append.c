#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present

/****************************************************************************
 * Function:        EthRxBuffersAppend
 *
 * PreCondition:    EthInit, EthAddDescriptors, EthRxSetBufferSize should have been called.
 *                  each buffer supplied should be >= EthRxSetBufferSize().
 *
 * Input:           ppBuff - pointer to an array of buffers (could be NULL terminated) to be appended to the receiving process
 *                  nBuffs - number of buffers supplied (or 0 if ppBuff is NULL terminated)
 *                  rxFlags  - flags applied to all RX buffers passed
 *
 * Output:          ETH_RES_OK for success,
 *                  an error code otherwise
 *
 * Side Effects:    None
 *
 * Overview:        This function supplies buffers to the receiving process and enables the receiving part of the controller.
 *                  As soon as the controller starts receiving data packets these will be stored into memory
 *                  at the addresses specified by these buffers.
 *                  A received packet can consist of multiple buffers, split across buffers with the SAME size, as specified in the EthRxSetBufferSize().
 *                  Each buffer needs an associated receive descriptor. Therefore, the number of needed
 *                  receive descriptors should be available for this function to succeed.
 *                  Once a receive operation is scheduled, EthRxPacket() can be called to get the received packet.
 *
 * Note:            - Not multithreaded safe. Don't call from from both ISR -non ISR code or multiple ISR's!
 *                  - This function enables the Ethernet receiving.
 *                  - When a packet is split into multiple buffers, all buffers have the same size, set by the EthRxSetBufferSize().
 *                  - The append process continues until a NULL buffer pointer is retrieved or nBuffs buffers are appended.
 *                  - Only RX eEthBuffFlags are relevant for this function
 *****************************************************************************/
eEthRes EthRxBuffersAppend(void* ppBuff[], int nBuffs, eEthBuffFlags rxFlags)
{
	void*   	pBuff;
	sEthDNode	*pEDcpt;
	eEthRes		res=ETH_RES_OK;
	sEthDcptList	newList={0, 0};

	if(nBuffs==0)
	{
		nBuffs=0x7fffffff;
	}

	for(pBuff=*ppBuff; pBuff!=0 && nBuffs; pBuff=*(++ppBuff), nBuffs--)
	{
		pEDcpt=SlRemoveHead(&_EthRxFreeList);
		if(!pEDcpt)
		{	// we've run out of descriptors...
			res=ETH_RES_NO_DESCRIPTORS;
			break;
		}
		// ok valid descriptor
		// pas it to hw, always use linked descriptors
		pEDcpt->hwDcpt.pEDBuff=(unsigned char*)KVA_TO_PA(pBuff);

        pEDcpt->hwDcpt.hdr.w=_SDCPT_HDR_NPV_MASK_|_SDCPT_HDR_EOWN_MASK_;	// hw owned

        if(rxFlags&ETH_BUFF_FLAG_RX_STICKY)
		{
			pEDcpt->hwDcpt.hdr.sticky=1;
		}

        if(rxFlags&ETH_BUFF_FLAG_RX_UNACK)
		{
			pEDcpt->hwDcpt.hdr.rx_nack=1;
		}

        if(IS_KVA0(pBuff))
		{
			pEDcpt->hwDcpt.hdr.kv0=1;
		}
		else if(!IS_KVA(pBuff))
		{
			res=ETH_RES_USPACE_ERR;
			break;
		}

		SlAddTail(&newList, pEDcpt);
	}

	if(pBuff && nBuffs)
	{	// failed, still buffers in the descriptors, put back the removed nodes
		SlAppendTail(&_EthRxFreeList, &newList);
		return res;
	}

	// all's well
	if(!SlIsEmpty(&newList))
	{
		_EthAppendBusyList(&_EthRxBusyList, &newList, 1);

		if(ETHRXST==0)
		{	// 1st time reception!
			ETHRXST=KVA_TO_PA(&_EthRxBusyList.head->hwDcpt);
		}
		ETHCON1SET=_ETHCON1_RXEN_MASK;		// and we're running!
	}

	return ETH_RES_OK;

}


#endif	// _ETH
