# Basic code for CAN communication with a minimal GUI in Qt5

This code was tested on Ubuntu 22.04.

### Build in C++

```
cd cpp
mkdir build && cd build
cmake ..
```

### Test using a virtual CAN

```
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set up can0
```
