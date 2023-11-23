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

### Building the project

With SSH connection to grenoble.iot-lab.info

1. Clone the repository to your folder of choise:

    ```bash
    git clone https://github.com/matluuk/IoT-miniproject-1.git
    ```

2. Change into the working directory:

    ```bash
    cd IoT-miniproject-1/app
    ```

3. Set source:

    ```bash
    source /opt/riot.source
    ```

4. Build the project:

    ```bash
    make
    ```


### Additional Configuration

Set up Fit IoT-Lab experiment with two nodes for device firmware and border router:

1. Submit an experiment at grenoble for 20 minutes:

    ```bash
    iotlab-experiment submit -n "IoT-miniproject-1" -d 20 -l 2,archi=m3:at86rf231+site=grenoble
    ```

2. Wait for the experiment to be in the Running state:

    ```bash
    iotlab-experiment wait --timeout 30 --cancel-on-timeout
    ```

3. Get the experiment nodes list:

    ```bash
    iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
    ```

## Usage

To use this project, follow these guidelines:

1. Flash the firmware into the experiment node:

    ```bash
    make IOTLAB_NODE=auto flash
    ```

Free up resources:

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
│   ├── Makefile
│   
├── RIOT
│   └── ...
├── README.md
└── ...
```

## Authors

- **Hermanni Hanhela** - [GitHub Profile](https://github.com/HHanhela)
- **Matti Luukkonen** - [GitHub Profile](https://github.com/matluuk)
- **Touko Kinnunen** - [GitHub Profile](https://github.com/toukokinnunen)
