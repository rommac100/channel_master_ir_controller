//#define ARDUINO_MEGA 1
//#define ATTINY85 1

#include <IRremote.h>

#define SERIAL_TYPE SERIAL
#define IR_PIN 6

int16_t current_azimuth;

#define IR_OUT_PIN 1
#define IR_IN_PIN 2

#define NORTH_DEG 0
#define SOUTH_DEG 180
#define EAST_DEG 270
#define WEST_DEG 360

#define DELAY_PER_DEG_MS 500 // takes about 6 seconds per degree

String buffer_read;
String az_string;
int16_t az_value;

bool AZser = false;
bool ELser = false;

#define A_CODE 0xAC
#define TYPE 'NEC'
#define CODE_POWER 0x1C
#define CODE_UP_AZ_C 0x10
#define CODE_DOWN_AZ_C 0x11

//Code break down.
//
//NEC, A, C
// Number codes are entered from left to right. and are 1 to 1
// aka: '1' = 0x01, '0' = 0x00, etc up to 9.
//
//

uint8_t update_rotor_position()
{
  if (az_value != -1)
  {

    int ii =0;
    /*
    if (current_azimuth == -1)
    {
       // go to North first just as precaution.
       Serial.println("DEBUG: sending north command");

        for (ii=0; ii< 3; ii++)
        {
          IrSender.sendNEC(0xAC,0x00,0);
          delay(350);  
          Serial.println("DEBUG: Sending 0x00 ");
        }

        //TODO ADD BACK IN DELAY FOR GOING TO 0
        //delay(DELAY_PER_DEG*360); // assume worse case scenario and delay 360 degrees worth of rotation.
        current_azimuth = 0;

    }
    */

    az_string = String(az_value);
    int temp_indx_value =0;

    if (az_string.length() != 3)
    {
      for (ii=0; ii< 3-az_string.length(); ii++)
      {
        IrSender.sendNEC(0xAC,0x00,0);
        delay(250);  
        //Serial.println("DEBUG: Sending 0x00 ");
      }
    } 

    for (ii=0; ii < az_string.length(); ii++)
    {
      temp_indx_value = az_string.charAt(ii) - '0';
      /*
      Serial.print("DEBUG: Sending char ");
      Serial.println(ii);
      Serial.println(temp_indx_value);
      */
      IrSender.sendNEC(0xAC,(temp_indx_value&0xFF),0);
      delay(250);  
    }

    unsigned long delay_time = (unsigned long) (abs(current_azimuth-az_value))*300;
    Serial.print("delaying by sec: ");
    Serial.println(delay_time);
    delay(delay_time);
    if (Serial.available())
    {
    Serial.readString();
    }
    current_azimuth = az_value;
    az_value = -1;
  }
  else
    return 1;
    

}

uint8_t parse_buffer()
{
  az_string ="";
  az_value = -1; // -1 invalid
  while(Serial.available())
  {
    buffer_read = Serial.readString();
    //Serial.print("read string: ");
    //Serial.println(buffer_read);
    for (int i=0; i < buffer_read.length(); i++)
    {
      if (buffer_read == "AZ EL \n")
      {
        Serial.print("AZ");
        Serial.print(current_azimuth); 
        Serial.println(" EL0.0");
        break;
      }
      else if ((buffer_read.charAt(i) == 'A') && (buffer_read.charAt(i+1) =='Z'))
      {
        Serial.println(buffer_read);
        for (int ii=i+2; ii<buffer_read.length(); ii++)
        {
          if (isDigit(buffer_read.charAt(ii)))
          {
            az_string = az_string + buffer_read.charAt(ii);
          }
          else
          {
            break;
          }
        }
      }
    }

    if (az_string != "")
    {
      az_value = (az_string.toInt()%360);

      return 0;
    }
    else
      return 1;
  }
}


void setup() {
    Serial.begin(9600);
    current_azimuth = -1; // set it -1 so it has to be reset from north.
    //Serial.println("DEBUG: ROTOR CONTROLLER");

    IrSender.begin(IR_PIN);

    az_value = -1;
}

void loop() {
  if (Serial.available())
  {
    parse_buffer();   
  }

  update_rotor_position();
}
