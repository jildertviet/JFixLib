#include "../../../src/JFixLib.h"
// #include <WiFi.h>

char version[2] = {1, 1};
JJonisk jonisk;

void setup(){
  jonisk.setup("JV_");
}
void loop(){
  jonisk.update();
}
