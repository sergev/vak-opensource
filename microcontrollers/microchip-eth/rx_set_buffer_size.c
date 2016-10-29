#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present

/****************************************************************************
 * Function:        EthRxSetBufferSize
 *
 * PreCondition:    0 < rxBuffSize <= 2032
 *
 * Input:           rxBuffSize - size of the rx buffers
 *
 * Output:          ETH_RES_OK for success,
 *                  an error code otherwise
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the required buffer size for the receive operation.
 *                  In this implementation, all receive descriptors use the same buffer size (unlike the transmission flow
 *                  where each descriptor can have a different buffer size).
 *
 * Note:            - This function should be part of the initialization process and shoult NOT be called when the rx process is active!
 *                  - The receive buffer size is always TRUNCATED to a multiple of 16 bytes.
 *****************************************************************************/
eEthRes EthRxSetBufferSize(int rxBuffSize)
{
	rxBuffSize/=16;		// truncate
	if(!rxBuffSize)
	{
		return ETH_RES_RX_SIZE_ERR;
	}

	ETHCON2=rxBuffSize<<_ETHCON2_RXBUF_SZ_POSITION;
	return ETH_RES_OK;
}


#endif	// _ETH
