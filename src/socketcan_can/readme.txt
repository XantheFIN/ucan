Command cheat-sheet:

See if CAN adapter has been loaded: 
dmesg | fgrep -i can

Configure baudrate: '
sudo ip link set can0 type can bitrate 500000

Enable auto bus-OFF recovery after delay: 
sudo ip link set can0 type can bitrate 250000 restart-ms 100

Start interface: 
sudo ip link set up can0

Show interface details:
ip -details link show can0
