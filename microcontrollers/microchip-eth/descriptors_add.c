#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


// global data
//
sEthDcptList _EthTxFreeList= {0, 0};	// transmit list of free descriptors
sEthDcptList _EthTxBusyList= {0, 0};	// transmit list of descriptors passed to the Tx Engine
					// the _EthTxBusyList always ends with an sw owned descriptor (hdr.EOWN=0);

sEthDcptList _EthRxFreeList= {0, 0};	// receive list of free descriptors
sEthDcptList _EthRxBusyList= {0, 0};	// receive list of descriptors passed to the Rx Engine


/*******************************************************************************
  Function:
    int EthDescriptorsPoolAdd ( int nDescriptors, eEthDcptType dType,
                            pEthDcptAlloc fAlloc, void* fParam )

  Summary:
    Adds transmit & receive descriptors to the pool.

  Description:
    This function adds the specified number of transmit/receive descriptors to
    the pool of descriptors.  These descriptors are used whenever a new
    transmission or a reception is initiated.  The needed memory per descriptor
    is returned by EthDescriptorsGetSize.   Descriptors can be freed with
    EthDescriptorsPoolRemove.

  Precondition:
    EthInit should have been called.

  Parameters:
    nDescriptors    - Number of descriptors to be added

    dType           - TX/RX descriptors requested

    fAlloc          - Function to be called to allocate the needed memory

    fParam          - parameter to be used when fAlloc is called

  Returns:
    The number of descriptors successfully created.

  Example:
    <code>
    if( EthDescriptorsPoolAdd(txDcptNo, ETH_DCPT_TYPE_TX, fMemAlloc, &myInstanceData) != txDcptNo )
    {
        // Handle allocation error
    }
    </code>

  Remarks:
    The number of created descriptors is different from the number of
    requested ones only if the memory allocation failed.

    The first time the specific pool is initialized an extra descriptor is added to the pool
    so that the actual memory requested will be (nDescriptors+1)*EthDescriptorsGetSize().

    fAlloc should not be NULL.

    The complete clean-up is done by EthDescriptorsPoolCleanUp();

    ETH_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has
    to be used.
 *****************************************************************************/

int EthDescriptorsPoolAdd ( int nDescriptors, eEthDcptType dType, pEthDcptAlloc fAlloc, void* fParam )
{
	sEthDNode*	pDcpt;
	sEthDcptList	*pFList, *pBList;	// free and busy lists
	int		nCreated;

	if(fAlloc==0)
	{
		return 0;
	}
    else if(dType == ETH_DCPT_TYPE_TX)
	{
		pFList=&_EthTxFreeList;
		pBList=&_EthTxBusyList;
	}
	else if(dType == ETH_DCPT_TYPE_RX)
	{
		pFList=&_EthRxFreeList;
		pBList=&_EthRxBusyList;
	}
	else
	{
		return 0;
	}


	if(SlIsEmpty(pBList))
	{	// first time we're creating descriptors for this list...
		pDcpt=(sEthDNode*)(*fAlloc)(1, sizeof(*pDcpt), fParam);
		if(!pDcpt)
		{
			return 0;
		}
		SlAddHead(pBList, pDcpt);	// the busy list must always have a dummy sw owned tail descriptor
	}

	// create the descriptors
	nCreated=0;
	while(nDescriptors--)
	{
		pDcpt=(sEthDNode*)(*fAlloc)(1, sizeof(*pDcpt), fParam);
		if(!pDcpt)
		{
			break;
		}
		SlAddTail(pFList, pDcpt);
		nCreated++;
	}

	return nCreated;
}




/********************
 * legacy functions
 ***********************/

/****************************************************************************
 * Function:        EthDescriptorsAdd
 *
 * PreCondition:    EthMACOpen should have been called.
 *
 * Input:           nDescriptors - number of descriptors to be added
 *                  dType        - tx/rx descriptors requested
 *                  fAlloc       - function to be called to allocate the needed memory or NULL.
 *
 * Output:          the number of descriptors succesfully created
 *
 * Side Effects:    None
 *
 * Overview:        This function adds the specified number of transmit/receive descriptors to the pool of descriptors.
 *                  These descriptors are used whenever a new transmission or a reception is initiated.
 *                  The needed memory per descriptor is returned by EthDescriptorsGetSize();
 *                  Descriptors can be freed with EthDescriptorsRemove().
 *
 * Note:            - The number of created descriptors is different from the number of requested ones only if the memory allocation failed.
 *                  If fAlloc is 0 then the standard calloc() is used to get the needed memory.
 *                  - The complete clean-up is done by EthDescriptorsCleanUp();
 *                  - ETH_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has to be used.
 *****************************************************************************/
int EthDescriptorsAdd(int nDescriptors, eEthDcptType dType, pEthDcptAllocF fAlloc)
{
	sEthDNode*	pDcpt;
	sEthDcptList	*pFList, *pBList;	// free and busy lists
	int		nCreated;

	if(dType == ETH_DCPT_TYPE_TX)
	{
		pFList=&_EthTxFreeList;
		pBList=&_EthTxBusyList;
	}
	else if(dType == ETH_DCPT_TYPE_RX)
	{
		pFList=&_EthRxFreeList;
		pBList=&_EthRxBusyList;
	}
	else
	{
		return 0;
	}

	if(fAlloc==0)
	{
		fAlloc=calloc;
	}

	if(SlIsEmpty(pBList))
	{	// first time we're creating descriptors for this list...
		pDcpt=(sEthDNode*)(*fAlloc)(1, sizeof(*pDcpt));
		if(!pDcpt)
		{
			return 0;
		}
		SlAddHead(pBList, pDcpt);	// the busy list must always have a dummy sw owned tail descriptor
	}

	// create the descriptors
	nCreated=0;
	while(nDescriptors--)
	{
		pDcpt=(sEthDNode*)(*fAlloc)(1, sizeof(*pDcpt));
		if(!pDcpt)
		{
			break;
		}
		SlAddTail(pFList, pDcpt);
		nCreated++;
	}

	return nCreated;
}



#endif	// _ETH
