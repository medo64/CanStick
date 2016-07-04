### CanStick ###

CanStick is a small USB device converting data from CAN bus to serial port.


#### Protocol ####

This device is compatible with SLCAN protocol and most of its communication will
follow the same.


##### Channel commands #####

###### Set speed (S) ######

This command will set the speed.

|                 | Send            | Receive         | Notes                                                      |
|-----------------|-----------------|-----------------|------------------------------------------------------------|
| Syntax          | S{1:index}`CR`  | `CR` -or- `BEL` |                                                            |
| Query           | ?S`CR`          | S{1:index}`CR`  | If predefined value is not matched, `BEL` is returned (a!) |
| Example         | S4`CR`          | `CR`            | Speed is set to 125Kbit/s                                  |
| Example (error) | Sx`CR`          | `BEL`           | Invalid speed (p!)                                         |
| Example (error) | S4`CR`          | `BEL`           | Setting speed on open channel (a!)                         |

Following values are allowed (default is 125 Kbit/s):

| Index |          Speed |    BRP |    PRSEG |   SEG1PH |   SEG2PH |      SJW |     Range |
|------:|---------------:|-------:|---------:|---------:|---------:|---------:|----------:|
|   0   |    10 Kbit/s   |  199   |   5 TQ   |   4 TQ   |   2 TQ   |   1 TQ   |  8000 m   |
|   1   |    20 Kbit/s   |   99   |   5 TQ   |   4 TQ   |   2 TQ   |   1 TQ   |  4000 m   |
|   2   |    50 Kbit/s   |   39   |   5 TQ   |   4 TQ   |   2 TQ   |   1 TQ   |  1000 m   |
|   3   |   100 Kbit/s   |   19   |   5 TQ   |   4 TQ   |   2 TQ   |   1 TQ   |   700 m   |
| **4** | **125 Kbit/s** | **15** | **5 TQ** | **4 TQ** | **2 TQ** | **1 TQ** | **600 m** |
|   5   |   250 Kbit/s   |    7   |   5 TQ   |   4 TQ   |   2 TQ   |   1 TQ   |   200 m   |
|   6   |   500 Kbit/s   |    3   |   5 TQ   |   4 TQ   |   2 TQ   |   1 TQ   |   100 m   |
|   7   |   800 Kbit/s   |    2   |   4 TQ   |   3 TQ   |   2 TQ   |   1 TQ   |    50 m   |
|   8   |  1000 Kbit/s   |    1   |   6 TQ   |   3 TQ   |   2 TQ   |   1 TQ   |    50 m   |


###### Open channel (O) ######

Opening the channel so that CAN bus data can be sent and received.

|                 | Send    | Receive         | Notes                                     |
|-----------------|---------|-----------------|-------------------------------------------|
| Syntax          | O`CR`   | `CR` -or- `BEL` |                                           |
| Query           | ?O`CR`  | O{1:state}`CR`  | Returns if channel is open (1) or not (0) |
| Example         | O`CR`   | `CR`            | Open channel                              |
| Example (error) | O`CR`   | `BEL`           | Channel is already open (a!)              |


###### Open channel in listen-only mode (L) ######

Opening the channel so that CAN bus data can be only received.

|                 | Send    | Receive         | Notes                                                    |
|-----------------|---------|-----------------|----------------------------------------------------------|
| Syntax          | L`CR`   | `CR` -or- `BEL` |                                                          |
| Query           | ?L`CR`  | L{1:state}`CR`  | Returns if channel is open as listen-only (1) or not (0) |
| Example         | L`CR`   | `CR`            | Open channel                                             |
| Example (error) | L`CR`   | `BEL`           | Channel is already open (a!)                             |


###### Open channel in loopback mode (l) ######

Opening the channel in loopback mode.

|                 | Send    | Receive         | Notes                                                 |
|-----------------|---------|-----------------|-------------------------------------------------------|
| Syntax          | l`CR`   | `CR` -or- `BEL` |                                                       |
| Query           | ?l`CR`  | l{1:state}`CR`  | Returns if channel is open in loopback (1) or not (0) |
| Example         | l`CR`   | `CR`            | Open channel                                          |
| Example (error) | l`CR`   | `BEL`           | Channel is already open (a!)                          |


###### Close channel (C) ######

Close the CAN bus.

|                 | Send    | Receive         | Notes                                       |
|-----------------|---------|-----------------|---------------------------------------------|
| Syntax          | C`CR`   | `CR` -or- `BEL` |                                             |
| Query           | ?C`CR`  | C{1:state}`CR`  | Returns if channel is closed (1) or not (0) |
| Example         | C`CR`   | `CR`            | Close channel                               |
| Example (error) | C`CR`   | `BEL`           | Channel is already closed (a!)              |


##### Data commands #####

###### Transmit standard frame (t) ######

Transmits standard frame with 11-bit ID.

|                 | Send                            | Receive         | Notes                                                         |
|-----------------|---------------------------------|-----------------|---------------------------------------------------------------|
| Syntax          | t{3:id}{1:length}{0-8:data}`CR` | `CR` -or- `BEL` |                                                               |
| Example         | t12324142`CR`                   | `CR`            | Transmits standard frame (ID=0x123 Length=2 Data=0x4142)      |
| Example (error) | t`CR`                           | `BEL`           | No ID (p!)                                                    |
| Example (error) | t1232`CR`                       | `BEL`           | No data (p!)                                                  |
| Example (error) | t123241429`CR`                  | `BEL`           | Extra data (p!)                                               |
| Example (error) | t123941429`CR`                  | `BEL`           | Invalid length (p!)                                           |
| Example         | t12324142`CR`                   | `BEL`           | Channel not open (a!)                                         |
| Example         | t12324142`CR`                   | `BEL`           | Cannot transmit in read-only (e!)                             |


###### Transmit extended frame (T) ######

Transmits standard frame with 29-bit ID.

|                 | Send                            | Receive         | Notes                                                         |
|-----------------|---------------------------------|-----------------|---------------------------------------------------------------|
| Syntax          | T{8:id}{1:length}{0-8:data}`CR` | `CR` -or- `BEL` |                                                               |
| Example         | T1234567824142`CR`              | `CR`            | Transmits standard frame (ID=0x12345678 Length=2 Data=0x4142) |
| Example (error) | T`CR`                           | `BEL`           | No ID (p!)                                                    |
| Example (error) | T123456782`CR`                  | `BEL`           | No data (p!)                                                  |
| Example (error) | T12345678241429`CR`             | `BEL`           | Extra data (p!)                                               |
| Example (error) | T12345678941429`CR`             | `BEL`           | Invalid length (p!)                                           |
| Example         | T1234567824142`CR`              | `BEL`           | Channel not open (a!)                                         |
| Example         | T1234567824142`CR`              | `BEL`           | Cannot transmit in read-only (e!)                             |


###### Transmit standard remote frame (r) ######

Transmits remote frame with 11-bit ID.

|                 | Send                            | Receive         | Notes                                                         |
|-----------------|---------------------------------|-----------------|---------------------------------------------------------------|
| Syntax          | r{3:id}{1:length}`CR`           | `CR` -or- `BEL` |                                                               |
| Example         | r1232`CR`                       | `CR`            | Transmits remote frame (ID=0x123 Length=2)                    |
| Example (error) | r`CR`                           | `BEL`           | No ID (p!)                                                    |
| Example (error) | r12324142`CR`                   | `BEL`           | Extra data (p!)                                               |
| Example (error) | r1239`CR`                       | `BEL`           | Invalid length (p!)                                           |
| Example         | r1232`CR`                       | `BEL`           | Channel not open (a!)                                         |
| Example         | r1232`CR`                       | `BEL`           | Cannot transmit in read-only (e!)                             |


###### Transmit extended remote frame (R) ######

Transmits remote frame with 29-bit ID.

|                 | Send                            | Receive         | Notes                                                         |
|-----------------|---------------------------------|-----------------|---------------------------------------------------------------|
| Syntax          | R{8:id}{1:length}`CR`           | `CR` -or- `BEL` |                                                               |
| Example         | R123456782`CR`                  | `CR`            | Transmits remote frame (ID=0x12345678 Length=2                |
| Example (error) | R`CR`                           | `BEL`           | No ID (p!)                                                    |
| Example (error) | R1234567824142`CR`              | `BEL`           | Extra data (p!)                                               |
| Example (error) | R123456789`CR`                  | `BEL`           | Invalid length (p!)                                           |
| Example         | R123456782`CR`                  | `BEL`           | Channel not open (a!)                                         |
| Example         | R123456782`CR`                  | `BEL`           | Cannot transmit in read-only (e!)                             |


###### Auto-polling (X) ######

Determines if automatic or manual pooling is to be used. Automatic pooling is default.

|                 | Send                            | Receive         | Notes                                                         |
|-----------------|---------------------------------|-----------------|---------------------------------------------------------------|
| Syntax          | X{1:state}`CR`                  | `CR` -or- `BEL` |                                                               |
| Query           | ?X`CR`                          | X{1:state}`CR`  | Returns if automatic polling is enabled (1) or not (0)        |
| Example         | X0`CR`                          | `CR`            | Disable automatic polling                                     |
| Example (error) | X`CR`                           | `BEL`           | No state (p!)                                                 |


###### Poll one (P) ######

Reports next message waiting in buffer.

|                 | Send                            | Receive         | Notes                                                         |
|-----------------|---------------------------------|-----------------|---------------------------------------------------------------|
| Syntax          | P`CR`                           | `CR` -or- `BEL` |                                                               |
| Example (error) | P0`CR`                          | `BEL`           | Extra data (p!)                                               |
| Example (error) | P`CR`                           | `BEL`           | Cannot poll in auto-polling mode (e!)                         |


###### Poll all (A) ######

Reports all messages waiting in buffer.

|                 | Send                            | Receive         | Notes                                                         |
|-----------------|---------------------------------|-----------------|---------------------------------------------------------------|
| Syntax          | A`CR`                           | `CR` -or- `BEL` |                                                               |
| Example (error) | A0`CR`                          | `BEL`           | Extra data (p!)                                               |
| Example (error) | A`CR`                           | `BEL`           | Cannot poll in auto-polling mode (e!)                         |


##### Status commands #####

###### Version (V) ######

Returns hardware and software version.

|                 | Send    | Receive                                  | Notes                                     |
|-----------------|---------|------------------------------------------|-------------------------------------------|
| Syntax          | V`CR`   | V{2:software}{2:hardware}`CR` -or- `BEL` |                                           |
| Example         | V`CR`   | V1010`CR`                                | Hardware=1.0  Software=1.0                |
| Example (error) | V0`CR`  | `BEL`                                    | Invalid parameters (p!)                   |

Second digit of hardware revisions shows board type:
  * 0: CanStick
  * 1: CanStick/jack
  * 2: CanStick/mini


###### Serial number (N) ######

Returns serial number of device. Not used.

|                 | Send    | Receive                    | Notes                                     |
|-----------------|---------|----------------------------|-------------------------------------------|
| Syntax          | N`CR`   | N{4:number}`CR` -or- `BEL` |                                           |
| Example         | N`CR`   | N0000`CR`                  | Always returns 0x0000                     |
| Example (error) | N0`CR`  | `BEL`                      | Invalid parameters (p!)                   |


###### Flags (F) ######

Returns current error flags.

|                 | Send    | Receive                   | Notes                                     |
|-----------------|---------|---------------------------|-------------------------------------------|
| Syntax          | F`CR`   | F{2:flags}`CR` -or- `BEL` |                                           |
| Example         | F`CR`   | F00`CR`                   | No errors                                 |
| Example (error) | F0`CR`  | `BEL`                     | Invalid parameters (p!)                   |

Following are bits and their meanings:
  * 0: Rx queue full
  * 1: Tx queue full
  * 2: Tx/Rx warning
  * 3: Rx overflow
  * 4: -
  * 5: Error-passive
  * 6: Arbitration lost
  * 7: Bus error


##### Extra commands #####

In addition SLCAN command, there are additional commands available.


###### Debug mode (*D) ######

Turns on/off various features that will help with debugging:

|                 | Send            | Receive         | Notes                                                  |
|-----------------|-----------------|-----------------|--------------------------------------------------------|
| Syntax          | *D{2:state}`CR` | `CR`            | Configures debugging features                          |
| Query           | ?*D`CR`         | *D{2:state}`CR` | Returns the current state                              |
| Example         | *D`CR`          | `CR``LF`        | Turns on all debugging features (bits 1-8)             |
| Example         | *D00`CR`        | `CR``LF`        | Turns debugging features off                           |
| Example         | *D07`CR`        | `CR``LF`        | Turns on all debugging features except echo            |
| Example (error) | *DFF`CR`        | `BEL`           | Unsupported flags (x!)                                 |
| Example (error) | *D8`CR`         | `BEL`           | Invalid value (p!)                                     |
| Example (error) | *D888`CR`       | `BEL`           | Invalid value (p!)                                     |

Following are bits and their meanings:
  * 0: Extra LF
  * 1: Extended errors
  * 2: Cansend format
  * 3: Not used
  * 4: Not used
  * 5: Not used
  * 6: Not used
  * 7: Echo

  
####### Extra LF #######

Send LF after each command is executed (in addition to CR or BEL).


####### Extended error mode #######

Turn on the extended error mode. Every `BEL` will be preceeded by an additional
character describing error in more details, and an exclamation point (!).

|                 | Send    | Receive         | Notes                                       |
|-----------------|---------|-----------------|---------------------------------------------|
| Syntax          | *E`CR`  | `CR`            | Turns extended error mode on                |
| Syntax          | *E_`CR` | `CR`            | Turns extended error mode on (1) or off (0) |
| Query           | ?*E`CR` | *E{1:state}`CR` | Returns the current state                   |
| Example         | *E0`CR` | `CR`            | Turns extended error mode off               |
| Example (error) | *E2`CR` | `BEL`           | Invalid value (p!)                          |

Following extended errors are currently returned:

| Response | Reason                                                            |
|----------|-------------------------------------------------------------------|
| a!`BEL`  | Invalid access (e.g. writing in listen-only mode)                 |
| e!`BEL`  | Generic error (e.g. cannot transmit at this time)                 |
| p!`BEL`  | Value given to parameter is not valid (e.g. unknown speed)        |
| x!`BEL`  | Command not supported                                             |


####### Cansend format #######

Controls if received data is formatted as cansend or SLCAN.


####### Echo #######

Controls echoing of received characters.


###### Terminate CAN bus (*T) ######

Turns on termination resistors on CAN bus. Works only on CanStick/mini.

|                 | Send    | Receive         | Notes                               |
|-----------------|---------|-----------------|-------------------------------------|
| Syntax          | *T`CR`  | `CR`            | Turns termination on                |
| Syntax          | *T_`CR` | `CR`            | Turns termination on (1) or off (0) |
| Query           | ?*T`CR` | *T{1:state}`CR` | Returns the current state           |
| Example         | *T0`CR` | `CR`            | Turns termination off               |
| Example (error) | *T2`CR` | `BEL`           | Invalid value (p!)                  |
| Example (error) | *T1`CR` | `BEL`           | Works only on CanStick/mini (x!)    |

Following values are allowed:
  * 0: off
  * 1: on


###### Power CAN bus (*P) ######

Powers CAN bus from device itself. Works only on CanStick/mini.

|                 | Send    | Receive         | Notes                            |
|-----------------|---------|-----------------|----------------------------------|
| Syntax          | *P`CR`  | `CR`            | Turns V+ on                      |
| Syntax          | *P_`CR` | `CR`            | Turns V+ on (1) or off (0)       |
| Query           | ?*P`CR` | *P{1:state}`CR` | Returns the current state        |
| Example         | *P0`CR` | `CR`            | Turns V+ off                     |
| Example (error) | *P2`CR` | `BEL`           | Invalid value (p!)               |
| Example (error) | *P1`CR` | `BEL`           | Works only on CanStick/mini (x!) |

Following values are allowed:
  * 0: off
  * 1: on


###### Reset device (*R) ######

Resets the device.

|                 | Send    | Receive         | Notes                            |
|-----------------|---------|-----------------|----------------------------------|
| Syntax          | *R`CR`  |                 | Reset                            |
| Example (error) | *R1`CR` | `BEL`           | Invalid parameters (p!)          |


##### Data format #####

SLCAN format:

| Type                   | Format                          | Example            | Notes                              |
|------------------------|---------------------------------|--------------------|------------------------------------|
| Standard frame         | t{3:id}{1:length}{0-8:data}`CR` | t12324142`CR`      | ID=0x123 Length=2 Data=0x4142      |
| Extended frame         | T{8:id}{1:length}{0-8:data}`CR` | T1234567824142`CR` | ID=0x12345678 Length=2 Data=0x4142 |
| Standard remote frame  | r{3:id}{1:length}`CR`           | r1230`CR`          | ID=0x123 Length=0                  |
| Extended remote frame  | R{8:id}{1:length}`CR`           | R123456782`CR`     | ID=0x12345678 Length=2             |

Cansend format:

| Type                   | Format                          | Example            | Notes                              |
|------------------------|---------------------------------|--------------------|------------------------------------|
| Standard frame         | {3:id}#{0-8:data}`CR`           | 123#4142`CR`       | ID=0x123 Length=2 Data=0x4142      |
| Extended frame         | {8:id}#{0-8:data}`CR`           | 12345678#4142`CR`  | ID=0x12345678 Length=2 Data=0x4142 |
| Standard remote frame  | {3:id}#R{0-1:length}`CR`        | 123#R`CR`          | ID=0x123 Length=0                  |
| Extended remote frame  | {8:id}#R{0-1:length}`CR`        | 12345678#R2`CR`    | ID=0x12345678 Length=2             |

---

*You can check my blog and other projects at [www.medo64.com](https://www.medo64.com/)*
