# IoT-miniproject-1

## Overview

Briefly describe the purpose and goals of your IoT mini project.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Code Structure](#code-structure)
- [Authors](#authors)

## Features

- **Real-time Sensor Data:** Monitor and display real-time sensor data from the LPSXXX sensor.
- **Energy-Efficient Reading:** Implement an energy-efficient reading thread for prolonged device battery life.
- ...

## Getting Started

### Prerequisites

Before you begin, ensure you have the following:

1. Access to Fit IoT-Lab account.

2. SSH connection to grenoble.iot-lab.info

3. Access to virtual machine running some linux distro.

### Build the project

1. Connect to grenoble.iot-lab.info:

    ```batch
    ssh <username>@grenoble.iot-lab.info
    ```

2. Clone the repository to your folder of choise at grenoble.iot-lab.info:

    ```bash
    git clone https://github.com/matluuk/IoT-miniproject-1.git
    ```

3. Change into the miniproject-1 repository working directory:

    ```bash
    cd IoT-miniproject-1/app
    ```

4. Set source:

    ```bash
    source /opt/riot.source
    ```

5. Build the project:

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
    # Command returns the ID of both nodes, they are needed later!
    iotlab-experiment submit -n "IoT-miniproject-1" -d 20 -l 2,archi=m3:at86rf231+site=grenoble
    ```

3. Wait for the experiment to be in the Running state:

    ```bash
    iotlab-experiment wait --timeout 30 --cancel-on-timeout
    ```

4. Get the experiment nodes list:

    ```bash
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
    # <board-id> = Select one ID from section "Set up Fit IoT-Lab experiment" point 2.
    sh flash_border_router.sh <board-id>
    ```

4. Check which tap interfaces are open:

    ```bash
    ip addr show | grep tap
    ```

5. Check which ipv6 prefixes are already used:

    ```bash
    ip -6 route
    ```

On the frontend SSH launch the ethos_uhcpd command with:

a free tap <num> network interface

a free <ipv6_prefix> on the good site. For example the first one of Grenoble site <ipv6_prefix>=2001:660:5307:3100

the good node's <id> for the border router

    ```bash
    sudo ethos_uhcpd.py m3-<id> tap<num> <ipv6_prefix>::/64
    ```

## Usage

To use this project, follow these guidelines:

1. Flash the firmware into the experiment node:

    ```bash
    make IOTLAB_NODE=auto flash
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
