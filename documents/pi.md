#PI

## Sound

```bash
aplay /usr/share/sounds/alsa/Noise.wav
speaker-test -c2
```

If you install Lite on RPI4 muss install pulseaudio for get HDMI Sound get working (I found no other Way)

```bash
sudo apt-get install pulseaudio pulseaudio-utils
```

When Knacks try disable 

```bash
/etc/pulse/default.pa
#load-module module-suspend-on-idle
```

## Netteil Dokumentation

SerialPort serial0 -> Kommunikation Netzteil

dtoverlay gpio-poweroff,active_low=1,gpiopin=5 ->  in die /boot/config.txt eintragen ->
Mit Dig3 verbinden damit der Halt zustand erkannt werden kann. Sonst gilt ein Timeout in AVR Software
Natürlich kann man jeden Anderen GPIO auch verwenden. Nur auf dem Netzteil ist fix es sei den man
Ändert den AVR Code.
https://github.com/raspberrypi/firmware/blob/master/boot/overlays/README Dokumentation für den PI.


## HDD M2 SSD via USB3

dmesg

mkdir /mnt/hdd

sudo mount /dev/sda2 /mnt/hdd
sudo umount /mnt/hdd
sudo mkfs.ext4 /dev/sda2
sudo nano /etc/fstab
UUID=xxxxxxxx-xxxxxx-xxx-xxxx-xxxxxxxxx none swap sw 0 0
UUID=12b50f26-d442-4363-a317-5f3984c55b78   /mnt/hdd   ext4   auto,nofail,sync,users,rw,exec   0   0
/dev/sda1: UUID="390deafc-f255-4ecc-8ba8-bfd0cccf2b7c" TYPE="swap" PARTUUID="0aa63205-3ea9-1843-b1d7-9fcf6c06d531"
/dev/sda2: UUID="12b50f26-d442-4363-a317-5f3984c55b78" BLOCK_SIZE="4096" TYPE="ext4" PARTUUID="aa870308-b1d7-924c-a8c2-5054bbe25cdc"

dphys-swapfile swapoff
dphys-swapfile swapon

sudo chmod -R 777 /mnt/hdd
chown root:users /mnt/hdd
sudo chmod 775 /mnt/hdd

## Dependency

wget -N https://www.libsdl.org/release/SDL2-2.0.22.tar.gz
tar -xzf SDL2-2.0.22.tar.gz


###Links

https://www.derpade.de/raspberry-pi-swap-auf-externen-usb-stick-auslagern/
https://www.shellbefehle.de/befehle/chown/