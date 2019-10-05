/*
 * Project: MCP2221 HID Library
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2015 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/mcp2221-hid-library/
 */

#ifndef LIBMCP2221_H_
#define LIBMCP2221_H_

#include <stdint.h>
#include <wchar.h>

#define MCP2221_STR_LEN                 31  /**< Maximum length of wchar_t USB descriptor strings + 1 for null term */
#define MCP2221_GPIO_COUNT              4   /**< GPIO pin count */
#define MCP2221_DAC_MAX                 31  /**< Maximum value of DAC output */
#define MCP2221_ADC_COUNT               3   /**< ADC count */

#define MCP2221_DEFAULT_VID             0x04D8  /**< Default VID */
#define MCP2221_DEFAULT_PID             0x00DD  /**< Default PID */
#define MCP2221_DEFAULT_MANUFACTURER    L"Microchip Technology Inc."    /**< Default manufacturer descriptor */
#define MCP2221_DEFAULT_PRODUCT         L"MCP2221 USB-I2C/UART Combo"   /**< Default product descriptor */

#define MCP2221_REPORT_SIZE             64  /**< HID Report size */

/**
 * \enum mcp2221_error
 * \brief Error codes
 */
typedef enum
{
    MCP2221_SUCCESS = 0,        /**< All is well */
    MCP2221_ERROR = -1,         /**< General error */
    MCP2221_INVALID_ARG = -2,   /**< Invalid argument supplied, probably a null pointer */
    MCP2221_ERROR_HID = -3      /**< HIDAPI returned an error */
}mcp2221_error;

/**
 * \enum mcp2221_i2c_state_t
 * \brief I2C states (TODO: incomplete)
 */
typedef enum
{
    MCP2221_I2C_IDLE = 0,
    MCP2221_I2C_ADDRNOTFOUND = 37,  // WR NACK
    MCP2221_I2C_DATAREADY = 85,
    MCP2221_I2C_UNKNOWN1 = 98       // stop timeout
}mcp2221_i2c_state_t;

/**
 * \enum mcp2221_dac_ref_t
 * \brief Reference voltages for DAC
 */
typedef enum
{
    MCP2221_DAC_REF_4096 = 7,   /**< 4096mV Reference */
    MCP2221_DAC_REF_2048 = 5,   /**< 2048mV Reference */
    MCP2221_DAC_REF_1024 = 3,   /**< 1024mV Reference */
    MCP2221_DAC_REF_OFF = 1,    /**< No reference */
    MCP2221_DAC_REF_VDD = 0     /**< VDD Reference */
}mcp2221_dac_ref_t;

/**
 * \enum mcp2221_adc_ref_t
 * \brief Reference voltages for ADC
 */
typedef enum
{
    MCP2221_ADC_REF_4096 = 7,   /**< 4096mV Reference */
    MCP2221_ADC_REF_2048 = 5,   /**< 2048mV Reference */
    MCP2221_ADC_REF_1024 = 3,   /**< 1024mV Reference */
    MCP2221_ADC_REF_OFF = 1,    /**< No reference */
    MCP2221_ADC_REF_VDD = 0     /**< VDD Reference */
}mcp2221_adc_ref_t;

/**
 * \enum mcp2221_int_trig_t
 * \brief Trigger modes for interrupt
 */
typedef enum
{
    MCP2221_INT_TRIG_INVALID = 0,           /**< Invalid */
    MCP2221_INT_TRIG_RISING = 0b00001000,   /**< Trigger on rising edge */
    MCP2221_INT_TRIG_FALLING = 0b00000010,  /**< Trigger on falling edge */
    MCP2221_INT_TRIG_BOTH = 0b00001010      /**< Trigger on either rising or falling edge (MCP2221_INT_TRIG_RISING | MCP2221_INT_TRIG_FALLING) */   // TODO maybe just OR the 2 values above?
}mcp2221_int_trig_t;

/**
 * \enum mcp2221_gpio_mode_t
 * \brief GPIO modes (check GP DESIGNATION TABLE in the datasheet for what the dedicated and alternative functions do for each pin)
 * \note GP DESIGNATION TABLE has the ALT_FUNC numbered 0-2, should be 1-3 to match the rest of the datasheet
 */
typedef enum
{
    MCP2221_GPIO_MODE_GPIO = 0,         /**< Normal IO, manually set to input/output, high/low */
    MCP2221_GPIO_MODE_DEDI = 1,         /**< Dedicated function */
    MCP2221_GPIO_MODE_ALT1 = 2,         /**< Alternative function 1 */
    MCP2221_GPIO_MODE_ALT2 = 3,         /**< Alternative function 2 */
    MCP2221_GPIO_MODE_ALT3 = 4,         /**< Alternative function 3 */
    MCP2221_GPIO_MODE_INVALID = 0xff    /**< Invalid */
}mcp2221_gpio_mode_t;

#define MCP2221_GPIO_MODE_SSPND MCP2221_GPIO_MODE_DEDI  /**< TODO */
#define MCP2221_GPIO_MODE_ADC   MCP2221_GPIO_MODE_ALT1  /**< TODO */
#define MCP2221_GPIO_MODE_DAC   MCP2221_GPIO_MODE_ALT2  /**< TODO */
#define MCP2221_GPIO_MODE_IOC   MCP2221_GPIO_MODE_ALT3  /**< TODO */

/**
 * \enum mcp2221_gpio_value_t
 * \brief GPIO read values
 */
typedef enum
{
    MCP2221_GPIO_VALUE_HIGH = 1,        /**< GPIO pin reads HIGH */
    MCP2221_GPIO_VALUE_LOW = 0,         /**< GPIO pin reads LOW */
    MCP2221_GPIO_VALUE_INVALID = 0xEE   /**< GPIO is not set as an input or output */
}mcp2221_gpio_value_t;

/**
 * \enum mcp2221_gpio_direction_t
 * \brief Direction when pin is set to GPIO mode
 */
typedef enum
{
    MCP2221_GPIO_DIR_INPUT = 1,         /**< Input */
    MCP2221_GPIO_DIR_OUTPUT = 0,        /**< Output */
    MCP2221_GPIO_DIR_INVALID = 0xEF     /**< GPIO is not set as an input or output */
}mcp2221_gpio_direction_t;

/**
 * \enum mcp2221_pwrsrc_t
 * \brief Power source
 */
typedef enum
{
    MCP2221_PWRSRC_SELFPOWERED = 0x40,
    MCP2221_PWRSRC_BUSPOWERED = 0
}mcp2221_pwrsrc_t;

/**
 * \enum mcp2221_wakeup_t
 * \brief Remote wakeup (wakeup the USB host from sleep mode)
 */
typedef enum
{
    MCP2221_WAKEUP_DISABLED = 0,    /**< Disable USB host wakeup */
    MCP2221_WAKEUP_ENABLED = 0x20   /**< Enable USB host wakeup */
}mcp2221_wakeup_t;

/**
 * \enum mcp2221_clkdiv_t
 * \brief Clock reference output divider from 48MHz
 */
typedef enum
{
    MCP2221_CLKDIV_RESERVED = 0,    /**< Invalid */
    MCP2221_CLKDIV_2 = 1,           /**< 24MHz */
    MCP2221_CLKDIV_4 = 2,           /**< 12MHz */
    MCP2221_CLKDIV_8 = 3,           /**< 6MHz */
    MCP2221_CLKDIV_16 = 4,          /**< 3MHz */
    MCP2221_CLKDIV_32 = 5,          /**< 1.5MHz */
    MCP2221_CLKDIV_64 = 6,          /**< 750KHz */
    MCP2221_CLKDIV_128 = 7,         /**< 375KHz */
}mcp2221_clkdiv_t;

/**
 * \enum mcp2221_clkduty_t
 * \brief Clock reference output duty cycle
 */
typedef enum
{
    MCP2221_CLKDUTY_0 = 0x00,       /**< 0% duty cycle, disabled */
    MCP2221_CLKDUTY_25 = 0x08,      /**< 25% */
    MCP2221_CLKDUTY_50 = 0x10,      /**< 50% */
    MCP2221_CLKDUTY_75 = 0x18       /**< 75% */
}mcp2221_clkduty_t;

/**
 * \enum mcp2221_i2crw_t
 * \brief TODO
 */
typedef enum
{
    MCP2221_I2CRW_NORMAL = 0,
    MCP2221_I2CRW_REPEATED = 1,
    MCP2221_I2CRW_NOSTOP = 2
}mcp2221_i2crw_t;

/**
 * \enum mcp2221_dedipin_t
 * \brief Used to select which dedicated pin to operate on (only used for setting/getting polarity)
 */
typedef enum
{
    MCP2221_DEDIPIN_LEDUARTRX = 6,
    MCP2221_DEDIPIN_LEDUARTTX = 5,
    MCP2221_DEDIPIN_LEDI2C = 4,
    MCP2221_DEDIPIN_SSPND = 3,
    MCP2221_DEDIPIN_USBCFG = 2
}mcp2221_dedipin_t;

/**
 * \enum mcp2221_gpio_t
 * \brief GPIO pins
 */
typedef enum
{
    MCP2221_GPIO0 = 1,  /**< GPIO0 */
    MCP2221_GPIO1 = 2,  /**< GPIO1 */
    MCP2221_GPIO2 = 4,  /**< GPIO2 */
    MCP2221_GPIO3 = 8   /**< GPIO3 */
}mcp2221_gpio_t;



/**
* \struct mcp2221_usbinfo_t
* \brief Contains enumerated USB info about the device
*/
typedef struct{
    wchar_t manufacturer[MCP2221_STR_LEN];  /**< Enumerated manufacturer descriptor */
    wchar_t product[MCP2221_STR_LEN];       /**< Enumerated product descriptor */
    wchar_t serial[MCP2221_STR_LEN];        /**< Enumerated serial descriptor */
    uint8_t factorySerialLen;               /**< Factory serial length (usually 8) */
    char factorySerial[60];                 /**< Factory serial */
    char firmware[2];                       /**< Firmware version */
    char hardware[2];                       /**< Hardware version */
    int vid;                                /**< VID */
    int pid;                                /**< PID */
    mcp2221_pwrsrc_t powerSource;           /**< Power source */
    mcp2221_wakeup_t remoteWakeup;          /**< Remote USB host wakeup */
    int milliamps;                          /**< Enumerated current limit */
}mcp2221_usbinfo_t;

/**
* \struct mcp2221_t
* \brief TODO
*/
typedef struct{
    void* handle;   /**< Device handle */
    char* path;     /**< Device path, used to identify the physical device */
    uint8_t gpioCache[MCP2221_GPIO_COUNT];  /**< GPIO config cache */
    mcp2221_usbinfo_t usbInfo;
}mcp2221_t;

/**
* \struct mcp2221_i2cpins_t
* \brief Raw I2C pin values
*/
typedef struct{
    uint8_t SCL;    /**< I2C SCL Value */
    uint8_t SDA;    /**< I2C SDA Value */
}mcp2221_i2cpins_t;

/**
* \struct mcp2221_gpioconf_t
* \brief GPIO configuration
*/
typedef struct{
    int gpios;                          /**< Which pins to operate on (see ::mcp2221_gpio_t) */ // TODO: change to mcp2221_gpio_t ?
    mcp2221_gpio_mode_t mode;           /**< Mode */
    mcp2221_gpio_direction_t direction; /**< Direction (GPIO mode only) */
    mcp2221_gpio_value_t value;         /**< Value (GPIO mode only) */
}mcp2221_gpioconf_t;

// Can't return arrays as value from funcs, but can return structs
/**
* \struct mcp2221_gpioconfset_t
* \brief TODO
*/
typedef struct{
    mcp2221_gpioconf_t conf[MCP2221_GPIO_COUNT];
}mcp2221_gpioconfset_t;




#if defined(__cplusplus)
extern "C" {
#endif

/**
* @brief Initialise, must be called before anything else!
*
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_init(void);

/**
* @brief TODO
*
* @return (none)
*/
void mcp2221_exit(void);

/**
* @brief Find all HIDs matching the supplied parameters, must be called before attempting to open a device
*
* @param [vid] VID to match, 0 will match all VIDs
* @param [pid] PID to match, 0 will match all PIDs
* @param [manufacturer] Manufacturer to match, NULL will match all manufacturers
* @param [product] Product to match, NULL will match all products
* @param [serial] Serial to match, NULL will match all serials (enumerating with serial needs to be enabled for this to work - mcp2221_saveSerialEnumerate())
* @return Number of devices found
*/
int mcp2221_find(int vid, int pid, wchar_t* manufacturer, wchar_t* product, wchar_t* serial);

/**
* @brief TODO
*
* @return 1 if devices are the same, 0 if not
*/
int mcp2221_sameDevice(mcp2221_t* dev1, mcp2221_t* dev2);

/**
* @brief Open first MCP2221 device found
*
* @return ::mcp2221_error error code
*/
mcp2221_t* mcp2221_open(void);

/**
* @brief Open device with specified index number (starting from 0 up to however many devices were found)
*
* @return ::mcp2221_error error code
*/
mcp2221_t* mcp2221_open_byIndex(int idx);

/**
* @brief Open device with specified serial
*
* Serial enumeration must be enable for this to work - mcp2221_saveSerialEnumerate()
*
* @return ::mcp2221_error error code
*/
mcp2221_t* mcp2221_open_bySerial(wchar_t* serial);

/**
* @brief Close device
*
* @return (none)
*/
void mcp2221_close(mcp2221_t* device);

/**
* @brief Perform a reset of the device
*
* @param [device] Device to operate on
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_reset(mcp2221_t* device);

/**
* @brief Check to see if the device is still connected, should return ::MCP2221_SUCCESS
*
* @param [device] Device to operate on
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_isConnected(mcp2221_t* device);

/**
* @brief Send a custom report, the response is placed in the same buffer
*
* @param [device] Device to operate on
* @param [report] The report, should be an array with at least ::MCP2221_REPORT_SIZE elements
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_rawReport(mcp2221_t* device, uint8_t* report);

/**
* @brief TODO
*
* @return ::mcp2221_gpioconfset_t
*/
mcp2221_gpioconfset_t mcp2221_GPIOConfInit(void);

/**
* @brief Set the clock reference output divider and duty cycle (SRAM)
*
* @param [device] Device to operate on
* @param [div] Frequency divider from 48MHz
* @param [duty] Duty cycle
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_setClockOut(mcp2221_t* device, mcp2221_clkdiv_t div, mcp2221_clkduty_t duty);

/**
* @brief Set the DAC voltage reference and output value (SRAM)
*
* @param [device] Device to operate on
* @param [ref] Voltage reference
* @param [value] Output value (0 - 31)
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_setDAC(mcp2221_t* device, mcp2221_dac_ref_t ref, int value);

/**
* @brief Set the ADC voltage reference (SRAM)
*
* @param [device] Device to operate on
* @param [ref] Voltage reference
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_setADC(mcp2221_t* device, mcp2221_adc_ref_t ref);

/**
* @brief Set the interrupt trigger mode and optionally clear pending interrupt (SRAM)
*
* @param [device] Device to operate on
* @param [trig] Trigger mode
* @param [clearInt] Clear pending interrupt (0 = Don't clear, 1 = Clear)
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_setInterrupt(mcp2221_t* device, mcp2221_int_trig_t trig, int clearInt);

/**
* @brief Apply GPIO configuration
*
* @param [device] Device to operate on
* @param [confSet] Pointer to ::mcp2221_gpioconfset_t struct
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_setGPIOConf(mcp2221_t* device, mcp2221_gpioconfset_t* confSet);

/**
* @brief Set GPIO pin output values
*
* @param [device] Device to operate on
* @param [pins] Which GPIO pins should the new value be applied to
* @param [value] The new value
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_setGPIO(mcp2221_t* device, mcp2221_gpio_t pins, mcp2221_gpio_value_t value);

/**
* @brief Get the current clock output divider (SRAM)
*
* @param [device] Device to operate on
* @param [div] Pointer to ::mcp2221_clkdiv_t variable where value will be placed
* @param [duty] Pointer to ::mcp2221_clkduty_t variable where value will be placed
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_getClockOut(mcp2221_t* device, mcp2221_clkdiv_t* div, mcp2221_clkduty_t* duty);

/**
* @brief Get the current DAC voltage reference and output value (SRAM)
*
* @param [device] Device to operate on
* @param [ref] Pointer to ::mcp2221_dac_ref_t variable where value will be placed
* @param [value] Pointer to int variable where value will be placed
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_getDAC(mcp2221_t* device, mcp2221_dac_ref_t* ref, int* value);

/**
* @brief Get the current ADC voltage reference (SRAM)
*
* @param [device] Device to operate on
* @param [ref] Pointer to ::mcp2221_adc_ref_t variable where value will be placed
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_getADC(mcp2221_t* device, mcp2221_adc_ref_t* ref);

/**
* @brief Get the current interrupt trigger mode (SRAM)
*
* @param [device] Device to operate on
* @param [trig] Pointer to ::mcp2221_int_trig_t variable where value will be placed
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_getInterrupt(mcp2221_t* device, mcp2221_int_trig_t* trig);

/**
* @brief Get the current GPIO configuration (SRAM)
*
* @param [device] Device to operate on
* @param [confGet] Pointer to ::mcp2221_gpioconfset_t struct where data will be placed
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_getGPIO(mcp2221_t* device, mcp2221_gpioconfset_t* confGet);

/**
* @brief Read ADC values
*
* @param [device] Device to operate on
* @param [values] Int array of at least ::MCP2221_ADC_COUNT elements where values will be placed
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_readADC(mcp2221_t* device, int values[MCP2221_ADC_COUNT]);

/**
* @brief Read interrupt state
*
* @param [device] Device to operate on
* @param [state] Pointer to variable where state will be placed (0 = not triggered, 1 = triggered)
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_readInterrupt(mcp2221_t* device, int* state);

/**
* @brief Clear interrupt state
*
* @param [device] Device to operate on
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_clearInterrupt(mcp2221_t* device);

/**
* @brief Read GPIO values
*
* @param [device] Device to operate on
* @param [values] ::mcp2221_gpio_value_t array of at least ::MCP2221_GPIO_COUNT elements where values will be placed
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_readGPIO(mcp2221_t* device, mcp2221_gpio_value_t values[MCP2221_GPIO_COUNT]);

/**
* @brief Save new manufacturer USB descriptor string to flash (max 30 characters)
*
* @param [device] Device to operate on
* @param [buffer] The wide string buffer to read from
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveManufacturer(mcp2221_t* device, wchar_t* buffer);

/**
* @brief Save new product USB descriptor string to flash (max 30 characters)
*
* @param [device] Device to operate on
* @param [buffer] The wide string buffer to read from
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveProduct(mcp2221_t* device, wchar_t* buffer);

/**
* @brief Save new serial USB descriptor string to flash (max 30 characters)
*
* @param [device] Device to operate on
* @param [buffer] The wide string buffer to read from
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveSerial(mcp2221_t* device, wchar_t* buffer);

/**
* @brief Save new VID and PID to flash
*
* @param [device] Device to operate on
* @param [vid] New VID
* @param [pid] New PID
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveVIDPID(mcp2221_t* device, int vid, int pid);

/**
* @brief Enable/disable enumerating with serial number
*
* @param [device] Device to operate on
* @param [enumerate] 1 = Enable, 0 = Disable
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveSerialEnumerate(mcp2221_t* device, int enumerate);

/**
* @brief Set USB current limit
*
* @param [device] Device to operate on
* @param [milliamps] 2 - 500
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveMilliamps(mcp2221_t* device, int milliamps);

/**
* @brief Set power source
*
* @param [device] Device to operate on
* @param [source] Power source
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_savePowerSource(mcp2221_t* device, mcp2221_pwrsrc_t source);

/**
* @brief Enable/disable remote host wakeup
*
* @param [device] Device to operate on
* @param [wakeup] Enable/disable
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveRemoteWakeup(mcp2221_t* device, mcp2221_wakeup_t wakeup);

/**
* @brief Save polarity of dedicated pin functions to flash
*
* @param [device] Device to operate on
* @param [pin] Which dedicated pin
* @param [polarity] 0 = Default off, 1 = Default on
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_savePolarity(mcp2221_t* device, mcp2221_dedipin_t pin, int polarity);

/**
* @brief Save clock reference output settings to flash
*
* @param [device] Device to operate on
* @param [div] Frequency divider from 48MHz
* @param [duty] Duty cycle
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveClockOut(mcp2221_t* device, mcp2221_clkdiv_t div, mcp2221_clkduty_t duty);

/**
* @brief Save the DAC voltage reference and output value to flash
*
* @param [device] Device to operate on
* @param [ref] Voltage reference
* @param [value] Output value (0 - 31)
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveDAC(mcp2221_t* device, mcp2221_dac_ref_t ref, int value);

/**
* @brief Save the ADC voltage reference to flash
*
* @param [device] Device to operate on
* @param [ref] Voltage reference
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveADC(mcp2221_t* device, mcp2221_adc_ref_t ref);

/**
* @brief Save the interrupt trigger mode to flash
*
* @param [device] Device to operate on
* @param [trig] Trigger mode
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveInterrupt(mcp2221_t* device, mcp2221_int_trig_t trig);

/**
* @brief Save GPIO configuration to flash
*
* @param [device] Device to operate on
* @param [confSet] Pointer to ::mcp2221_gpioconfset_t struct to place configuration data into
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_saveGPIOConf(mcp2221_t* device, mcp2221_gpioconfset_t* confSet);

/**
* @brief Read manufacturer USB descriptor string from flash
*
* @param [device] Device to operate on
* @param [buffer] Wide string buffer of at least 31 elements (62 bytes)
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadManufacturer(mcp2221_t* device, wchar_t* buffer);

/**
* @brief Read product USB descriptor string from flash
*
* @param [device] Device to operate on
* @param [buffer] Wide string buffer of at least 31 elements (62 bytes)
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadProduct(mcp2221_t* device, wchar_t* buffer);

/**
* @brief Read serial USB descriptor string from flash
*
* @param [device] Device to operate on
* @param [buffer] Wide string buffer of at least 31 elements (62 bytes)
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadSerial(mcp2221_t* device, wchar_t* buffer);

/**
* @brief Read VID and PID from flash
*
* @param [device] Device to operate on
* @param [vid] TODO
* @param [pid]
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadVIDPID(mcp2221_t* device, int* vid, int* pid);

/**
* @brief Read serial enumeration setting from flash
*
* @param [device] Device to operate on
* @param [enumerate] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadSerialEnumerate(mcp2221_t* device, int* enumerate);

/**
* @brief Read current limit from flash
*
* @param [device] Device to operate on
* @param [milliamps] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadMilliamps(mcp2221_t* device, int* milliamps);

/**
* @brief Read power source from flash
*
* @param [device] Device to operate on
* @param [source] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadPowerSource(mcp2221_t* device, mcp2221_pwrsrc_t* source);

/**
* @brief Read remote USB host wakeup from flash
*
* @param [device] Device to operate on
* @param [wakeup] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadRemoteWakeup(mcp2221_t* device, mcp2221_wakeup_t* wakeup);

/**
* @brief Read dedicated pin polarity from flash
*
* @param [device] Device to operate on
* @param [pin] The dedicated pin to read
* @param [polarity] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadPolarity(mcp2221_t* device, mcp2221_dedipin_t pin, int* polarity);

/**
* @brief Read clock output settings from flash
*
* @param [device] Device to operate on
* @param [div] Pointer to ::mcp2221_clkdiv_t variable where value will be placed
* @param [duty] Pointer to ::mcp2221_clkduty_t variable where value will be placed
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadClockOut(mcp2221_t* device, mcp2221_clkdiv_t* div, mcp2221_clkduty_t* duty);

/**
* @brief Read DAC settings from flash
*
* @param [device] Device to operate on
* @param [ref] TODO
* @param [value] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadDAC(mcp2221_t* device, mcp2221_dac_ref_t* ref, int* value);

/**
* @brief Read ADC settings from flash
*
* @param [device] Device to operate on
* @param [ref] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadADC(mcp2221_t* device, mcp2221_adc_ref_t* ref);

/**
* @brief Read interrupt settings from flash
*
* @param [device] Device to operate on
* @param [trig] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadInterrupt(mcp2221_t* device, mcp2221_int_trig_t* trig);

/**
* @brief Read GPIO config from flash
*
* @param [device] Device to operate on
* @param [confSet] TODO
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_loadGPIOConf(mcp2221_t* device, mcp2221_gpioconfset_t* confSet);

/**
* @brief Perform an I2C write
*
* @param [device] Device to operate on
* @param [address] I2C slave address (7 bit addresses only)
* @param [data] Data to send
* @param [len] Number of bytes to send (max 60)
* @param [type] TODO
* @return ::mcp2221_error error code
* @note I2C is not fully implemented yet
*/
mcp2221_error mcp2221_i2cWrite(mcp2221_t* device, int address, void* data, int len, mcp2221_i2crw_t type);

/**
* @brief Perform an I2C read
*
* @param [device] Device to operate on
* @param [address] I2C slave address (7 bit addresses only)
* @param [len] Number of bytes to read (max 60)
* @param [type] TODO
* @return ::mcp2221_error error code
* @note I2C is not fully implemented yet
*/
mcp2221_error mcp2221_i2cRead(mcp2221_t* device, int address, int len, mcp2221_i2crw_t type);

/**
* @brief Get the data that was read
*
* @param [device] Device to operate on
* @param [data] Buffer to place data into
* @param [len] Number of bytes to read (max 60)
* @return ::mcp2221_error error code
* @note I2C is not fully implemented yet
*/
mcp2221_error mcp2221_i2cGet(mcp2221_t* device, void* data, int len);

/**
* @brief TODO
*
* @param [device] Device to operate on
* @return ::mcp2221_error error code
* @note I2C is not fully implemented yet
*/
mcp2221_error mcp2221_i2cCancel(mcp2221_t* device);

/**
* @brief TODO
*
* @param [device] Device to operate on
* @param [state] TODO
* @return ::mcp2221_error error code
* @note I2C is not fully implemented yet
*/
mcp2221_error mcp2221_i2cState(mcp2221_t* device, mcp2221_i2c_state_t* state);

/**
* @brief TODO
*
* @param [device] Device to operate on
* @param [i2cdiv] TODO
* @return ::mcp2221_error error code
* @note I2C is not fully implemented yet
*/
mcp2221_error mcp2221_i2cDivider(mcp2221_t* device, int i2cdiv);

/**
* @brief Read raw values of I2C pins. Allows using these pins as 2 additional input pins
*
* @param [device] Device to operate on
* @param [pins] Pointer to mcp2221_i2cpins_t struct to place values into
* @return ::mcp2221_error error code
*/
mcp2221_error mcp2221_i2cReadPins(mcp2221_t* device, mcp2221_i2cpins_t* pins);

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCP2221_H_ */
