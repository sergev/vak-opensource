#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present

/****************************************************************************
 * Function:        _EthAppendBusyList
 *
 * PreCondition:    None
 *
 * Input:           pBusyList  - busy list under hw control to add nodes to
 *                  pNewList   - list with descriptors to be added to the busy one
 *                  rxAck      - acknowledge the hw, it's an rx list
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function appends a list to the list of busy descriptors owned by the hw.
 *                  The new list to be added is supposed to have all the descriptors properly set.
 *
 * Note:            The late acknowledge for the head of the append list is to avoid a deadlock:
 *                  if hw is stuck on the end of list descriptors, decrementing the bufcnt will force re-fetch of the same
 *                  end descriptor and won't do much good.
 *                  We have to specifically write the BUFCDEC when there's a new descriptor seen by the hw that has EOWN=1.
 *****************************************************************************/
void _EthAppendBusyList(sEthDcptList* pBusyList, sEthDcptList* pNewList, int rxAck)
{
	sEthDNode	*head, *tail, *pN;


	tail=pBusyList->tail;
	head=SlRemoveHead(pNewList);
	head->hwDcpt.hdr.EOWN=0;	// not hw owned yet!

	// add all the required new descriptors/buffers
	while((pN=SlRemoveHead(pNewList)))
	{
		SlAddTail(pBusyList, pN);
		if(rxAck && !pN->hwDcpt.hdr.rx_nack)
		{
			ETHCON1SET=_ETHCON1_BUFCDEC_MASK;	// one more descriptor added
		}
	}


	head->next=tail->next;
	head->hwDcpt.next_ed=tail->hwDcpt.next_ed;	// proper connection needed
	*tail=*head;			// replace the prev tail

	head->hwDcpt.hdr.w=0;	// make it invalid descriptor for searches!
	SlAddTail(pBusyList, head);	// end properly, with EOWN=0;
	tail->hwDcpt.hdr.EOWN=1;	// ready to go!
	if(rxAck && !tail->hwDcpt.hdr.rx_nack)
	{
		ETHCON1SET=_ETHCON1_BUFCDEC_MASK;	// acknowledge the head of the list
	}

}



#endif	// _ETH
