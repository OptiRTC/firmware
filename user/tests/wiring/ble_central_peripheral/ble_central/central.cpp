#include "Particle.h"
#include "unit-test/unit-test.h"

#if Wiring_BLE == 1

#define SCAN_RESULT_COUNT       20

BleScanResult results[SCAN_RESULT_COUNT];

BleCharacteristic peerTxCharacteristic;
BleCharacteristic peerRxCharacteristic;
BlePeerDevice peer;

const String str1("This is string 1.");
const String str2("Hello from Particle.");
const String str3("Have a good day!");
bool str1Rec = false, str2Rec = false, str3Rec = false;

static void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    String str((const char*)data, len);
    if (str == str1) {
        str1Rec = true;
    }
    if (str == str2) {
        str2Rec = true;
    }
    if (str == str3) {
        str3Rec = true;
    }
}

test(BLE_01_Central_Scan_And_Connect) {
    peerTxCharacteristic.onDataReceived(onDataReceived, &peerTxCharacteristic);

    int ret = BLE.setScanTimeout(100); // Scan timeout: 1s
    assertEqual(ret, 0);

    Serial.println("BLE starts scanning...");

    size_t wait = 20; // Scanning for 20s.
    while (!BLE.connected() && wait > 0) {
        size_t count = BLE.scan(results, SCAN_RESULT_COUNT);
        if (count > 0) {
            for (uint8_t i = 0; i < count; i++) {
                BleUuid foundServiceUUID;
                size_t svcCount = results[i].advertisingData.serviceUUID(&foundServiceUUID, 1);
                if (svcCount > 0 && foundServiceUUID == "6E400001-B5A3-F393-E0A9-E50E24DCCA9E") {
                    peer = BLE.connect(results[i].address);
                    if (peer.connected()) {
                        peer.getCharacteristicByDescription(peerTxCharacteristic, "tx");
                        peer.getCharacteristicByUUID(peerRxCharacteristic, "6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
                    }
                    break;
                }
            }
        }
        wait--;
    }
    assertTrue(wait > 0);

    Serial.println("BLE connected.");
}

test(BLE_02_Central_Send_Characteristic_Value_With_Auto_Ack) {
    int ret = peerRxCharacteristic.setValue(str1);
    assertTrue(ret == str1.length());
}

test(BLE_03_Central_Send_Characteristic_Value_With_Ack) {
    int ret = peerRxCharacteristic.setValue(str2, BleTxRxType::ACK);
    assertTrue(ret == str2.length());
}

test(BLE_04_Central_Send_Characteristic_Value_Without_Ack) {
    int ret = peerRxCharacteristic.setValue(str3, BleTxRxType::NACK);
    assertTrue(ret == str3.length());
}

test(BLE_05_Central_Receive_Characteristic_Value_String1) {
    size_t wait = 5; //Wait for 5s to receive the data from BLE peripheral.
    while (!str1Rec && wait > 0) {
        delay(100);
        wait--;
    }
    assertTrue(wait > 0);
}

test(BLE_06_Central_Receive_Characteristic_Value_String2) {
    size_t wait = 5; //Wait for 5s to receive the data from BLE peripheral.
    while (!str2Rec && wait > 0) {
        delay(100);
        wait--;
    }
    assertTrue(wait > 0);
}

test(BLE_07_Central_Receive_Characteristic_Value_String3) {
    size_t wait = 5; //Wait for 5s to receive the data from BLE peripheral.
    while (!str3Rec && wait > 0) {
        delay(100);
        wait--;
    }
    assertTrue(wait > 0);
}

#endif // #if Wiring_BLE == 1
