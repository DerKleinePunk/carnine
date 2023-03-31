#Backend

## Commandline

add --v=9 to commandline to see all log Verbose level 9 (Performens is slower)
--c <FileName> the Config file Name this ins an must Parameter

## Messages

message Type [see](../src/common/messages/BackendMessages.hpp) backend_message_type

{ "version":"0.1", "type":2, "state":false }
{ "version":"0.1", "type":2, "state":true }

## Config als Systemd Service

[see](../tools/systemd/infos.md)