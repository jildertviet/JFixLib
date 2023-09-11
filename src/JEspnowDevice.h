#pragma once

class JEspnowDevice{
  public:
    uint8_t replyAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t myAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    String randomPw(){
      String r = "01234567";
      for(int i=0; i<8; i++){
        r[i] = random(255);
      }
      return r;
    }

    JEspnowDevice(){
      
    }

    void initEspnow(String networkName){
      WiFi.softAP(networkName.c_str(), randomPw().c_str(), CHANNEL, true); // This sets the channel... ?
      Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
      WiFi.softAPmacAddress(myAddr);
  
    }
};
