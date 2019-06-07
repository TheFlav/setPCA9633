#!/usr/bin/env bash

declare TEMP
FAN_ON=false
SLEEP_DELAY=15
while true; do
	TEMP=$(vcgencmd measure_temp | cut -d= -f2 | cut -d. -f1)
	if [[ "$TEMP" -lt 40 && "$FAN_ON" = true   ]]; then
		/home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -m OFF > /dev/null
		FAN_ON=false
	elif [[ "$TEMP" -ge 45 && "$TEMP" -le 60  ]]; then
		/home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -m PWM -q 0x55 > /dev/null
		FAN_ON=true
	elif [[ "$TEMP" -ge 60 && "$TEMP" -le 70  ]]; then
		/home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -m PWM -q 0xAA > /dev/null                                    FAN_ON=true
	else
		/home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -m PWM -q 0xFF > /dev/null
		FAN_ON=true
	fi
	sleep $SLEEP_DELAY
done
