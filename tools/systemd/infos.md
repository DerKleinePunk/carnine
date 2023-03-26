/etc/systemd/system/
sudo cp carnine.service /etc/systemd/system
sudo cp carnine.socket /etc/systemd/system

sudo systemctl daemon-reload
sudo systemctl enable carnine.service
sudo systemctl start carnine.socket

sudo systemctl status carnine.service

sudo systemctl disable carnine.service
sudo systemctl disable carnine.socket
rm /etc/systemd/system/carnine.service
rm /etc/systemd/system/carnine.socket
rm /usr/lib/systemd/system/carnine.service
sudo systemctl daemon-reload