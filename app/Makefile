# name of your application
APPLICATION = sensonrs-test

# If no BOARD is found in the environment, use this default:
BOARD ?= iotlab-m3

FEATURES_REQUIRED += periph_rtc

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

# Modules to include:
USEMODULE += ztimer_msec
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += lps331ap
#USEMODULE += lsm303dlhc
USEMODULE += ps
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include