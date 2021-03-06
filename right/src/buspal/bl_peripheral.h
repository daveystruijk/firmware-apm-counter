#ifndef __BL_PERIPHERAL_H__
#define __BL_PERIPHERAL_H__

//! @brief Peripheral type bit mask definitions.
//!
//! These bit mask constants serve multiple purposes. They are each a unique value that identifies
//! a peripheral type. They are also the mask for the bits used in the bootloader configuration
//! flash region to list available peripherals and control which peripherals are enabled.
enum _peripheral_types
{
    kPeripheralType_UART     = (1 << 0),
    kPeripheralType_I2CSlave = (1 << 1),
    kPeripheralType_SPISlave = (1 << 2),
    kPeripheralType_CAN      = (1 << 3),
    kPeripheralType_USB_HID  = (1 << 4),
    kPeripheralType_USB_CDC  = (1 << 5),
    kPeripheralType_USB_DFU  = (1 << 6),
    kPeripheralType_USB_MSC  = (1 << 7),
};

// Forward declaration.
typedef struct PeripheralDescriptor peripheral_descriptor_t;

typedef void (*serial_byte_receive_func_t)(uint8_t);

//! @brief Peripheral control interface.
typedef struct _peripheral_control_interface
{
    status_t (*init)(const peripheral_descriptor_t *self, serial_byte_receive_func_t function);
    void (*shutdown)(const peripheral_descriptor_t *self);
} peripheral_control_interface_t;

//! @brief Peripheral abstract byte interface.
typedef struct _peripheral_byte_inteface
{
    status_t (*read)(const peripheral_descriptor_t *self, uint8_t *buffer, uint32_t requestedBytes);
    status_t (*write)(const peripheral_descriptor_t *self, const uint8_t *buffer, uint32_t byteCount);
} peripheral_byte_inteface_t;

//! @brief Packet types.
typedef enum _packet_type
{
    kPacketType_Command, //!< Send or expect a command packet
    kPacketType_Data     //!< Send or expect a data packet
} packet_type_t;

//! @brief Peripheral Packet Interface.
typedef struct _peripheral_packet_interface
{
    status_t (*init)(const peripheral_descriptor_t *self);
    status_t (*readPacket)(const peripheral_descriptor_t *self,
                           uint8_t **packet,
                           uint32_t *packetLength,
                           packet_type_t packetType);
    status_t (*writePacket)(const peripheral_descriptor_t *self,
                            const uint8_t *packet,
                            uint32_t byteCount,
                            packet_type_t packetType);
    void (*abortDataPhase)(const peripheral_descriptor_t *self);
    status_t (*finalize)(const peripheral_descriptor_t *self);
    uint32_t (*getMaxPacketSize)(const peripheral_descriptor_t *self);
    void (*byteReceivedCallback)(uint8_t byte);
} peripheral_packet_interface_t;

//! @brief Peripheral descriptor.
//!
//! Instances of this struct describe a particular instance of a peripheral that is
//! available for bootloading.
struct PeripheralDescriptor
{
    //! @brief Bit mask identifying the peripheral type.
    //!
    //! See #_peripheral_types for a list of valid bits.
    uint32_t typeMask;

    //! @brief The instance number of the peripheral.
    uint32_t instance;

    //! @brief Control interface for the peripheral.
    const peripheral_control_interface_t *controlInterface;

    //! @brief Byte-level interface for the peripheral.
    //!
    //! May be NULL since not all periperhals support this interface.
    const peripheral_byte_inteface_t *byteInterface;

    //! @brief Packet level interface for the peripheral.
    const peripheral_packet_interface_t *packetInterface;
};

extern const peripheral_descriptor_t g_peripherals[];

#endif
