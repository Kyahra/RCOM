#include "data_link_layer.h"


int init_link_layer(int timeout,int numTransmissions, int baudRate){

  link_layer.timeoout = timeout;
  link_layer.numTransmissions = numTransmissions;
  link_layer.baudRate = baudRate;

}
