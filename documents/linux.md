ps aux | grep CarNiNeBackend

kill xxx

## Links

[swap on disk](https://www.derpade.de/raspberry-pi-swap-auf-externen-usb-stick-auslagern/)
https://www.shellbefehle.de/befehle/chown/

https://beej.us/guide/bgipc/html/split/mq.html

https://www.unix.com/unix-for-advanced-and-expert-users/110170-epoll-sockets-threads.html

https://lloydrochester.com/post/unix/systemd_sockets/

https://gist.github.com/alexandruc/2350954

https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/

https://github.com/eliben/code-for-blog/blob/master/2017/async-socket-server/epoll-server.c

https://github.com/42ity/fty-common-socket/blob/master/src/fty_common_socket_basic_mailbox_server.cc

https://eklitzke.org/systemd-socket-activation

https://github.com/42ity/fty-common-socket/blob/master/src/fty_common_socket_basic_mailbox_server.cc

https://idea.popcount.org/2019-11-06-creating-sockets/

https://www.smnd.sk/anino/programming/c/unix_examples/poll.html

https://eklitzke.org/systemd-socket-activation

http://0pointer.de/blog/projects/socket-activation.html

## Search Words

sd_listen_fds_with_names
sd_listen_fds
ListenUSBFunction
sd_is_socket()

## Commandline Tests

nc -U /tmp/CarNiNe.sock

valgrind --tool=memcheck ~/develop/carnine/bin/Linux/CarNiNeBackend.bin --c=BackendConfig.json
valgrind --tool=memcheck --leak-check=full ~/develop/carnine/bin/Linux/CarNiNeBackend.bin --c=BackendConfig.json
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ~/develop/carnine/bin/Linux/CarNiNeBackend.bin --c=BackendConfig.json

## UDEV

https://github.com/Jinjinov/Usb.Events/blob/master/Usb.Events/UsbEventWatcher.Linux.c

https://github.com/nwoki/udevnotifier

https://github.com/wkennington/systemd/blob/dd34ef678f6e8124c50db3d855e098ac6e55d75d/src/login/logind.c