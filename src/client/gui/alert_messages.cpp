#include "alert_messages.h"

#include "exceptions.h"

using namespace se_exceptions;

QString getAlertTitle(Alert alert) {
 switch(alert) {
   case Alert::INFO:
     return "Info";
   case Alert::WARNING:
     return "Warning";
   case Alert::ERROR:
     return "Error";
   default:
     throw GuiException{"WTF"};
 }
}
