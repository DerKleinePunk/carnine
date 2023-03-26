#include "BackendMessages.hpp"

std::ostream& operator<<(std::ostream& os, const backend_message_type c) {
	switch (c) {
          case backend_message_type::undefined: os << "undefined";    break;
          case backend_message_type::base: os << "base";    break;
          case backend_message_type::welcome: os << "welcome";    break;
          case backend_message_type::powerSupplyWatchdog: os << "powerSupplyWatchdog";    break;
          case backend_message_type::powerOff: os << "powerSupplyWatchdog";    break;
          default:  os << "backend_message_type not in list";
	}
	return os;
}