### CanStick ###

CanStick is a small USB device converting data from CAN bus to serial port.


#### Protocol ####


##### Message #####

| Description                             | Bytes                                                                       |
|-----------------------------------------|-----------------------------------------------------------------------------|
| Basic message                           | `ID (1-8 hex)` `:` `Data (0-16 hex)`                                        |
| Message with flags                      | `ID (1-8 hex)` `.` `Flags (1 or 2)` `:` `Data (0-16 hex)`                   |


##### Options #####

| Description                              | Bytes                                   |
|------------------------------------------|-----------------------------------------|
| Retrieve next message until buffer empty | `N`                                     |
| Retrieve next message                    | `n`                                     |
| Turn echo on                             | `O`                                     |
| Turn echo off                            | `o`                                     |
| Turn on termination                      | `P`                                     |
| Turn off termination                     | `p`                                     |
| Turn on auto-report                      | `R`                                     |
| Turn off auto-report                     | `r`                                     |
| Turn on output voltage                   | `W`                                     |
| Turn off output voltage                  | `w`                                     |
| Current state                            | `?`                                     |



---

*You can check my blog and other projects at [www.jmedved.com](http://www.jmedved.com/).*
