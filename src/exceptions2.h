#pragma once

#include "exceptions.h"

namespace se_exceptions {

  // network exceptions
  class NetException: public SE_Exception {
  public:
      NetException(QString str): SE_Exception(str) {}
  };

  class NotIPException: public NetException {
  public:
      NotIPException(QString str): NetException(str) {}
  };

  class DNSException: public NetException {
  public:
      DNSException(QString str): NetException(str) {}
  };

  class SendTimeoutException: public NetException {
  public:
      SendTimeoutException(QString str): NetException(str) {}
  };

}
