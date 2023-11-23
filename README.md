# IoT-miniproject-1

# Submit iotlab-experiment

Submit iotlab-experiment with following command.

Fill the <site>
```bash
iotlab-experiment submit -n "IoT-Miniproject-1" -d 60 -l 2,archi=m3:at86rf231+site=<site>
```

Wait that the experiment is on runnig state
```bash
iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

Check which nodes you got with following command
```bash
iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
```

# Set up border router

Run start_border_router.sh after booking the experiment
```bash
sh start_border_router.sh <board-id> <site>
```


Check which tap interfaces are open
```bash
ip addr show | grep tap
```

Check which ipv6 prefixes are already used
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