//Lívia Lutz dos Santos - 2211055
// Ana Luiza Pinto Marques - 2211960

#include <TimerOne.h> 
#include <MultiFuncShield.h>
#define RANGE_MAX_NUMBER 9999
#define RANGE_MIN_NUMBER 0000

enum GeneratorModeValues{
GENERATOR_STOPPED,
GENERATOR_STARTED,
SETTING_RANGE_MAX_NUM_STARTED,
SETTING_RANGE_MIN_NUM_STARTED
};

enum RangeStatusValues{
RANGE_NOT_SET,
RANGE_SET
};

enum ledModeValues{
LED_ALL_OFF,
LED_1_ON,
LED_2_ON,
LED_3_ON,
LED_4_ON
};

enum AnimationModeValues{
ANIMATION_STOPPED,
ANIMATION_STARTED,
ANIMATION_STAGE1,
ANIMATION_STAGE2,
ANIMATION_STAGE3,
ANIMATION_STAGE4,
ANIMATION_INTERRUPTED
};


byte ledModeValue = LED_ALL_OFF;
byte GeneratorModeValue = GENERATOR_STOPPED;
byte RangeStatusValueMin = RANGE_NOT_SET;
byte RangeStatusValueMax = RANGE_NOT_SET;
byte AnimationModeValue = ANIMATION_STOPPED;
int u,d,c,m,numero,maxi = RANGE_MAX_NUMBER,mini = RANGE_MIN_NUMBER;
char counter_string[5] = "0000";
int counter = 0;
int count_intr = 0;

void setup() {
 
  Timer1.initialize(); 
  MFS.initialize(&Timer1);
  MFS.write("OFF"); 
  MFS.writeLeds(LED_ALL, OFF);
  Serial.begin(9600);
}

void loop() {
  Serial.write(GeneratorModeValue + '0');
 
  byte btn = MFS.getButton();

  switch(GeneratorModeValue) {
    case GENERATOR_STOPPED:
      counter = 0;
      ledModeValue = LED_ALL_OFF;
      MFS.blinkDisplay(DIGIT_ALL,OFF);
      MFS.write("OFF");

      switch(btn) {
        case BUTTON_1_SHORT_RELEASE:
          if(RangeStatusValueMin == RANGE_SET && RangeStatusValueMax == RANGE_SET) {
            GeneratorModeValue = GENERATOR_STARTED;
            AnimationModeValue = ANIMATION_STARTED;
            MFS.beep(5);
            numero = random(RANGE_MIN_NUMBER, RANGE_MAX_NUMBER);
            Serial.println(numero);
            u = numero % 10;
            d = (numero / 10) % 10;
            c = (numero / 100) % 10;
            m = (numero / 1000) % 10;
          }
          break;
        case BUTTON_1_LONG_PRESSED:
          break;
        case BUTTON_2_SHORT_RELEASE:
          MFS.write(RANGE_MAX_NUMBER);
          break;
        case BUTTON_2_LONG_PRESSED:
          GeneratorModeValue = SETTING_RANGE_MAX_NUM_STARTED;
          break;
        case BUTTON_3_SHORT_RELEASE:
          MFS.write(RANGE_MIN_NUMBER);
          break;
        case BUTTON_3_LONG_PRESSED:
          GeneratorModeValue = SETTING_RANGE_MIN_NUM_STARTED;
          break;
      }
      break; 

    case GENERATOR_STARTED:
      if(btn == BUTTON_1_SHORT_RELEASE) {
        AnimationModeValue = ANIMATION_INTERRUPTED;
        
      }

      switch(AnimationModeValue) {
        case ANIMATION_INTERRUPTED:
          MFS.write("Intr");
          ledModeValue = LED_ALL_OFF;
          MFS.writeLeds(LED_ALL, OFF); 
          MFS.beep(50, 50, 3, 1, 50); 
          RangeStatusValueMin = RANGE_NOT_SET;
          RangeStatusValueMax = RANGE_NOT_SET; 
          GeneratorModeValue = GENERATOR_STOPPED; 
          break;
        case ANIMATION_STARTED:
          counter_string[0] = counter + '0';
          counter_string[1] = counter + '0';
          counter_string[2] = counter + '0';
          counter_string[3] = counter + '0';
          MFS.write(counter_string);
          counter++;
          if(counter == 10){
            counter = 0;
            AnimationModeValue = ANIMATION_STAGE1;
          }
          break;
        case ANIMATION_STAGE1:
          counter_string[0] = counter + '0';
          counter_string[1] = counter + '0';
          counter_string[2] = counter + '0';
          counter_string[3] = u + '0';
          MFS.write(counter_string);
          counter++;
          if(counter == 10){
            counter = 0;
            AnimationModeValue = ANIMATION_STAGE2;
          }
           break;
          
        case ANIMATION_STAGE2:
          counter_string[0] = counter + '0';
          counter_string[1] = counter + '0';
          counter_string[2] = d + '0';
          counter_string[3] = u + '0';
          MFS.write(counter_string);
          counter++;
          if(counter == 10){
            counter = 0;
            AnimationModeValue = ANIMATION_STAGE3;
          }
           break;
        case ANIMATION_STAGE3:
          counter_string[0] = counter + '0';
          counter_string[1] = c + '0';
          counter_string[2] = d + '0';
          counter_string[3] = u + '0';
          MFS.write(counter_string);
          counter++;
          if(counter == 10){
            counter = 0;
            AnimationModeValue = ANIMATION_STAGE4;
          }
           break;
        case ANIMATION_STAGE4:
          counter_string[0] = m + '0';
          counter_string[1] = c + '0';
          counter_string[2] = d + '0';
          counter_string[3] = u + '0';
          MFS.write(counter_string);
          MFS.blinkDisplay(DIGIT_ALL, ON);
          for(int i = 0; i < 3; i++) {
            MFS.beep(50);
            delay(500);
          }
          AnimationModeValue = ANIMATION_STOPPED;
          GeneratorModeValue = GENERATOR_STOPPED;
          break;
      }

      switch (ledModeValue) {
        case LED_ALL_OFF:
          ledModeValue = LED_1_ON;
          MFS.writeLeds(LED_1, ON);
          break;
        case LED_1_ON:
          MFS.writeLeds(LED_1, OFF);
          ledModeValue = LED_2_ON;
          MFS.writeLeds(LED_2, ON);
          break;
        case LED_2_ON:
          MFS.writeLeds(LED_2, OFF);
          ledModeValue = LED_3_ON;
          MFS.writeLeds(LED_3, ON);
          break;
        case LED_3_ON:
          MFS.writeLeds(LED_3, OFF);
          ledModeValue = LED_4_ON;
          MFS.writeLeds(LED_4, ON);
          break;
        case LED_4_ON:
          MFS.writeLeds(LED_4, OFF);
          ledModeValue = LED_1_ON;
          MFS.writeLeds(LED_1, ON);
          break;
      }
      break;

    case SETTING_RANGE_MAX_NUM_STARTED:
      MFS.write(maxi);
      MFS.blinkDisplay(DIGIT_ALL, ON);

      if(btn == BUTTON_1_SHORT_RELEASE) {
        RangeStatusValueMax = RANGE_SET;
        GeneratorModeValue = GENERATOR_STOPPED;
        ledModeValue = LED_1_ON; 
        MFS.writeLeds(LED_1, ON);
      } else if(btn == BUTTON_2_SHORT_RELEASE || btn == BUTTON_2_LONG_PRESSED) {
        maxi += 100;
        if(maxi > 9999) {
          maxi = 9999;
        }
        MFS.write(maxi);
        MFS.blinkDisplay(DIGIT_ALL, ON);
      } else if(btn == BUTTON_3_SHORT_RELEASE || btn == BUTTON_3_LONG_PRESSED) {
        maxi -= 100;
        if(maxi <= mini) {
          maxi += 100; 
        }
        MFS.write(maxi);
        MFS.blinkDisplay(DIGIT_ALL, ON);
      }

      break; 

    case SETTING_RANGE_MIN_NUM_STARTED:
      MFS.write(mini);
      MFS.blinkDisplay(DIGIT_ALL, ON);

      if(btn == BUTTON_1_SHORT_RELEASE) {
        RangeStatusValueMin = RANGE_SET;
        GeneratorModeValue = GENERATOR_STOPPED;
        ledModeValue = LED_2_ON; 
        MFS.writeLeds(LED_2, ON);
      } else if(btn == BUTTON_2_SHORT_RELEASE || btn == BUTTON_2_LONG_PRESSED) {
        mini += 100;
        if(mini >= maxi) {
          mini -= 100; 
        }
        MFS.write(mini);
        MFS.blinkDisplay(DIGIT_ALL, ON);
      } else if(btn == BUTTON_3_SHORT_RELEASE || btn == BUTTON_3_LONG_PRESSED) {
        mini -= 100;
        if(mini < 0) {
          mini = 0;
        }
        MFS.write(mini);
        MFS.blinkDisplay(DIGIT_ALL, ON);
      }
      break; 
  }
  delay(100);
}
