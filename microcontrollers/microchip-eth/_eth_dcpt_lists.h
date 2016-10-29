#ifndef __ETH_DCPT_LISTS_H
#define __ETH_DCPT_LISTS_H

#include <sys/kmem.h>

// descriptors

typedef union
{
	struct
	{
		unsigned	    : 7;
		unsigned EOWN	: 1;
		unsigned NPV	: 1;
		unsigned sticky	: 1;	// a receive buffer is sticky
		unsigned kv0	: 1;	// belongs to k0. else k1
		unsigned rx_wack: 1;	// rx buffer waiting for acknowledge
		unsigned rx_nack: 1;	// rx descriptor is not acknowledged
		unsigned	    : 3;
		unsigned bCount	: 11;
		unsigned	    : 3;
		unsigned EOP	: 1;
		unsigned SOP	: 1;
	};
	unsigned int	w;
}sDcptHdr;	// descriptor header

#define	_SDCPT_HDR_EOWN_MASK_		0x00000080
#define	_SDCPT_HDR_NPV_MASK_		0x00000100
#define _SDCPT_HDR_STICKY_MASK_		0x00000200
#define _SDCPT_HDR_KV0_MASK_		0x00000400
#define _SDCPT_HDR_RX_WACK_MASK_	0x00000800
#define	_SDCPT_HDR_BCOUNT_MASK_		0x07ff0000
#define	_SDCPT_HDR_BCOUNT_POS_		16
#define	_SDCPT_HDR_EOP_MASK_		0x40000000
#define	_SDCPT_HDR_SOP_MASK_		0x80000000

typedef struct
{
	volatile sDcptHdr		hdr;		// header
	unsigned char*			pEDBuff;	// data buffer address
	volatile unsigned long long	stat;		// tx/rx packet status
	unsigned int			next_ed;	// next descriptor (hdr.NPV==1);
}__attribute__ ((__packed__)) sEthHwDcpt;	// hardware Tx/Rx descriptor (linked).


typedef struct
{
	volatile sDcptHdr		hdr;		// header
	unsigned char*			pEDBuff;	// data buffer address
	volatile sEthTxPktStat	stat;		// transmit packet status
	unsigned int			next_ed;	// next descriptor (hdr.NPV==1);
}__attribute__ ((__packed__)) sEthHwTxDcpt;	// hardware Tx descriptor (linked).


typedef struct
{
	volatile sDcptHdr		hdr;		// header
	unsigned char*			pEDBuff;	// data buffer address
	volatile sEthRxPktStat	stat;		// transmit packet status
	unsigned int			next_ed;	// next descriptor (hdr.NPV==1);
}__attribute__ ((__packed__)) sEthHwRxDcpt;	// hardware Rx descriptor (linked).



typedef struct _tag_sEthDNode
{
	struct _tag_sEthDNode*		next;		// next pointer in virtual space
	sEthHwDcpt			hwDcpt;		// the associated hw descriptor
}sEthDNode;	// ethernet descriptor node: generic linked descriptor for both tx/rx.


typedef struct
{
	struct _tag_sEthDNode*		next;		// next pointer in virtual space
	sEthHwTxDcpt			hwDcpt;		// the associated hw descriptor
}sEthTxDNode;	// Tx linked descriptor


typedef struct
{
	struct _tag_sEthDNode*		next;		// next pointer in virtual space
	sEthHwRxDcpt			hwDcpt;		// the associated hw descriptor
}sEthRxDNode;	// Rx linked descriptor

// although there could be un-linked Tx and Rx descriptors (hdr.NPV==0), we don't use them in this implementation


typedef struct
{
	sEthDNode*	head;	// list head
	sEthDNode*	tail;
}sEthDcptList;	// single linked list




/////  single linked lists manipulation ///////////
//
#define		SlIsEmpty(pL)		((pL)->head==0)

extern __inline__ int __attribute__((always_inline)) SlCount(sEthDcptList* pL)
{
	sEthDNode*	pN;
	int		nNodes=0;
	for(pN=pL->head; pN!=0; pN=pN->next)
	{
		nNodes++;
	}
	return nNodes;
}

extern __inline__ void __attribute__((always_inline)) SlAddHead(sEthDcptList* pL, sEthDNode* pN)
{
	pN->next=pL->head;
	pN->hwDcpt.next_ed=KVA_TO_PA(&pL->head->hwDcpt);
	pL->head=pN;
	if(pL->tail==0)
	{  // empty list
		pL->tail=pN;
	}
}

extern __inline__ void __attribute__((always_inline)) SlAddTail(sEthDcptList* pL, sEthDNode* pN)
{
	pN->next=0;
	if(pL->tail==0)
	{
		pL->head=pL->tail=pN;
	}
	else
	{
		pL->tail->next=pN;
		pL->tail->hwDcpt.next_ed=KVA_TO_PA(&pN->hwDcpt);
		pL->tail=pN;
	}
}


// insertion in the middle, not head or tail
#define		SlAddMid(pL, pN, after)	do{	(pN)->next=(after)->next; (pN)->next_ed=(after)->next_ed; \
       						(after)->next=(pN); (after)->next_ed=KVA_TO_PA(&pN->hwDcpt); \
       					}while(0)


extern __inline__ sEthDNode* __attribute__((always_inline)) SlRemoveHead(sEthDcptList* pL)
{
	sEthDNode* pN=pL->head;
	if(pL->head==pL->tail)
	{
		pL->head=pL->tail=0;
	}
	else
	{
		pL->head=pN->next;
	}
	return pN;
}



extern __inline__ void __attribute__((always_inline)) SlAppendTail(sEthDcptList* pL, sEthDcptList* pAList)
{
	sEthDNode* pN;
	while((pN=SlRemoveHead(pAList)))
	{
		SlAddTail(pL, pN);
	}
}




#endif //  __ETH_DCPT_LISTS_H
