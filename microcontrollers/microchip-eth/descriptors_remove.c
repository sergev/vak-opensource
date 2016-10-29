#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


/*******************************************************************************
  Function:
    int EthDescriptorsPoolRemove ( int nDescriptors, eEthDcptType dType,
                               pEthDcptFree fFree, void* fParam )

  Summary:
    Tries to remove the specified number of descriptors from the pool.

  Description:
    This function tries to remove the specified number of descriptors from the
    pool of transmit/receive descriptors.  These descriptors that are NOT used
    at the time of call can be freed (i.e. the TX descriptors not queued for a
    transmission or the not busy RX descriptors).

  Precondition:
    EthInit should have been called.

  Parameters:
    nDescriptors    - Number of descriptors to be removed
    dType           - TX/RX descriptors requested
    fFree           - Function to release the the memory
    fParam          - parameter to be used when fFree is called


  Returns:
    The number of descriptors removed.

  Example:
    <code>
    if ( EthDescriptorsPoolRemove(txDcptNo, ETH_DCPT_TYPE_TX, ffree, 0) != txDcptNo)
    {
        // Handle de-allocation error.
    }
    </code>

  Remarks:
    If fFree is NULL then the descriptors are removed from the specified pool but no de-allocation
    function is called.
    The allocation/deallocation memory functions should be consistent.

    The complete clean-up is done by EthDescriptorsPoolCleanUp.

    ETH_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has
    to be used.
 *****************************************************************************/
int EthDescriptorsPoolRemove ( int nDescriptors, eEthDcptType dType, pEthDcptFree fFree, void* fParam )
{
	sEthDcptList*	pList;
	sEthDNode*	pN;
	int		removed=0;

	if(dType == ETH_DCPT_TYPE_TX)
	{
		pList=&_EthTxFreeList;
	}
	else if(dType == ETH_DCPT_TYPE_RX)
	{
		pList=&_EthRxFreeList;
	}
	else
	{
		return 0;
	}

	while(nDescriptors--)
	{
		pN=SlRemoveHead(pList);
		if(!pN)
		{
			break;
		}
        if(fFree)
        {
            (*fFree)(pN, fParam);
        }
		removed++;
	}

	return removed;
}



/*******************************************************************************
  Function:
    void EthDescriptorsPoolCleanUp ( eEthDcptType dType, pEthDcptFree fFree, void* fParam )

  Summary:
    Removes all the specified descriptors from the pool.

  Description:
    This function performs descriptors clean-up and removes all the specified
    descriptors from the pool of transmit/receive descriptors.

  Precondition:
    EthClose should have been called.

  Parameters:
    dType        - TX/RX/All descriptors requested
    fFree        - Function to release the the memory
    fParam       - parameter to be used when fFree is called

  Returns:
    None.

  Example:
    <code>
    EthDescriptorsPoolCleanUp(ETH_DCPT_TYPE_ALL, fFree, &myInstanceData);
    </code>

  Remarks:
    If fFree is NULL then the descriptors are removed from the pools but no de-allocation
    function is called.
    The allocation/deallocation memory functions should be consistent.

    It does not free the memory allocated for the application supplied "sticky"
    buffers.  It is the calling application responsibility.

    EthClose should have been called or no TX/RX activity should be enabled at
    the time this function is called.

    If ETH_DCPT_TYPE_ALL is supplied the function will perform the clean-up of
    both types of descriptors.
 *****************************************************************************/
void EthDescriptorsPoolCleanUp( eEthDcptType dType, pEthDcptFree fFree, void* fParam )
{
	// free all allocated descriptors

	if(dType&ETH_DCPT_TYPE_TX)
	{
		_EthPoolFreeDcptList(&_EthTxFreeList, fFree, fParam);
		_EthPoolFreeDcptList(&_EthTxBusyList, fFree, fParam);
	}

	if(dType&ETH_DCPT_TYPE_RX)
	{
		_EthPoolFreeDcptList(&_EthRxFreeList, fFree, fParam);
		_EthPoolFreeDcptList(&_EthRxBusyList, fFree, fParam);
	}


}



/*******************************************************************************
  Function:
    void* EthDescriptorGetBuffer ( void *pDcpt )

  Summary:
    Returns the associated descriptor buffer.

  Description:
    This function returns the buffer currently associated with the specified descriptor.

  Precondition:

  Parameters:
    pDcpt       - Pointer to a Ethernet Descriptor


  Returns:
    The address of the buffer associated with this descriptor or NULL if no such descriptor/buffer
    exists.

  Example:
    <code>

    EthDescriptorsPoolRemove ( 1, ETH_DCPT_TYPE_RX, myRxFreeFnc, 0 );

    void myRxFreeFnc( void* ptr, void* param)
    {
        void* pBuff = EthDescriptorGetBuffer(ptr);
        if(pBuff)
        {
            // De-allocate the associated buffer
        }
    }

    </code>

  Remarks:
    The function is a helper that returns the buffer associated with a descriptor.
    It can be used to perform the queued RX/TX buffers clean-up since this list is
    maintained by the Ethernet library anyway.

    The pointer for a descriptor is returned by one of the
    EthDescriptorsPoolRemove() or EthDescriptorsPoolCleanUp() calls as the
    first parameter of the fFree function.

    No protection against interrupts, multithreading, etc.

 *****************************************************************************/
void* EthDescriptorGetBuffer ( void *pDcpt )
{
    void*       pBuff=0;
    sEthDNode*  pEDcpt=(sEthDNode*) pDcpt;

    if(pEDcpt->hwDcpt.pEDBuff != 0)
    {
        pBuff=(pEDcpt->hwDcpt.hdr.kv0?PA_TO_KVA0((int)pEDcpt->hwDcpt.pEDBuff):PA_TO_KVA1((int)pEDcpt->hwDcpt.pEDBuff));
    }

    return pBuff;


}




/****************************************************************************
 * Function:        _EthPoolFreeDcptList
 *
 * PreCondition:    None
 *
 * Input:           pList - list to be cleaned-up
 *                  fFree - function to return the allocated memory
 *                  fParam - function call parameter
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Helper to clean-up a list.
 *
 * Note:            None
 *****************************************************************************/
void _EthPoolFreeDcptList(sEthDcptList* pList, pEthDcptFree fFree, void* fParam)
{
	sEthDNode*	pN;

	while((pN=SlRemoveHead(pList)))
	{
        if(fFree)
        {
            (*fFree)(pN, fParam);
        }
	}

}



/********************
 * legacy functions
 ***********************/

/****************************************************************************
 * Function:        EthDescriptorsRemove
 *
 * PreCondition:    EthMACOpen should have been called.
 *
 * Input:           nDescriptors - number of descriptors to be removed
 *                  dType        - tx/rx descriptors requested
 *                  fFree        - function to release the the memory or NULL
 *
 * Output:          the number of descriptors removed
 *
 * Side Effects:    None
 *
 * Overview:        This function tries to remove the specified number of descriptors from the pool of transmit/receive descriptors.
 *                  These descriptors that are NOT used at the time of call can be freed (i.e. the Tx descriptors not queued for a transmission
 *                  or the not busy Rx descriptors).
 *
 * Note:            - If fFree is 0 then the standard free() is used to release the allocated memory.
 *                  The allocation/deallocation memory functions should be consistent.
 *                  - The complete clean-up is done by EthDescriptorsCleanUp();
 *                  - ETH_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has to be used.
 *****************************************************************************/
int EthDescriptorsRemove(int nDescriptors, eEthDcptType dType, pEthDcptFreeF fFree)
{
	sEthDcptList*	pList;
	sEthDNode*	pN;
	int		removed=0;

	if(dType == ETH_DCPT_TYPE_TX)
	{
		pList=&_EthTxFreeList;
	}
	else if(dType == ETH_DCPT_TYPE_RX)
	{
		pList=&_EthRxFreeList;
	}
	else
	{
		return 0;
	}

	if(!fFree)
	{
		fFree=free;
	}

	while(nDescriptors--)
	{
		pN=SlRemoveHead(pList);
		if(!pN)
		{
			break;
		}
		(*fFree)(pN);
		removed++;
	}

	return removed;
}



/****************************************************************************
 * Function:        EthDescriptorsCleanUp
 *
 * PreCondition:    EthClose should have been called.
 *
 * Input:           dType        - tx/rx/all descriptors requested
 *                  fFree        - function to release the the memory or NULL
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function performs descriptors clean-up and removes all the specified descriptors from the pool of transmit/receive descriptors.
 *
 * Note:            - If fFree is 0 then the standard free() is used to release the allocated memory.
 *                  The allocation/deallocation memory functions should be consistent.
 *                  - It does not free the memory allocated for the application supplied "sticky" buffers.
 *                  It is the calling app responsibility.
 *                  - EthClose should have been called or no tx/rx activity should be enabled at the time this function is called.
 *                  - If ETH_DCPT_TYPE_ALL is supplied the function will perform the clean-up of both types of descriptors.
 ******************************************************************************/
void EthDescriptorsCleanUp(eEthDcptType dType, pEthDcptFreeF fFree)
{
	// free all allocated descriptors
	if(!fFree)
	{
		fFree=free;
	}

	if(dType&ETH_DCPT_TYPE_TX)
	{
		_EthFreeDcptList(&_EthTxFreeList, fFree);
		_EthFreeDcptList(&_EthTxBusyList, fFree);
	}

	if(dType&ETH_DCPT_TYPE_RX)
	{
		_EthFreeDcptList(&_EthRxFreeList, fFree);
		_EthFreeDcptList(&_EthRxBusyList, fFree);
	}


}


/****************************************************************************
 * Function:        _EthFreeDcptList
 *
 * PreCondition:    None
 *
 * Input:           pList - list to be cleaned-up
 *                  fFree - function to return the allocated memory
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Helper to clean-up a list.
 *                  The standard free() is used to release the allocated memory.
 *
 * Note:            None
 *****************************************************************************/
void _EthFreeDcptList(sEthDcptList* pList, pEthDcptFreeF fFree)
{
	sEthDNode*	pN;

	while((pN=SlRemoveHead(pList)))
	{
		(*fFree)(pN);
	}

}


#endif	// _ETH
