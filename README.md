# S18 Software Challenge
Once upon a time you were a Waterloo Student. You were living a nice life talking about geese all day, eating at Lazeez and thinking about why you’re getting 60s in your courses. Too bad those days are gone 😞 Our world has run out of resources and there is no electricity. You and your friends are sent to Planet X to grab a battery so powerful that it can power the world for a billion years. You arrived at the planet but your pod randomly ran out of battery and you estimate that the battery is 50 Km away from your location. You are unable to walk the 50km due to dangerous aliens and the battery being too heavy to carry back. You will have to find a way to travel 50 km so that you can pick up the battery, charge your pod, and finally provide electricity to planet earth. You look around and see a Hyperloop tube with an old Hyperloop pod in it. You guys are lucky and the tube is fully functional but there is one caveat. The Hyperloop pod inside the tube has not been programmed yet and hence cannot function. You will have to program the pod and make it function so that you can get the battery and come back home!! You find a manual describing the pod hardware and details on how to create the software system. Read through the manual and get to work; The world is relying on you!!!

# General

Programming PodX is a big project and hence you have decided to split it into three components: Embedded, Communication/Backend, and Controls Dashboard. You will work as a group in three separate teams and work on each individual component separately. You will then come together and assemble all the components to test if your system works.

# Embedded

Embedded team is in-charge of connecting with PodX hardware and making it function. You will parse sensor data provided by the pod, filter out noise, implement safety procedures, and signal states to the pod so that it can move. Below are requirements for everything you will need to program the embedded chip. The micro controller used on the pod is **Arduino Uno**

## Embedded Data Packets

PodX provides all sorts of information in binary packets. These packets have a very specific structure and they need to be read the same way and write the same way. If a packet is wrongly structured, it will not be accepted by the pod. The binary packets have to be parsed by you and when you want to send data to the machine, you have to encode them. PodX communicates over I2C protocol and you can make your micro controller as a master and request and send data to Pod’s CPU. The slave address you need to connect to is `0x5A`. You can read about I2C protocol for Arduino here: https://www.arduino.cc/en/Reference/Wire. 

**Sensor Packet**
The Sensor packet contains all the information about the sensor data that PodX’s CPU sends. The packet specifications are as follows:


    [8][8][8][32][32][32][32][32][32][8]

**Total Size:** 32 bytes

|    **Position** |    **Num Bits** |    **Data Description**                                                                                                                         |    **Format** |
| --------------- | --------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- | ------------- |
| First           | `[8]`           | Starting Byte (**0x56**)                                                                                                                        | uint_8        |
| Second          | `[8]`           | Error States ([**explained below**](/doc/S18-Software-Challenge-EOnK0BZy0ZdCuYIgCCCEb#:uid=577504544055172273039840&h2=Error-States))           | uint_8        |
| Third           | `[8]`           | Current Pod State ID ([**explained below**](/doc/S18-Software-Challenge-EOnK0BZy0ZdCuYIgCCCEb#:uid=846660890523151700825388&h2=Command-States)) | uint_8        |
| Fourth          | `[32]`          | Time Stamp (**Current time elapsed**)                                                                                                           | float_32      |
| Fifth           | `[32]`          | Acceleration Data 1 **(Front Position)**                                                                                                        | float_32      |
| Sixth           | `[32]`          | Acceleration Data 2 **(Middle Position)**                                                                                                       | float_32      |
| Seventh         | `[32]`          | Acceleration Data 3 **(Read Position)**                                                                                                         | float_32      |
| Eight           | `[32]`          | Temperature Data 2 (**Braking System**)                                                                                                         | float_32      |
| Ninth           | `[32]`          | Temperature Data 3 (**Propulsion System**)                                                                                                      | float_32      |
| Tenth           | `[32]`          | Temperature Data 4 (**Motherboard**)                                                                                                            | float_32      |
| Last            | `[8]`           | Ending Byte (**0x23**)                                                                                                                          | uint_8        |


**Command Packet**
Command Packets are to be used to send information to PodX’s CPU. If the packet is not in the right format, it will be rejected and you can see the error message in the dashboard.


    [8][8][8]

**Total Size:** 3 bytes

| **Position**    |    **Num Bits** |    **Data Description**                                                                                                                     |    **Format** |
| --------------- | --------------- | ------------------------------------------------------------------------------------------------------------------------------------------- | ------------- |
| First           | `[8]`           | Starting Byte (**0x56**)                                                                                                                    | uint_8        |
| Second          | `[8]`           | State ID ([**options available below**](/doc/S18-Software-Challenge-EOnK0BZy0ZdCuYIgCCCEb#:uid=846660890523151700825388&h2=Command-States)) | uint_8        |
| Last            | `[8]`           | Ending Byte (**0x23**)                                                                                                                      | uint_8        |


**Command States**
States are used to control the movement of PodX on the Hyperloop track. PodX has a State Machine built into it. This means that for every given state only very specific states can be transitioned into. You will use [Command Packet](/doc/S18-Software-Challenge-EOnK0BZy0ZdCuYIgCCCEb#:uid=861258600186427427486217&h2=Command-Packet) to communicate these states with the pod so that it can take you to your destination. Following are the states that are valid and will be accepted:

|    **State Name** |    **State ID** |
| ----------------- | --------------- |
| IDLE              | **0x01**        |
| READY             | **0x02**        |
| ACCEL             | **0x03**        |
| COAST             | **0x04**        |
| BRAKE             | **0x05**        |
| STOP              | **0x06**        |

- PodX will start in `IDLE` state and will automatically transition to `READY` state after it is powered on. You have to wait until the pod is in `READY` state before doing anything.
- `ACCEL`, `COAST`, and `BRAKE` are the states that are controlled by you. This will determine how the pod moves through the tube.
  - `ACCEL` state can only be turned on when the pod is in `READY` or `COAST` state
  - `COAST` state can only be turned on when the pod is in `ACCEL` state
  - `BRAKE` state can only be turned on when the pod is in `ACCEL` or `COAST` state
- When you brake the pod, the pod automatically transitions to `STOP` state
- When you will request data from the pod, it will have the current state of the pod in the [Sensor Packet](/doc/S18-Software-Challenge-EOnK0BZy0ZdCuYIgCCCEb#:uid=361049301459783597291263&h2=Sensor-Packet) you will receive.

**Error States**
Error states are encoded in a 8 bit unsigned integer which you receive in every [Sensor Packet](/doc/S18-Software-Challenge-EOnK0BZy0ZdCuYIgCCCEb#:uid=361049301459783597291263&h2=Sensor-Packet). Each bit will either be 0 or 1 depending on whether the sensor/status associated is in an error state or working state. **0 means working state and 1 means error state**. Error States are as follows:

| **Bit Position**    | **Error State**       |
| ------------------- | --------------------- |
| First Bit           | Pod State ID          |
| Second Bit          | Acceleration Sensor 1 |
| Third Bit           | Acceleration Sensor 2 |
| Fourth Bit          | Acceleration Sensor 3 |
| Fifth Bit           | Temperature Sensor 1  |
| Sixth Bit           | Temperature Sensor 2  |
| Seventh Bit         | Temperature Sensor 3  |
| Last Bit            | Temperature Sensor 4  |

# Communication/Backend Manual

Communication and the Backend team has a big responsibility of connecting low level code with a more visual controllable dashboard. You obviously have to control the pod yourself and hence you will have a controls dashboard where you can do that. What your job is to make sure that dashboard can connect properly with the low level embedded code. You will be writing a server code on your laptop and arduino will be plugged into your machine. You will receive binary packets from arduino and you will have to parse these packets and make them readable and transferable and manageable (JSON, XML, etc). Then these packets will be sent to the dashboard so that they can show that data on the screen. **Your tasks are as follows:**

- Set up bi-directional communication with controls dashboard
  - Choose a suitable data format that you can use
- Create a log stash to dump data you receive from both Arduino and Dashboard
  - Do not shore binary data but rather the parsed data
- Connect with PlanetX Tube Server and send 20 packets per second. [Details are below](/doc/S18-Software-Challenge-EOnK0BZy0ZdCuYIgCCCEb#:uid=768755499247695124184763&h2=PlanetX-Server-Communication):


## PlanetX Tube Communication

This packet will be used to inform the PlanetX Tube server your position and speed in the pod. This is important because the tube wants to know if you can safely brake and other hazards in the tube. Packet structure is as follows:


    {
      "time": "",
      "id": "",
      "velocity": ""
      "distance": ""
    }

This packet must be sent at minimum 20 packets per second and if these packets not received, an emergency alert will be in place. So make sure you send at minimum 20 packets per second.

# Controls Dashboard 
