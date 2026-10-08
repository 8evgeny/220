sudo ip link set eno1 mtu 9000
sudo ifconfig eno1 169.254.0.1 netmask 255.255.0.0
sudo setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' Release/track_from_lib

