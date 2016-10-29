#include "_eth_include.h"


#ifdef _ETH	// the Ethernet device is present

/*********************************************************************
 * Function:        void EthEventsEnableSet(eEthEvents eEvents)
 *
 * PreCondition:    None
 *
 * Input:           eEvents	- events with the significance described in the eEthEvents definition.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function enables the events that will generate interrupts for the Eth controller.
 *                  Multiple events can be orr-ed together.
 *                  Any event that is set in the eEvents will be enabled , the other events won't be touched.
 *
 * Note:            None.
 *
 ********************************************************************/
void EthEventsEnableSet(eEthEvents eEvents)
{
	ETHIENSET=eEvents;
}

/*********************************************************************
 * Function:        void EthEventsEnableClr(eEthEvents eEvents)
 *
 * PreCondition:    None
 *
 * Input:           eEvents	- events with the significance described in the eEthEvents definition.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function disables the events that will generate interrupts for the Eth controller.
 *                  Multiple events can be orr-ed together.
 *                  Any event that is set in the eEvents will be disabled , the other events won't be touched.
 *
 * Note:            None.
 ********************************************************************/
void EthEventsEnableClr(eEthEvents eEvents)
{
	ETHIENCLR=eEvents;
}

/*********************************************************************
 * Function:        void EthEventsEnableWrite(eEthEvents eEvents)
 *
 * PreCondition:    None
 *
 * Input:           eEvents	- events with the significance described in the eEthEvents definition.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The function enables the events that will generate interrupts for the Eth controller.
 *                  The enabled events are forced to the eEvents value.
 *
 * Note:            None.
 ********************************************************************/
void EthEventsEnableWrite(eEthEvents eEvents)
{
	ETHIEN=eEvents;
}

/*********************************************************************
 * Function:        eEthEvents EthEventsEnableGet(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          eEvents	- events with the significance described in the eEthEvents definition.
 *
 * Side Effects:    None
 *
 * Overview:        The function returns the enabled events for the Eth controller.
 *
 * Note:            None.
 ********************************************************************/
eEthEvents EthEventsEnableGet(void)
{
	return (eEthEvents)ETHIEN;
}



#endif	// _ETH
