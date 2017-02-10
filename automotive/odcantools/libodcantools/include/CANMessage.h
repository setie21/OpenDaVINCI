#ifndef CAN_MESSAGE_H_
#define CAN_MESSAGE_H_

#include <automotivedata/generated/automotive/GenericCANMessage.h>

#include <vector>
#include <map>

namespace automotive {
    namespace odcantools {
        class CANMessage;
        
        class CANSignal {
            friend class CANMessage;
            
            public:
                CANSignal(const uint8_t, const uint8_t, const string, const string, const double, const double, const double, const double);
                
            private:
                CANSignal();
                
                uint8_t m_startBit; // lsb of the signal
                uint8_t m_length; // length in bits
                string m_signedness; // signed or unsigned
                string m_endianness; // little endian (intel) or big endian (motorola)
                double m_factor; // factor to be multiplied
                double m_offset; // offset to be added
                double m_rangeB; // lower range limit for the value
                double m_rangeE; // upper range limit for the value
        };
    
        class CANMessage : public automotive::GenericCANMessage {
            public: 
                CANMessage(const GenericCANMessage&);
                CANMessage(const GenericCANMessage&, const string);
                CANMessage(const GenericCANMessage&, const bool);
                
                uint64_t getToUINT64();
                
                bool addSignal(const uint16_t, const CANSignal);
                double decodeSignal(const uint16_t);
                double decodeSignal(const CANSignal);
                void encodeSignal(const uint16_t, double);
                void encodeSignal(const CANSignal, double);
            
            private:
                CANMessage();
                CANMessage(const CANMessage&);
                
                void setFromUINT64(const uint64_t, const uint8_t);
                void advanceByteMask(uint8_t&);
                void resetByteMask(uint8_t&);
                void advanceByteNumberInCorrectEndianness(int8_t&, const string);
                bool checkByteMaskInLastPosition(const uint8_t);
                uint8_t getStartBitInByteInCorrectByteOrder(const uint8_t);
                void insertRawSignal(const CANSignal, const uint64_t);
                uint64_t extractRawSignal(const CANSignal);
                //double getSignal(const uint8_t, const uint8_t, const string, const double, const double, const double, const double);
            
            private: 
                map<const uint16_t, const CANSignal> m_signals;
                vector<uint8_t> m_payload;
                bool m_vectorByteOrder;
        };
        
    }
}

#endif /* CAN_MESSAGE_H_ */

