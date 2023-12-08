# IoT-miniproject-1

## Overview

### Introduction

**IoT-miniproject-1** is the first part of the project work assigned at the Internet of Things (521043S-3004) course at University of Oulu in 2023. Goal of the project is to create a firmware for a spesific microcontroller, enabling a secure and energy-efficent transmission of sensor data from microcontroller to a cloud-backend using IPv6 protocol. 

The project uses [Fit IoT-Lab](https://www.iot-lab.info/) remote testbed to book IoT-devices, to program them, interact with them and control them.



## Table of Contents

- [Getting Started](#getting-started)
- [Usage](#usage)
- [Code Structure](#code-structure)
- [Authors](#authors)


## Getting Started

### Prerequisites

Before you begin, ensure you have the following:

1. Access to Fit IoT-Lab account.

2. SSH connection to grenoble.iot-lab.info

3. Access to virtual machine running some linux distro to run CoAP server.

Requirements to run the CoAp server in the linux virtual machine

1. Create new python venv

    Follow guide at [Link](https://docs.python.org/3/library/venv.html)

2. intall autoconf on linux 

    ```bash
    sudo apt-get install autoconf
    ```

3. install python-dev for 

    ```bash
    sudo apt-get install python-dev-is-python3 -y
    ```

4. install build-essential

    ```bash
    sudo apt-get install build-essential
    ```

5. intall aiocoap to python venv

    ```bash
    pip3 install --upgrade "aiocoap[all]"
    ```

### Build the project

1. Connect to grenoble.iot-lab.info:

    ```batch
    ssh <username>@grenoble.iot-lab.info
    ```

2. Clone the repository to your folder of choise at grenoble.iot-lab.info:

    ```bash
    git clone https://github.com/matluuk/IoT-miniproject-1.git
    ```

3. Init the RIOT-submodule:

    ```bash
    git submodule init
    ```

4. Update the RIOT-submodule:

    ```bash
    git submodule update
    ```

5. Change into the miniproject-1 repository working directory:

    ```bash
    cd IoT-miniproject-1/app
    ```

6. Set source:

    ```bash
    source /opt/riot.source
    ```

7. Build the project:

    ```bash
    make
    ```


### Set up Fit IoT-Lab experiment

Set up Fit IoT-Lab experiment with two nodes for device firmware and border router:

1. Connect to grenoble.iot-lab.info IF not already:

    ```batch
    ssh <username>@grenoble.iot-lab.info
    ```

2. Submit an experiment at grenoble for 20 minutes:

    ```bash
    iotlab-experiment submit -n "IoT-miniproject-1" -d 20 -l 2,archi=m3:at86rf231+site=grenoble
    ```

3. Wait for the experiment to be in the Running state:

    ```bash
    iotlab-experiment wait --timeout 30 --cancel-on-timeout
    ```

4. Get the experiment nodes list:

    ```bash
    # Command returns the ID of both nodes, they are needed later!
    iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
    ```

### Set up border router

1. Connect to grenoble.iot-lab.info IF not already:

    ```batch
    ssh <username>@grenoble.iot-lab.info
    ```

2. Change into the miniproject-1 repository directory:

    ```bash
    cd IoT-miniproject-1
    ```

3. Run flash_border_router.sh after booking the experiment:

    ```bash
    sh flash_border_router.sh <board-id>
    ```

    ```bash
    # Example to get correct <board.id>

    user@grenoble.iot-lab.info:~$ iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
    [
    "m3-6.grenoble.iot-lab.info",
    "m3-7.grenoble.iot-lab.info"
    ]
    # In this example <board-id> = 6 or 7
    # Only need to pick one ID!
    ```

4. Check which tap interfaces are open:

    ```bash
    ip addr show | grep tap
    ```

5. Check which ipv6 prefixes are already used:

    ```bash
    ip -6 route
    ```

6. Launch the ethos_uhcpd command with:

    ```bash
    sudo ethos_uhcpd.py m3-<id> tap<num> <ipv6_prefix>::/64
    ```

    ```bash
    # Example to get correct <id>, <num> and <ipv6_prefix>
    
    # For <id>:
    # Use the same ID as you chose earlier with command: sh flash_border_router.sh <board-id>

    #For <num>:
    user@grenoble.iot-lab.info:~$ ip addr show | grep tap
    313: tap0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc pfifo_fast state DOWN group default qlen 1000
    ...
    316: tap1: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc pfifo_fast state DOWN group default qlen 1000
    ...
    582: tap8: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc pfifo_fast state DOWN group default qlen 1000
    ...
    # Example list of network interfaces already in use. This example has 3 network interfaces (eg. tap0, tap1 and tap8). 
    # Choose a free interface, for example <id> = tap8 + 1 = 9

    #For <ipv6_prefix>
    user@grenoble.iot-lab.info:~$ ip -6 route
    2001:660:5307:3100::/64 via fe80::2 dev tap0 metric 1024 linkdown  pref medium
    2001:660:5307:3102::/64 via fe80::2 dev tap2 metric 1024 linkdown  pref medium
    2001:660:5307:3103::/64 via fe80::2 dev tap3 metric 1024 linkdown  pref medium
    2001:660:5307:3104::/64 via fe80::2 dev tap4 metric 1024 linkdown  pref medium
    2001:660:5307:3107::/64 via fe80::2 dev tap100 metric 1024 linkdown  pref medium
    ...
    # Example list of ipv6 prefixes in use. 
    # Choose a free prefix, for example <ipv6_prefix> = 2001:660:5307:3108
    ```

7. After launching ethos_uhcpd, leave the script running!

## Set up server 

For coap server a linux virtual machine is needed. 

Requirements for the virtual machine:
- Ubuntu 20.04
- external IPV6 address for connecting with iot-lab nodes
- coap port 8683 open for ingress traffic
- ICMPv6 traffic allowed for testing ping

Any linux virtual machine should work, but we walk through how to set up Google Cloud virtual machine. Google cloud has a free trial with 300$ credits to use. The virtual machine needs a external IPV6 ip address for connection with the iot-lab node. 

### Create VPC (Virtual Private Cloud)

To get external ipv6 address, Virtual Private Cloud is needed.



### Create google cloud VM

Create Google Cloud account. You need a credit card for google cloud account in order to use the free tier.

https://console.cloud.google.com/?hl=en&_ga=2.87814171.-2055644655.1699615458&_gac=1.116033140.1702051941.CjwKCAiAmsurBhBvEiwA6e-WPIQOg4lsX1QJevny4vxo9FBotFtCxOCFgTHR5MXrhSOSkf66HEamdRoCthsQAvD_BwE

To create a google cloud VM go to 

https://console.cloud.google.com/compute/instances?_ga=2.181990823.1219151963.1702027579-2055644655.1699615458

Press Create new instance
![Alt text](images/1.png)

### Set up CoAP server

## Usage

To use this project, follow these guidelines:

1. Connect to grenoble.iot-lab.info IF not already:

    ```batch
    ssh <username>@grenoble.iot-lab.info
    ```

2. Change into the miniproject-1 repository working directory:

    ```bash
    cd IoT-miniproject-1/app
    ```

3. Flash the firmware into the experiment node:

    ```bash
    make IOTLAB_NODE=m3-114 DEFAULT_CHANNEL=18 DEFAULT_PAN_ID=0xbcb4 flash 
    ```

### Free up resources:

After finished with the experiment, stop your experiment to free up the experiment nodes at Fit IoT-Lab:

```bash
iotlab-experiment stop
```


## Code Structure

The project has the following code structure:

```plaintext
.
├── app
│   ├── main.c
│   └── Makefile
├── Coap-Server
│   ├── server.py
│   └── start_server.sh
├── RIOT
│   └── ...
├── flash_border_router.sh
└── README.md
```

## Authors

- **Hermanni Hanhela** - [GitHub Profile](https://github.com/HHanhela)
- **Matti Luukkonen** - [GitHub Profile](https://github.com/matluuk)
- **Touko Kinnunen** - [GitHub Profile](https://github.com/toukokinnunen)
