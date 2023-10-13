# Personal sound and image recording system on demand, using peripheral devices

Bachelor thesis project, combining areas of IoT and multimedia

## Description

From the thesis description:

Abstract.
This thesis focuses on the design and implementation of a personal system operated by a single user. The activated system, which is in a standby state and can be initiated by the user, enables recording of audio and video from the user’s surroundings using a personal smartwatch, personal smartphone, and an ESP32-CAM module. The system can record audio and video simultaneously and can be controlled using the aforementioned devices. The recorded data is transmitted in real-time and stored on an external server for later access.
The designed system is compact, portable, and allows for discreet initiation that is unnoticed by people in the user’s vicinity. Special attention has also been given to energy efficiency. According to the measurements conducted, the system can withstand over 40 hours in standby mode without recharging or over 8 hours in active recording and audio transmission mode.
To enhance the thesis and conduct better testing of the system, two additional alternative solutions for audio recording and transmission have been implemented. These solutions utilize different communication protocols: Bluetooth Classic and Socket.IO, aiming to compare the efficiency of the protocol used in the base solution, Bluetooth Low Energy. The Bluetooth Classic solution is competitive in terms of similar energy consumption compared to the base solution, with the significant advantage of enabling higher-quality audio transmission. On the other hand, the Socket.IO solution offers better audio quality but has considerably higher energy consumption, making it impractical.
Finally, the system has also been analyzed from a legal perspective. Potential cases of system misuse in public settings and the possibilities of using the recorded data as evidence in legal proceedings has been considered.

Keywords: Bluetooth Low Energy, Android, audio transmission, real-time system, smartwatch

## Getting Started

### Executing program

Project consists of four main components:

- Android App for a smartwatch, which records the audio and sends it in real-time the the gate/server using BLE/Bluetooth classic/Socket.IO; for complete version check this project [watch-audio-recorder](https://github.com/AdamRogowski/watch-audio-recorder);
- Android App for a smartphone which is a gate between the smartwatch and the server; for complete version check this project [mobile-ble-gate-for-audio](https://github.com/AdamRogowski/mobile-ble-gate-for-audio);
- ESP32-cam sketch for recording video and sending it in real-time to the server;
- nodeJS server which receives the audio/video from the peripheral devices and saves recordings locally.

To connect the gate to the server, mSocket with the server's IP has to be established in file: blePhoneCentral/app/src/main/java/com/example/blephonecentral/SocketHandler.kt

To establish webSocket connection between ESP32-cam and the server, explicit network credentials as well as server's IP have to be added to the sketch.

## Authors

Adam Rogowski

## Acknowledgments

Most relevant sources:

- [BLE-guide](https://punchthrough.com/android-ble-guide/)
- [BLEProof-collection](https://github.com/alexanderlavrushko/BLEProof-collection)
