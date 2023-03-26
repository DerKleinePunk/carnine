#include "WorkerMessage.hpp"

std::ostream& operator<<(std::ostream& os, const worker_message_type c) {
	switch (c) {
          case worker_message_type::die: os << "die";    break;
          case worker_message_type::json: os << "json";    break;
          default:  os << "worker_message_type not in list";
	}
	return os;
}

WorkerMessage::WorkerMessage()
{
}

WorkerMessage::~WorkerMessage()
{
}
