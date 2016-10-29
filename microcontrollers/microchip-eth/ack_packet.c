#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present

/****************************************************************************
 * Function:        _EthRxAckBuffer
 *
 * PreCondition:    EthRxSetBufferSize, EthRxBuffersAppend, EthRxGetPacket should have been called.
 *
 * Input:           pBuff  - buffer/packet to be acknowledged or NULL
 *                  ackFnc - function to be called for the acknowledged buffers or NULL
 *                  fParam - argument to be used in ackFnc callback
 *
 * Output:          ETH_RES_OK - success
 *                  ETH_RES_PACKET_QUEUED - there are packets in the receiving queue
 *                  ETH_RES_NO_PACKET - no packets available in the receiving queue
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a received buffer/packet.
 *                  The supplied packet has to have been previously received otherwise the call will fail or the packet will be discarded.
 *                  When pBuff==NULL, all currently received packets will be acknowledged.
 *                  The ackFnc, if !NULL, will be called for each buffer within the packet in turn.
 *
 * Note:            - Any received packet has to be acknowledged, otherwise the Eth API will run out of descriptors.
 *                  - pBuff must be the pointer to the first buffer in the packet, if the packet spans multiple buffers.
 *                  - ackFnc is just a helper that allows the application to call an acknowledge function for each received buffer in turn.
 *****************************************************************************/
static eEthRes _EthRxAckBuffer(const void* pBuff, pEthBuffAck ackFnc, void* fParam)
{
	eEthRes		res;
	sEthDNode*	pEDcpt;
	sEthDcptList	ackList={0, 0};
	sEthDcptList	stickyList={0, 0};

	res=_EthAckPacket(pBuff, &_EthRxBusyList, &ackList, ackFnc, fParam);

	while((pEDcpt=SlRemoveHead(&ackList)))
	{
		if(pEDcpt->hwDcpt.hdr.sticky)
		{
			// add it to the busy list...
			pEDcpt->hwDcpt.hdr.SOP=pEDcpt->hwDcpt.hdr.EOP=pEDcpt->hwDcpt.hdr.rx_wack=0;
			pEDcpt->hwDcpt.hdr.EOWN=1;	// hw owned
			SlAddTail(&stickyList, pEDcpt);
		}
		else
		{
			SlAddTail(&_EthRxFreeList, pEDcpt);
            if(!pEDcpt->hwDcpt.hdr.rx_nack)
            {
                ETHCON1SET=_ETHCON1_BUFCDEC_MASK;	// acknowledge the hardware;
            }
		}
	}

	if(!SlIsEmpty(&stickyList))
	{
		_EthAppendBusyList(&_EthRxBusyList, &stickyList, 1);	// append the descriptors that have valid buffers
	}

	return res;
}




/****************************************************************************
 * Function:        EthRxAcknowledgeBuffer
 *
 * PreCondition:    EthRxSetBufferSize, EthRxBuffersAppend, EthRxGetPacket should have been called.
 *
 * Input:           pBuff  - buffer/packet to be acknowledged or NULL
 *                  ackFnc - function to be called for the acknowledged buffers or NULL
 *
 * Output:          ETH_RES_OK - success
 *                  ETH_RES_PACKET_QUEUED - there are packets in the receiving queue
 *                  ETH_RES_NO_PACKET - no packets available in the receiving queue
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a received buffer/packet.
 *                  The supplied packet has to have been previously received otherwise the call will fail or the packet will be discarded.
 *                  When pBuff==NULL, all currently received packets will be acknowledged.
 *                  The ackFnc, if !NULL, will be called for each buffer within the packet in turn.
 *
 * Note:            - Any received packet has to be acknowledged, otherwise the Eth API will run out of descriptors.
 *                  - pBuff must be the pointer to the first buffer in the packet, if the packet spans multiple buffers.
 *                  - ackFnc is just a helper that allows the application to call an acknowledge function for each received buffer in turn.
 *****************************************************************************/
eEthRes EthRxAcknowledgeBuffer(const void* pBuff, pEthBuffAck ackFnc, void* fParam)
{
    return _EthRxAckBuffer(pBuff, ackFnc, fParam);
}


/****************************************************************************
 * Function:        EthTxAcknowledgeBuffer
 *
 * PreCondition:    EthTxSendPacket should have been called.
 *
 * Input:           pBuff  - buffer/packet to be acknowledged or NULL.
 *                  ackFnc - function to be called for the acknowledged buffers	or NULL
 *                  fParam - argument to be used in function call
 *
 * Output:          ETH_RES_OK - success
 *                  ETH_RES_PACKET_QUEUED - there are packets scheduled to be transmitted
 *                  ETH_RES_NO_PACKET - no packets scheduled for transmit
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a transmitted buffer/packet.
 *                  The transmission of this packet has to have been completed otherwise the call will fail.
 *                  When pBuff==NULL, all currently transmitted packets will be acknowledged.
 *                  The ackFnc, if !NULL, will be called for each buffer within the packet in turn.
 *
 * Note:            - Any transmitted packet has to be acknowledged, otherwise the Eth API will run out of transmission descriptors.
 *                  - pBuff must be the pointer to the first buffer in the packet, if the packet spans multiple buffers.
 *                  - ackFnc is just a helper that allows the application to acknowledge the transmitted buffers without the need
 *                  to maintain a list of buffers scheduled for transmission (this list is maintained by the Eth library anyway).
 *                  Useful especially when the transmission result is not really needed and the function is called with pPkt=0.
 *****************************************************************************/
eEthRes EthTxAcknowledgeBuffer(const void* pBuff, pEthBuffAck ackFnc, void* fParam)
{
	return _EthAckPacket(pBuff, &_EthTxBusyList, &_EthTxFreeList, ackFnc, fParam);
}


/****************************************************************************
 * Function:        _EthAckPacket
 *
 * PreCondition:    None
 *
 * Input:           pPkt       - buffer/packet to be acknowledged or NULL
 *                  pRemList   - list to look for done packets and to remove the packets from
 *                  pAddList   - list were to add the removed packets
 *                  ackFnc     - function to be called for each acknowledged buffer in turn
 *                  fParam     - function argument
 *
 * Output:          ETH_RES_OK - success
 *                  ETH_RES_PACKET_QUEUED - there are packets queued
 *                  ETH_RES_NO_PACKET - no packets available in the queue
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a packet.
 *                  The supplied packet has to have been completed otherwise the call will fail.
 *                  When pPkt==NULL, all packets with EOWN==0 will be acknowledged.
 *
 * Note:            None
 *****************************************************************************/
eEthRes _EthAckPacket(const void* pPkt, sEthDcptList* pRemList, sEthDcptList* pAddList, pEthBuffAck ackFnc, void* fParam)
{

	sEthDNode	*pEDcpt;
	sEthDNode	*prev, *next;
	int		nAcks;
	int		pktFound;
	int		buffIx;

	prev=next=0;
	nAcks=pktFound=0;

	for(pEDcpt=pRemList->head; pEDcpt!=0; pEDcpt=next)
	{
		if(pEDcpt->hwDcpt.hdr.SOP && (pPkt==0 || pEDcpt->hwDcpt.pEDBuff==(unsigned char*)KVA_TO_PA(pPkt)))
		{ // found the beg of a packet
			pktFound=1;

			if(pEDcpt->hwDcpt.hdr.EOWN)
			{
				break;		// hw not done with it
			}

			next=pEDcpt;
			buffIx=0;
			do
			{
				pEDcpt=next;
				next=pEDcpt->next;
				while(pEDcpt->hwDcpt.hdr.EOWN);			// shouldn't happen
				SlAddTail(pAddList, pEDcpt);	// ack this node
				if(ackFnc)
				{
					void* pBuff;
					pBuff=(pEDcpt->hwDcpt.hdr.kv0?PA_TO_KVA0((int)pEDcpt->hwDcpt.pEDBuff):PA_TO_KVA1((int)pEDcpt->hwDcpt.pEDBuff));
					(*ackFnc)(pBuff, buffIx++, fParam);	// call user's acknowledge
				}

			}while(!pEDcpt->hwDcpt.hdr.EOP);

			nAcks++;
			// reconstruct the removed list
			if(prev)
			{
				prev->next=next;
				// prev->next_ed shouldn't matter here!
			}
			else
			{
				pRemList->head=next;
			}

			if(pPkt)
			{	// done, just one packet ack-ed
				break;	// done
			}
		}
		else
		{
			prev=pEDcpt;
			next=pEDcpt->next;
		}
	}

	return nAcks?ETH_RES_OK:(pktFound?ETH_RES_PACKET_QUEUED:ETH_RES_NO_PACKET);


}


/********************
 * legacy functions
 ***********************/



/****************************************************************************
 * Function:        EthRxAckBuffer
 *
 * PreCondition:    EthRxSetBufferSize, EthRxBuffersAppend, EthRxGetPacket should have been called.
 *
 * Input:           pBuff  - buffer/packet to be acknowledged or NULL
 *                  ackFnc - function to be called for the acknowledged buffers or NULL
 *
 * Output:          ETH_RES_OK - success
 *                  ETH_RES_PACKET_QUEUED - there are packets in the receiving queue
 *                  ETH_RES_NO_PACKET - no packets available in the receiving queue
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a received buffer/packet.
 *                  The supplied packet has to have been previously received otherwise the call will fail or the packet will be discarded.
 *                  When pBuff==NULL, all currently received packets will be acknowledged.
 *                  The ackFnc, if !NULL, will be called for each buffer within the packet in turn.
 *
 * Note:            - Any received packet has to be acknowledged, otherwise the Eth API will run out of descriptors.
 *                  - pBuff must be the pointer to the first buffer in the packet, if the packet spans multiple buffers.
 *                  - ackFnc is just a helper that allows the application to call an acknowledge function for each received buffer in turn.
 *****************************************************************************/
eEthRes EthRxAckBuffer(const void* pBuff, pEthPktAckF ackFnc)
{
    return _EthRxAckBuffer(pBuff, (pEthBuffAck)ackFnc, 0);
}


/****************************************************************************
 * Function:        EthTxAckBuffer
 *
 * PreCondition:    EthTxSendPacket should have been called.
 *
 * Input:           pBuff  - buffer/packet to be acknowledged or NULL.
 *                  ackFnc - function to be called for the acknowledged buffers	or NULL
 *
 * Output:          ETH_RES_OK - success
 *                  ETH_RES_PACKET_QUEUED - there are packets scheduled to be transmitted
 *                  ETH_RES_NO_PACKET - no packets scheduled for transmit
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a transmitted buffer/packet.
 *                  The transmission of this packet has to have been completed otherwise the call will fail.
 *                  When pBuff==NULL, all currently transmitted packets will be acknowledged.
 *                  The ackFnc, if !NULL, will be called for each buffer within the packet in turn.
 *
 * Note:            - Any transmitted packet has to be acknowledged, otherwise the Eth API will run out of transmission descriptors.
 *                  - pBuff must be the pointer to the first buffer in the packet, if the packet spans multiple buffers.
 *                  - ackFnc is just a helper that allows the application to acknowledge the transmitted buffers without the need
 *                  to maintain a list of buffers scheduled for transmission (this list is maintained by the Eth library anyway).
 *                  Useful especially when the transmission result is not really needed and the function is called with pPkt=0.
 *****************************************************************************/
eEthRes EthTxAckBuffer(const void* pBuff, pEthPktAckF ackFnc)
{
	return _EthAckPacket(pBuff, &_EthTxBusyList, &_EthTxFreeList, (pEthBuffAck)ackFnc, 0);
}


#endif	// _ETH
