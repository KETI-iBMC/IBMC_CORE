#include <ipmi/sensor_define.hpp>

const float m_val_vol[] = {
    63.15, 17.19, 7.06, 7.06, 63.15, 9.41, 20.22, 9.40,
    9.40,  7.06,  7.06, 7.06, 7.06,  7.06, 7.06}; // 15.65==12(V)*1000*1.8(V)/(1.348*1024)
const float input_volt[] = {1.348, 1.355, 1.0, 1.05, 1.348, 1.35, 0.998, 1.276,
                            1.276, 1.2,   1.2, 1.2,  1.2,   1.0,  1.0};
const float nomi[] = {1.843, 1.69, 0.929, 1.347, 1.724, 1.69,  1.69, 1.69,
                      1.69,  1.2,  1.2,   1.2,   1.2,   0.929, 0.929};
const float volt[] = {12.0, 3.3, 1.0, 1.05, 12.0, 1.8, 3.0, 1.7,
                      1.7,  1.2, 1.2, 1.2,  1.2,  1.0, 1.0};
sensor_thresh_t peb_sensors[PEB_SENSOR_COUNT] = {
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE,
     0x00, // entry id
     0x01, // instance
     0x00, // init
     0x68, // SensorCapabilities
     0x02, // SensorType
     0x01, // Event/ReadingType
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // "SensorUnits1"
     SENSOR_UNIT_VOLTS, // "SensorUnits2-BaseUnit"
     0x00,              // "SensorUnits3-ModifierUnit"
     0x00,              // Linearization
     0x3f,              // m_val
     0x00,              // m_tolerance
     0x00,              //"b_val
     0x0,               // b_accuracy //Accuracy-Accuracyexponent
     0x1,               // Rexponent-Bexponent  accuracy_dir
     0xD0,              // Rexponent-Bexponent rb_exp
     0x0,               // analog_flags
     (0x00),
     0x00,
     0x00,
     0x00, // max
     0x00,
     0x00,
     0xd6,
     0xd0,
     0x00,
     0xa9,
     0xae,
     0x00, // RESERVEd
     0x00, // RESERVEd
     0x00, // OEM
     0x11,
     "__P12V_PSU1"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 1,
     0x00, // entry id
     0x01, // instance
     0x00, // init
     0x68, // SensorCapabilities
     0x02, // SensorType
     0x01, // Event/ReadingType
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // "SensorUnits1"
     SENSOR_UNIT_VOLTS, // "SensorUnits2-BaseUnit"
     0x00,              // "SensorUnits3-ModifierUnit"
     0x00,              // Linearization
     0x3f,              // m_val
     0x00,              // m_tolerance
     0x00,              //"b_val
     0x0,               // b_accuracy //Accuracy-Accuracyexponent
     0x1,               // Rexponent-Bexponent  accuracy_dir
     0xD0,              // Rexponent-Bexponent rb_exp
     0x0,               // analog_flags
     (0x00),
     0x00,
     0x00,
     0x00, // max
     0x00,
     0x00,
     0xd6,
     0xd0,
     0x00,
     0xa9,
     0xae,
     0x00,
     0x00,
     0x00,
     0x11,
     "__P12V_PSU2"},

    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 2,
     0x00, // entry id
     0x01, // instance
     0x00, // init
     0x68, // SensorCapabilities
     0x02, // SensorType
     0x01, // Event/ReadingType
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // "SensorUnits1"
     SENSOR_UNIT_VOLTS, // "SensorUnits2-BaseUnit"
     0x00,              // "SensorUnits3-ModifierUnit"
     0x00,              // Linearization
     0x3f,              // m_val
     0x00,              // m_tolerance
     0x00,              //"b_val
     0x0,               // b_accuracy //Accuracy-Accuracyexponent
     0x1,               // Rexponent-Bexponent  accuracy_dir
     0xD0,              // Rexponent-Bexponent rb_exp
     0x0,               // analog_flags
     (0x00),
     0x00,
     0x00,
     0x00,
     0x00,
     P3V3_VOUT_UNR_THRESHOLD,
     P3V3_VOUT_UCR_THRESHOLD,
     P3V3_VOUT_UNC_THRESHOLD,
     P3V3_VOUT_LNR_THRESHOLD,
     P3V3_VOUT_LCR_THRESHOLD,
     P3V3_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00,
     0x06,
     "__P3V3"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 3,
     0x00,
     0x01,
     0x00,
     0x68,
     0x02,
     0x01,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     0x04,
     0x00,
     0x00,
     (unsigned char)0x06,
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (0x00),
     0x00,
     0x00,
     0x00,
     0x00,
     P3V3_VOUT_UNR_THRESHOLD,
     P3V3_VOUT_UCR_THRESHOLD,
     P3V3_VOUT_UNC_THRESHOLD,
     P3V3_VOUT_LNR_THRESHOLD,
     P3V3_VOUT_LCR_THRESHOLD,
     P3V3_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00,
     0x05,
     "__P5V"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 4,
     0x07,
     0x01, // instance
     0x00,
     0x68,
     0x02,
     0x01,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     0x04,
     0x00,
     0x00,
     (unsigned char)0x60,
     M_TOLERANCE,
     0x00,
     0x0,
     0x0,
     0xc0,
     0x0,
     (unsigned char)0x00,
     0x00,
     0x00,
     0x00, // SensorMaximumReading
     0x00,
     0x00, // RecoverableThreshold
     0x7e,
     0x74,
     0x00,
     0x5d,
     0x66,
     0x00, // Reserved
     0x00,
     0x00,
     0x0a,
     "__PVNN_PCH"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 5,
     0x07,
     0x0d,
     0x7F,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_VOLTS,
     0x00,
     0x00,
     (unsigned char)0x63,
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (unsigned char)0xc0,
     0x00,
     0x00,
     0x00,
     0x00,
     PVNN_VOUT_UNR_THRESHOLD,
     PVNN_VOUT_UCR_THRESHOLD,
     PVNN_VOUT_UNC_THRESHOLD,
     PVNN_VOUT_LNR_THRESHOLD,
     PVNN_VOUT_LCR_THRESHOLD,
     PVNN_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00,
     0x11,
     "__P1V05_PCH"},

    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 6,
     0x00, // entity id
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensorUnit1
     SENSOR_UNIT_VOLTS, // sensorUnit2? sensonUnit2면 0x04
     0x00,              // sensorUnit3
     0x00,              // Linerization
     (0x01),            //(unsigned char)9.41, // M 0x01
     M_TOLERANCE,
     0x00,   // B
     0x00,   // B-Accuracyexponent
     0x00,   // A-A
     0xe0,   // RB
     0x00,   // AFlags
     (0x00), // ((unsigned char)(1000 * 1.8) / 9.41),//NominalReading
     0x00,   // NMax
     0x00,   // NMin
     0x00,   // SensorMax
     0x00,   // SensorMin
     P1V8_VOUT_UNR_THRESHOLD,
     P1V8_VOUT_UCR_THRESHOLD,
     P1V8_VOUT_UNC_THRESHOLD,
     P1V8_VOUT_LNR_THRESHOLD,
     P1V8_VOUT_LCR_THRESHOLD,
     P1V8_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // OEM
     0x06, // LengthCode
     "__P1V8"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 7,
     0x00, // entity id
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensor unit1
     SENSOR_UNIT_VOLTS, // sensor unit2 0x04임 이름 다름
     0x00,              // sensor unit3
     0x00,              // Linear
     (0xc7),            //(unsigned char)20.22,  //M 0xc7 값이 다름
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (0x00), //((unsigned char)(1000 * 3.0) / 20.22), //nominalReading
     0x00,   // MMax
     0x00,   // NMin
     0x00,   // SensorMax
     0x00,   // SensorMin
     BAT_VOUT_UNR_THRESHOLD,
     BAT_VOUT_UCR_THRESHOLD,
     BAT_VOUT_UNC_THRESHOLD,
     BAT_VOUT_LNR_THRESHOLD,
     BAT_VOUT_LCR_THRESHOLD,
     BAT_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // FRU_PEB, //OEM 0x00 @
     0x05,
     "__BAT"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 8,
     0x00, // entity id
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensorunit1
     SENSOR_UNIT_VOLTS, ////sensorunit2 이름 다름 0x04
     0x00,              // sensorunit3
     0x00,
     (0x5d), //(unsigned char)1.8,// M 0x5d
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (0x00), //((unsigned char)(1000 * 0.8) / 1.8), // nominal 값이 다름
     0x00,
     0x00,
     0x00,
     0x00,
     PVCCIN_VOUT_UNR_THRESHOLD,
     PVCCIN_VOUT_UCR_THRESHOLD,
     PVCCIN_VOUT_UNC_THRESHOLD,
     PVCCIN_VOUT_LNR_THRESHOLD,
     PVCCIN_VOUT_LCR_THRESHOLD,
     PVCCIN_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // OEM 0x00
     0x08, // lengthCode
     "__PVCCIN"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 9,
     0x00, // entity id
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensorunit1
     SENSOR_UNIT_VOLTS, // sensorunit2 이름이 다름 0x04
     0x00,              // sensorunit3
     0x00,
     (0x60), //(unsigned char)9.40, // M 0x60
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (0x00), //((unsigned char)(1000 * 0.8) / 9.40),//nominal
     0x00,   // nMax
     0x00,   // nMin
     0x00,   // sensorMax
     0x00,   // sensorMin
     PVNN_VOUT_UNR_THRESHOLD,
     PVNN_VOUT_UCR_THRESHOLD,
     PVNN_VOUT_UNC_THRESHOLD,
     PVNN_VOUT_LNR_THRESHOLD,
     PVNN_VOUT_LCR_THRESHOLD,
     PVNN_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // OEM 이름 다름 0x00
     0x0f, // lengthCode
     "__PVNN_PCH_CPU0"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 10,
     0x00, // entity
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensorunit1
     SENSOR_UNIT_VOLTS, // 이름 다름 sensorunit2 0x04
     0x00,              // sensorunit3
     0x00,
     (0x01), //(unsigned char)1, // M 0x01
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xe0,
     0x00,
     (0x00), //((unsigned char)(1000 * 1.8) / 9.40), //nominal 0x00
     0x00,
     0x00,
     0x00,
     0x00,
     P1V8_VOUT_UNR_THRESHOLD,
     P1V8_VOUT_UCR_THRESHOLD,
     P1V8_VOUT_UNC_THRESHOLD,
     P1V8_VOUT_LNR_THRESHOLD,
     P1V8_VOUT_LCR_THRESHOLD,
     P1V8_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // OEM 0x00
     0x0f, // len
     "__P1V8_NACDELAY"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 11,
     0x00, // entity id
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // seonsorunit1
     SENSOR_UNIT_VOLTS, // 2 이름 다름 sensorunit2 0x04
     0x00,              // sensorunit3
     0x00,
     (0x01), //(unsigned char)7.06, //M 0x01
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xe0,
     0x00,
     (0x00), //((unsigned char)(1000 * 1.2) / 7.06), // nominal 0x00
     0x00,
     0x00,
     0x00,
     0x00,
     P1V2_VOUT_UNR_THRESHOLD,
     P1V2_VOUT_UCR_THRESHOLD,
     P1V2_VOUT_UNC_THRESHOLD,
     P1V2_VOUT_LNR_THRESHOLD,
     P1V2_VOUT_LCR_THRESHOLD,
     P1V2_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // OEM 00
     0x11, // len
     "__P1V2_VDDQ"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 12,
     0x00, // entity id
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensorunit1
     SENSOR_UNIT_VOLTS, // 이름 다름 sensorunit2
     0x00,              // sensorunit3
     0x00,
     (0x60), //(unsigned char)9.40, // M 0x60
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (0x00), //((unsigned char)(1000 * 0.8) / 9.40), //nominal 0x00
     0x00,
     0x00,
     0x00,
     0x00,
     PVNN_VOUT_UNR_THRESHOLD,
     PVNN_VOUT_UCR_THRESHOLD,
     PVNN_VOUT_UNC_THRESHOLD,
     PVNN_VOUT_LNR_THRESHOLD,
     PVNN_VOUT_LCR_THRESHOLD,
     PVNN_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // 이름 다름 OEM 0x00
     0x10, // len
     "__PVNN_NAC"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 13,
     0x00, // entry ID
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensorunit1
     SENSOR_UNIT_VOLTS, // 이름 다름 sensorunit2 0x04
     0x00,              // sensorunit3
     0x00,
     (0x63), //(unsigned char)7.06, //0x63
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (0x00), //((unsigned char)(1000 * 1.05) / 7.06), //nominal 0x00
     0x00,
     0x00,
     0x00,
     0x00,
     P1V05_VOUT_UNR_THRESHOLD,
     P1V05_VOUT_UCR_THRESHOLD,
     P1V05_VOUT_UNC_THRESHOLD,
     P1V05_VOUT_LNR_THRESHOLD,
     P1V05_VOUT_LCR_THRESHOLD,
     P1V05_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // 이름 다름 OEM
     0x11, // len
     "__P1V05_NAC"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 14,
     0x00, // entity id
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensorUnit1
     SENSOR_UNIT_VOLTS, // 이름 다름 sensorUnit2 0x04
     0x00,              // sensorUnit3
     0x00,
     (0xac), //(unsigned char)7.06, //M 0xac
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (0x00), //((unsigned char)(1000 * 2.5) / 7.06), //nominal 0x00
     0x00,
     0x00,
     0x00,
     0x00,
     PVPP_VOUT_UNR_THRESHOLD,
     PVPP_VOUT_UCR_THRESHOLD,
     PVPP_VOUT_UNC_THRESHOLD,
     PVPP_VOUT_LNR_THRESHOLD,
     PVPP_VOUT_LCR_THRESHOLD,
     PVPP_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // OEM 0x00
     0x06, // len
     "__PVPP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PEB_SENSOR_NUM_BASE + 15,
     0x00, // entity id
     0x01,
     0x00,
     0x68,
     SENSOR_TYPE_VOLTAGE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,              // sensorunit1
     SENSOR_UNIT_VOLTS, // 이름 다름 sensorunit2 0x04
     0x00,              // sensorunit3
     0x00,
     (0x60), //(unsigned char)3.53, // M 0x60
     M_TOLERANCE,
     0x00,
     0x00,
     0x00,
     0xc0,
     0x00,
     (0x00), //((unsigned char)(1000 * 0.6) / 3.53), // nominal 0x00
     0x00,
     0x00,
     0x00,
     0x00,
     PVTT_VOUT_UNR_THRESHOLD,
     PVTT_VOUT_UCR_THRESHOLD,
     PVTT_VOUT_UNC_THRESHOLD,
     PVTT_VOUT_LNR_THRESHOLD,
     PVTT_VOUT_LCR_THRESHOLD,
     PVTT_VOUT_LNC_THRESHOLD,
     0x00,
     0x00,
     0x00, // 이름 다름 OEM 0x00
     0x06, // len
     "__PVTT"},

};

sensor_thresh_t pdpb_sensors[PDPB_SENSOR_COUNT] = {
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU1,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_CPU_UNR_THRESHOLD,
     PDPB_CPU_UCR_THRESHOLD,
     PDPB_CPU_UNC_THRESHOLD,
     PDPB_CPU_LNR_THRESHOLD,
     PDPB_CPU_LCR_THRESHOLD,
     PDPB_CPU_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU1_TEMP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR0_TEMP_LM75,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     LM75_TEMP_UNR_THRESHOLD,
     LM75_TEMP_UCR_THRESHOLD,
     LM75_TEMP_UNC_THRESHOLD,
     LM75_TEMP_LNC_THRESHOLD,
     LM75_TEMP_LCR_THRESHOLD,
     LM75_TEMP_LNR_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__LM75_TEMP0"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR1_TEMP_LM75,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     LM75_TEMP_UNR_THRESHOLD,
     LM75_TEMP_UCR_THRESHOLD,
     LM75_TEMP_UNC_THRESHOLD,
     LM75_TEMP_LNC_THRESHOLD,
     LM75_TEMP_LCR_THRESHOLD,
     LM75_TEMP_LNR_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__LM75_TEMP1"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR2_TEMP_LM75,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     LM75_TEMP_UNR_THRESHOLD,
     LM75_TEMP_UCR_THRESHOLD,
     LM75_TEMP_UNC_THRESHOLD,
     LM75_TEMP_LNC_THRESHOLD,
     LM75_TEMP_LCR_THRESHOLD,
     LM75_TEMP_LNR_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__LM75_TEMP2"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR3_TEMP_LM75,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     LM75_TEMP_UNR_THRESHOLD,
     LM75_TEMP_UCR_THRESHOLD,
     LM75_TEMP_UNC_THRESHOLD,
     LM75_TEMP_LNC_THRESHOLD,
     LM75_TEMP_LCR_THRESHOLD,
     LM75_TEMP_LNR_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__LM75_TEMP3"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR4_TEMP_LM75,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     LM75_TEMP_UNR_THRESHOLD,
     LM75_TEMP_UCR_THRESHOLD,
     LM75_TEMP_UNC_THRESHOLD,
     LM75_TEMP_LNC_THRESHOLD,
     LM75_TEMP_LCR_THRESHOLD,
     LM75_TEMP_LNR_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__LM75_TEMP4"},

    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU0_CH0_DIMM0,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_MB_UNR_THRESHOLD,
     PDPB_MB_UCR_THRESHOLD,
     PDPB_MB_UNC_THRESHOLD,
     PDPB_MB_LNR_THRESHOLD,
     PDPB_MB_LCR_THRESHOLD,
     PDPB_MB_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU0_DIMM1_TEMP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU0_CH0_DIMM1,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_MB_UNR_THRESHOLD,
     PDPB_MB_UCR_THRESHOLD,
     PDPB_MB_UNC_THRESHOLD,
     PDPB_MB_LNR_THRESHOLD,
     PDPB_MB_LCR_THRESHOLD,
     PDPB_MB_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU0_DIMM2_TEMP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU0_CH0_DIMM2,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_MB_UNR_THRESHOLD,
     PDPB_MB_UCR_THRESHOLD,
     PDPB_MB_UNC_THRESHOLD,
     PDPB_MB_LNR_THRESHOLD,
     PDPB_MB_LCR_THRESHOLD,
     PDPB_MB_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU0_DIMM3_TEMP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU0_CH1_DIMM0,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_MB_UNR_THRESHOLD,
     PDPB_MB_UCR_THRESHOLD,
     PDPB_MB_UNC_THRESHOLD,
     PDPB_MB_LNR_THRESHOLD,
     PDPB_MB_LCR_THRESHOLD,
     PDPB_MB_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU0_DIMM4_TEMP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU0_CH1_DIMM1,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_MB_UNR_THRESHOLD,
     PDPB_MB_UCR_THRESHOLD,
     PDPB_MB_UNC_THRESHOLD,
     PDPB_MB_LNR_THRESHOLD,
     PDPB_MB_LCR_THRESHOLD,
     PDPB_MB_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU0_DIMM5_TEMP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU0_CH1_DIMM2,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_MB_UNR_THRESHOLD,
     PDPB_MB_UCR_THRESHOLD,
     PDPB_MB_UNC_THRESHOLD,
     PDPB_MB_LNR_THRESHOLD,
     PDPB_MB_LCR_THRESHOLD,
     PDPB_MB_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU0_DIMM6_TEMP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU0_CH2_DIMM0,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_MB_UNR_THRESHOLD,
     PDPB_MB_UCR_THRESHOLD,
     PDPB_MB_UNC_THRESHOLD,
     PDPB_MB_LNR_THRESHOLD,
     PDPB_MB_LCR_THRESHOLD,
     PDPB_MB_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU0_DIMM7_TEMP"},
    {BMC_SLAVE_ADDR,
     0x00,
     PDPB_SENSOR_TEMP_CPU0_CH2_DIMM1,
     0x0A,
     0x00,
     0x7F,
     0x64,
     SENSOR_TYPE_TEMPERATURE,
     SENSOR_EVENT_READING_THRESHOLD,
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_EVENT_ASSERTION_LS, SENSOR_EVENT_ASSERTION_MS},
     {SENSOR_MASK_READ_SETTABLE_LS, SENSOR_MASK_READ_SETTABLE_MS},
     0x00,
     SENSOR_UNIT_CELSIUS,
     0x00,
     0x00,
     100,
     0x00,
     0x00,
     0x0,
     0x01,
     0xE0,
     0x01,
     100,
     0xFF,
     0x00,
     0xFF,
     0x00,
     PDPB_MB_UNR_THRESHOLD,
     PDPB_MB_UCR_THRESHOLD,
     PDPB_MB_UNC_THRESHOLD,
     PDPB_MB_LNR_THRESHOLD,
     PDPB_MB_LCR_THRESHOLD,
     PDPB_MB_LNC_THRESHOLD,
     0x00,
     0x00,
     FRU_PDPB,
     0xCF,
     "__CPU0_DIMM8_TEMP"},
};