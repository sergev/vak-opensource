#include <peripheral/eth.h>

#ifdef _ETH	// the Ethernet device is present

#include <sys/kmem.h>

#include <peripheral/int.h>

#include "_eth_dcpt_lists.h"


// local definitions
//


// local prototypes
//

sEthDNode*	_EthFindPacket(const void* pBuff, sEthDcptList* pList);

eEthRes		_EthAckPacket(const void* pBuff, sEthDcptList* pRemList, sEthDcptList* pAddList, pEthBuffAck ackFnc, void* fParam);

void		_EthAppendBusyList(sEthDcptList* pBusyList, sEthDcptList* pNewList, int rxAck);

void        _EthPoolFreeDcptList(sEthDcptList* pList, pEthDcptFree fFree, void* fParam);

void		_EthFreeDcptList(sEthDcptList* pList, pEthDcptFreeF fFree);

// local data
//
extern sEthDcptList _EthTxFreeList;		// transmit list of free descriptors
extern sEthDcptList _EthTxBusyList;		// transmit list of descriptors passed to the Tx Engine
						// the TxBusyList always ends with an sw owned descriptor (hdr.EOWN=0);

extern sEthDcptList _EthRxFreeList;		// receive list of free descriptors
extern sEthDcptList _EthRxBusyList;		// receive list of descriptors passed to the Rx Engine




// local function implementation
//

extern __inline__ void __attribute__((always_inline)) _EthMacReset(void)
{
	EMACxCFG1=_EMACxCFG1_SOFTRESET_MASK;	// reset
	EMACxCFG1=0;				// clear reset
}

extern __inline__ void __attribute__((always_inline)) _EthMacInit(void)
{
	_EthMacReset();
	EMACxMAXF=0x600;			// allow VLAN tagged frames
}



#endif	// _ETH
