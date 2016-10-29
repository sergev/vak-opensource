#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


/****************************************************************************
 * Function:        EthRxGetBuffer
 *
 * PreCondition:    EthRxSetBufferSize, EthRxBuffersAppend should have been called.
 *
 * Input:           ppBuff  - address of a pointer to packet buffer data or NULL
 *                  pRxStat - address to store the pointer to the packet status or NULL
 *
 * Output:          ETH_RES_OK - for success,
 *                  ETH_RES_PACKET_QUEUED - there are packets queued for receive but not completed yet
 *                  ETH_RES_NO_PACKET - no packets available in the receiving queue
 *                  ETH_RES_RX_PKT_SPLIT_ERR - packet spans accross multiple buffers
 *
 * Side Effects:    None
 *
 * Overview:        This function returns a pointer to the data and the status of the next available received packet.
 *                  If there's no available fully received packet the call will fail (ETH_RES_PACKET_QUEUED/ETH_RES_NO_PACKET).
 *                  If the received packet spans multiple buffers/descriptors (i.e. when the size of the buffer,
 *                  EthRxSetBufferSize() is less than the packet size) ETH_RES_RX_PKT_SPLIT_ERR will be returned.
 *                  However, if the ppBuff is NULL, than the caller can use the returned pRxStat to check the
 *                  total size of the packet. Then using the size of the set buffers, the number of buffers used in
 *                  the packet will result.
 *
 * Note:            - The returned status pointer is invalid after the EthRxAckBuffer() is called.
 *                    Also the associated packet buffer if it was appended with a ETH_BUFF_FLAG_RX_STICKY flag.
 *****************************************************************************/
eEthRes EthRxGetBuffer(void** ppBuff, const sEthRxPktStat** pRxStat)
{

	eEthRes		    res;
	sEthPktDcpt     pktDcpt;
	int		        nBuffs;		// buffers per packet

	pktDcpt.next=0;		// create a single buffer packet descriptor;

	res=EthRxGetPacket(&pktDcpt, &nBuffs, pRxStat);

	*ppBuff=pktDcpt.pBuff;

	return res;
}




/****************************************************************************
 * Function:        EthRxGetPacket
 *
 * PreCondition:    EthRxSetBufferSize, EthRxBuffersAppend should have been called.
 *
 * Input:           pPkt    - pointer to a packet descriptor: a list of buffer descriptors large enough to receive the packet layout or NULL
 *                  pnBuffs - pointer to store the number of buffers in the packet or NULL.
 *                  pRxStat - address to store the pointer to the packet status or NULL
 *
 * Output:          ETH_RES_OK - for success,
 *                  ETH_RES_PACKET_QUEUED - there are packets queued for receive but not completed yet
 *                  ETH_RES_NO_PACKET - no packets available in the receiving queue
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the layout and the status of the next available received packet.
 *                  If there's no available fully received packet the call will fail (ETH_RES_PACKET_QUEUED/ETH_RES_NO_PACKET).
 *                  The supplied pPkt has to be large enough to accomodate the packet layout in the case
 *                  of packets split across multiple buffers/descriptors (i.e. when the size of the buffer,
 *                  EthRxSetBufferSize() is less than the packet size) otherwise ETH_RES_RX_PKT_SPLIT_ERR will be returned.
 *                  However, if the pPkt is NULL, than the caller can use the returned pRxStat to check the
 *                  total size of the packet. Then using the size of the set buffers, the number of buffers in
 *                  pPkt will result.
 *                  pnBuffs can be used for the same purpose. If pnBuffs is supplied, the numbers of buffers
 *                  in the packet will be returned.
 *
 * Note:            - The packet list, when supplied, is terminated with a NULL buffer
 *                  - The returned status pointer is invalid after the EthRxAckPacket() is called.
 *****************************************************************************/
eEthRes EthRxGetPacket(sEthPktDcpt* pPkt, int* pnBuffs, const sEthRxPktStat** pRxStat)
{
	sEthDNode	*pEDcpt, *pHead;
	eEthRes		res;

	res=ETH_RES_NO_PACKET;

	if(pPkt)
	{
		pPkt->pBuff=0;
		pPkt->nBytes=0;
	}

	for(pEDcpt=_EthRxBusyList.head; pEDcpt!=0; pEDcpt=pEDcpt->next)
	{
		if(pEDcpt->hwDcpt.hdr.EOWN)
		{
			res=ETH_RES_PACKET_QUEUED;
			break;		// not done
		}
		else if(pEDcpt->hwDcpt.hdr.SOP && pEDcpt->hwDcpt.hdr.rx_wack==0)
		{ // found the beg of a packet
			sEthPktDcpt*	pBuffDcpt;
			int		nBuffs, reportBuffs;

			pHead=pEDcpt;
			nBuffs=reportBuffs=0;
			pBuffDcpt=pPkt;
			res=ETH_RES_OK;

			if(pRxStat)
			{
				*pRxStat=(const sEthRxPktStat*)&((sEthRxDNode*)pEDcpt)->hwDcpt.stat;
			}

			while(pBuffDcpt || pnBuffs)
			{	// either way, we have to parse the packet
				if(pBuffDcpt)
				{
					pBuffDcpt->pBuff=(pEDcpt->hwDcpt.hdr.kv0?PA_TO_KVA0((int)pEDcpt->hwDcpt.pEDBuff):PA_TO_KVA1((int)pEDcpt->hwDcpt.pEDBuff));
					pBuffDcpt->nBytes=pEDcpt->hwDcpt.hdr.bCount;
					pBuffDcpt=pBuffDcpt->next;
					reportBuffs++;
				}
				nBuffs++;

				while(pEDcpt->hwDcpt.hdr.EOWN);		// shouldn't happen
				if(pEDcpt->hwDcpt.hdr.EOP)
				{	// end of packet
					if(pnBuffs)
					{
						*pnBuffs=nBuffs;
					}

					if(pBuffDcpt)
					{
						pBuffDcpt->pBuff=0;	// end it properly
						pBuffDcpt->nBytes=0;
					}

					if(pPkt)
					{
						if(reportBuffs!=nBuffs)
						{
							res=ETH_RES_RX_PKT_SPLIT_ERR;
						}
						else
						{
							pHead->hwDcpt.hdr.rx_wack=1;		// reported ok, waiting to be acknowledged
						}
					}
					break;
				}
				pEDcpt=pEDcpt->next;
			}

			break;
		}
	}

	return res;
}
#endif	// _ETH
