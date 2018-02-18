//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//

/// \brief A template class enabling packet-based Serial communication.
///
/// The template parameters allow the user to define their own
/// custom packet marker and receive buffer size.
///
/// \tparam PacketMarker The byte value used to mark the packet boundary.
/// \tparam BufferSize The number of bytes allocated for the receive buffer.
template<uint8_t PacketMarker = 0, size_t BufferSize = 256>
class TextSerial_
{
public:
    /// \brief A typedef describing the packet handler method.
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(const uint8_t* buffer, size_t size);
    ///
    /// where buffer is a pointer to the incoming buffer array, and size is the
    /// number of bytes in the incoming buffer.
    typedef void (*PacketHandlerFunction)(const uint8_t* buffer, size_t size);

    /// \brief Construct a default TextSerial_ device.
    TextSerial_():
        _receiveBufferIndex(0),
        _stream(NULL),
        _onPacketFunction(NULL)
    {
    }

    /// \brief Destroy the TextSerial_ device.
    ~TextSerial_()
    {
    }

    /// \brief Begin a default serial connection with the given speed.
    ///
    /// The default Serial port `Serial` and default config `SERIAL_8N1` will be
    /// used. For example:
    ///
    ///     TextSerial myTextSerial;
    ///
    ///     void setup()
    ///     {
    ///         myTextSerial.begin(9600);
    ///     }
    ///
    /// \param speed The serial data transmission speed in bits / second (baud).
    /// \sa https://www.arduino.cc/en/Serial/Begin
    void begin(unsigned long speed)
    {
        Serial.begin(speed);
        _stream = &Serial;
    }

    /// \brief The update function services the serial connection.
    ///
    /// This must be called often, ideally once per `loop()`, e.g.:
    ///
    ///     void loop()
    ///     {
    ///         // Other program code.
    ///
    ///         myTextSerial.update();
    ///     }
    ///
    void update()
    {
        if (_stream == NULL)
            return;

        while (_stream->available() > 0) {
            uint8_t data = _stream->read();

            if (data == PacketMarker) {
                if (_onPacketFunction) {
                    _onPacketFunction(_receiveBuffer, _receiveBufferIndex);
                }
                _receiveBufferIndex = 0;
            } else {
                if ((_receiveBufferIndex + 1) < BufferSize) {
                    _receiveBuffer[_receiveBufferIndex++] = data;
                } else {
                    // Error, buffer overflow if we write.
                }
            }
        }
    }

    /// \brief Set a packet of data.
    ///
    /// This function will send an arbitrary packet of data. After
    /// sending, it will send the specified `PacketMarker` defined in the
    /// template parameters.
    ///
    ///     // Make an array.
    ///     uint8_t myPacket[2] = { 255, 10 };
    ///
    ///     // Send the array.
    ///     myTextSerial.send(myPacket, 2);
    ///
    /// \param buffer A pointer to a data buffer.
    /// \param size The number of bytes in the data buffer.
    void send(const uint8_t* buffer, size_t size) const
    {
        if(_stream == NULL || buffer == NULL || size == 0) return;

        _stream->write(buffer, size);
        _stream->write(PacketMarker);
    }

    /// \brief Set the function that will receive packets.
    ///
    /// This function will be called when data is read from the serial stream
    /// connection. The packet will be passed to the packet handler.
    /// The packet handler must have the form:
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(const uint8_t* buffer, size_t size);
    ///
    /// The packet handler would then be registered like this:
    ///
    ///     myTextSerial.setPacketHandler(&onPacketReceived);
    ///
    /// Setting a packet handler will remove all other packet handlers.
    ///
    /// \param onPacketFunction A pointer to the packet handler function.
    void setPacketHandler(PacketHandlerFunction onPacketFunction)
    {
        _onPacketFunction = onPacketFunction;
    }

private:
    TextSerial_(const TextSerial_&);
    TextSerial_& operator = (const TextSerial_&);

    uint8_t _receiveBuffer[BufferSize];
    size_t _receiveBufferIndex = 0;

    Stream* _stream = NULL;

    PacketHandlerFunction _onPacketFunction = NULL;
};
