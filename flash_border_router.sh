

CHANNEL=18
PAN_ID=0xbcb4

BR_ID = $1
Site = $2
APP_DIR = "../../RIOT/examples/gnrc_border_router"
make -C $APP_DIR ETHOS_BAUDRATE=500000 BOARD=iotlab-m3 DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID IOTLAB_NODE=m3-$BR_ID.$SITE.iot-lab.info flash