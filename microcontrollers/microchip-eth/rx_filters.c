#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present


/****************************************************************************
 * Function:        EthRxFiltersSet
 *
 * PreCondition:    This function should not be called when Eth rx operation is enabled.
 *
 * Input:           rxFilters - rx filters that are to be set
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the acceptance/rejection filters for the Eth receive mechanism.
 *                  Multiple filters can be OR-red together.
 *                  All filter specified in rxFilters will be set.
 *
 * Note:            All filters except Pattern Match can be set. Use EthRxFiltersPMSet();
 *                  Before enabling the Hash Table filter, the hash table values should be set using EthRxFiltersHTSet().
 *****************************************************************************/
void EthRxFiltersSet(eEthRxFilters rxFilters)
{
	ETHRXFCSET=rxFilters;
}




/****************************************************************************
 * Function:        EthRxFiltersClr
 *
 * PreCondition:    This function should not be called when Eth rx operation is enabled.
 *
 * Input:           rxFilters - rx filters that are to be cleared
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function clears the acceptance/rejection filters for the Eth receive mechanism.
 *                  Multiple filters can be OR-red together.
 *                  All filters specified in rxFilters will be cleared.
 *
 * Note:            All filters except Pattern Match can be cleared. Use EthRxFiltersPMClr();
 *****************************************************************************/
void EthRxFiltersClr(eEthRxFilters rxFilters)
{
	ETHRXFCCLR=rxFilters;
}


/****************************************************************************
 * Function:        EthRxFiltersWrite
 *
 * PreCondition:    This function should not be called when Eth rx operation is enabled.
 *
 * Input:           rxFilters - RX filters that are to be written
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function updates the acceptance/rejection filters for the Eth receive mechanism to the required value.
 *
 * Note:            All filters except Pattern Match can be updated.
 *                  The Pattern Match filter will be disabled after this call.  Use EthRxFiltersPMSet();
 *                  Before enabling the Hash Table filter, the hash table values should be set using EthRxFiltersHTSet().
 *****************************************************************************/
void EthRxFiltersWrite(eEthRxFilters rxFilters)
{
	ETHRXFC=rxFilters;
}


/****************************************************************************
 * Function:        EthRxFiltersHTSet
 *
 * PreCondition:    This function should not be called when Eth rx operation is enabled and the hash filter is active.
 *
 * Input:           htable - the hash table itself
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the hash table for the hash table rx filter.
 *
 * Note:            Properly set the hash table with this function before enabling the Hash Table filter.
 *****************************************************************************/
void EthRxFiltersHTSet(unsigned long long htable)
{
	ETHHT0=(unsigned int)htable;
	ETHHT1=(unsigned int)(htable>>32);
}


/****************************************************************************
 * Function:        EthRxFiltersPMSet
 *
 * PreCondition:    This function should not be called when Eth rx operation is enabled.
 *
 * Input:           mode          - the required Pattern Match Mode
 *                  matchMask     - mask in the 64 byte window
 *                  matchOffs     - the offset applied to the incoming data (0 to 8128)
 *                  matchChecksum - the 16 bit checksum used for comparison
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function enables the Pattern Match filter with the specified settings for the
 *                  offset and window mask. The calculated checksum of the 64 bytes window
 *                  starting at matchOffs and using the matchMask bytes in the window, is compared against
 *                  matchChecksum.
 *                  If ETH_FILT_PMATCH_INVERT is used in the mode, then the calculated checksum must not
 *                  match the passed matchChecksum in order for the Pattern Match to succeed.
 *
 * Note:            None
 *****************************************************************************/
void EthRxFiltersPMSet(eEthPMatchMode mode, unsigned long long matchMask, unsigned int matchOffs, unsigned int matchChecksum)
{
        ETHRXFCCLR = _ETHRXFC_PMMODE_MASK;	// disable pattern match mode

	ETHPMM0 = (unsigned int)matchMask;
        ETHPMM1 = (unsigned int)(matchMask>>32);

        ETHPMO = matchOffs;
        ETHPMCS = matchChecksum;

        if(mode&ETH_FILT_PMATCH_INVERT)
        {
            ETHRXFCSET = _ETHRXFC_NOTPM_MASK;
        }
        else
        {
            ETHRXFCCLR = _ETHRXFC_NOTPM_MASK;
        }

        ETHRXFCSET = (mode&(~ETH_FILT_PMATCH_INVERT))<<_ETHRXFC_PMMODE_POSITION;	// enable Pattern Match mode

}


/****************************************************************************
 * Function:        EthRxFiltersPMClr
 *
 * PreCondition:    This function should not be called when Eth rx operation is enabled.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function disables the Pattern Match receive filter.
 *
 * Note:            None
 *****************************************************************************/
void EthRxFiltersPMClr(void)
{
        ETHRXFCCLR = _ETHRXFC_PMMODE_MASK;	// disable pattern match mode
}



#endif	// _ETH
