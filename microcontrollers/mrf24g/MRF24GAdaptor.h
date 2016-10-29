/*
 * MRF24GAdaptor.h This implements a Nework adaptor
 * for the MRF24WG0MA
 *
 * Revision History:
 *      5/31/2013(KeithV): Created
 */
#ifndef MRF24GADAPTOR_H
#define	MRF24GADAPTOR_H

#include "../DEIPck/utility/deIP.h"
#include "utility/wf_universal_driver.h"

// there are a lot of MRF24 Universal driver errors, here are some of the error masks
// for each of the various sections
#define UDMask      0x00001000
#define InitMask    0x00002000
#define CFMask      0x00003000
#define CLMask      0x00004000
#define DDMask      0x00005000
#define SCMask      0x00006000
#define EAPMask     0x00007000
#define WPSMask     0x00008000
#define P2PMask     0x00009000
#define WFDMask     0x0000A000

// 00000001 -> 0000FFFF; Adaptor status; specific to adaptor

// 10000001 -> 1000FFFF; Adaptor errors; specific to adaptor

#define WFNbrAllocFails 10

#define MRF24G_NWA_VERSION          0x01000101
#define MRF24G_NWA_MTU_RX_FRAME     1536
#define MRF24G_NWA_MTU_RX           1500
#define MRF24G_NWA_MIN_TX_MTU       TCP_EMTU_S

typedef struct WFMRF_T
{
    bool (* IsInitialized)(IPSTATUS * pStatus);
    bool (* Connect)(const uint8_t * szSSID, const void * pSecurityPkt, bool fPICKeyCalc, IPSTATUS * pStatus);
    void (* Disconnect)(void);
    bool (* StartScan)(t_scanMode filter, IPSTATUS * pStatus);
    bool (* IsScanDone)(int32_t * pcAP);
    bool (* GetScanResult)(int32_t index, t_scanResult *pScanResult);

    void (* ConnectionStateGet)(uint8_t *p_state);

    void (* RegionalDomainSet)(uint8_t regionalDomain);
    void (* SsidSet)(uint8_t *p_ssid,  uint8_t ssidLength);
    void (* NetworkTypeSet)(uint8_t networkType);
    void (* ChannelListSet)(uint8_t *p_channelList, uint8_t numChannels);
    void (* ReconnectModeSet)(uint8_t retryCount, uint8_t deauthAction, uint8_t beaconTimeout, uint8_t beaconTimeoutAction);

    // WiFi security functions
    //------------------------
    void (* SecurityOpenSet)(void);
    void (* SecurityWepSet)(t_wepContext *p_context);
    void (* SecurityWpaSet)(t_wpaContext *p_context);

    // Power-save mode (PS-Poll)
    //--------------------------
    void (* PsPollEnable)(t_psPollContext *p_context);
    void (* PsPollDisable)(void);
    void (* Hibernate)(void);
    void (* PowerStateGet)(uint8_t *p_powerState);


    // Maximum Tx power functions
    //---------------------------
    void (* TxPowerFactoryMaxGet)(uint8_t *p_maxTPower);
    void (* TxPowerMaxSet)(uint8_t maxTxPower);

    // WiFi status functions
    //----------------------
    void (* DeviceInfoGet)(t_deviceInfo *p_deviceInfo);
    void (* MacStatsGet)(t_macStats *p_stats);

    // data tx functions
    //------------------
    bool (* TxPacketAllocate)(uint16_t bytesNeeded);
    void (* TxPacketCopy)(uint8_t *p_buf, uint16_t length);
    void (* TxPacketTransmit)(uint16_t packetSize);

    // data rx functions
    //------------------
    void (* ProcessRxPacket)(void);
    uint16_t (* RxPacketLengthGet)(void);
    void (* RxPacketCopy)(uint8_t *p_buf, uint16_t len);
    void (* RxPacketDeallocate)(void);

    // Miscellaneous functions
    //------------------------
    void (* MacAddressSet)(uint8_t *p_macAddress);
    void (* MacAddressGet)(uint8_t *p_macAddress);

    // Advanced functions.  These are not typically needed because the MRF24WG
    // defaults suffice for most applications.
    //------------------------------------------------------------------------
    void (* ScanContextSet)(t_scanContext *p_context);
    void (* AdhocContextSet)(t_adHocNetworkContext *p_context);
    void (* TxModeSet)(uint8_t mode);
    void (* BssidSet)(uint8_t *p_bssid);
    void (* RssiSet)(uint8_t rssi);
    void (* RtsThresholdSet)(uint16_t rstThreshold);
    void (* LinkDownThresholdSet)(uint8_t threshold);

    // multicast filter functions
    void (* SetHwMultiCastFilter)(uint8_t multicastFilterId, uint8_t multicastAddress[WF_MAC_ADDRESS_LENGTH]);

    void (* SecurityWpsSet)(t_wpsContext *p_context);
    void (* WpsCredentialsGet)(t_wpsCredentials *p_cred);

    void (* WpaConvPassphraseToKey)(t_wpaKeyInfo *p_keyInfo);
    void (* WpsKeyGenerate)(void);

} WFMRF;

typedef struct WFMRFP_T
{
    FFPT                    ffptRead;
    FFPT                    ffptWrite;
    IPSTACK *               pIpStackBeingTx;
    IPSTATUS    volatile    initStatus;
    IPSTATUS    volatile    connectionStatus;
    int32_t     volatile    cScanResults;
    bool        volatile    fMRFBusy;      // scan, key gen, connect
    uint8_t     volatile    lastEventType;
    uint32_t    volatile    lastEventData;
} WFMRFP;

typedef struct WFMRFD_T
{
    NWADP   adpMRF24G;
    NWWF    wfMRF24G;
    WFMRF   funcMRF24G;
    WFMRFP  priv;
} WFMRFD;

void _WFInterrupt(void); // Interrupt handler
const NWADP * GetMRF24GAdaptor(MACADDR *pUseThisMac, HRRHEAP hAdpHeap, IPSTATUS * pStatus);

#endif	/* MRF24GADAPTOR_H */
