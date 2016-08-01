FrontEndAMBLib: low-level CAN and Socket communications.<br>
Based on the ALMA CONTROL software but adapted to use NI-CAN on Windows.

The intent of this library is to make all higher-level code relatively easy to port from Windows to ALMA CONTROL on RTAI/Linux.

To implement a new communication bus, derive a class from CANBusInterface and fill in the required virtual functions with the particulars of your bus hardware.

