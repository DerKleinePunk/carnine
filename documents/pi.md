### Netteil Dokumentation
dtoverlay gpio-poweroff,active_low=1,gpiopin=19 ->  in die /boot/config.txt eintragen ->
Mit Dig3 verbinden damit der Halt zustand erkannt werden kann. Sonst gilt ein Timeout in AVR Software
Natürlich kann man jeden Anderen GPIO auch verwenden. Nur auf dem Netzteil ist fix es sei den man
Ändert den AVR Code.
https://github.com/raspberrypi/firmware/blob/master/boot/overlays/README Dokumentation für den PI.
