#include // Bibliotec TimerOne
#include #define RANGE_MAX_NUMBER 9999
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
byte GeneratorModeValues = GENERATOR_STOPPED;

void setup() {
  // put your setup code here, to run once:
  Timer1.initialize(); // inicializa o Timer 1
  MFS.initialize(&Timer1);
}

void loop() {
  // put your main code here, to run repeatedly:
  byte btn = MFS.getButton();

  switch(GeneratorModeValues){
    case GENERATOR_STOPPED:
      GeneratorModeValues = GENERATOR_STOPPED;
      ledModeValue = LED_ALL_OFF;
      MFS.writeLeds(LED_ALL_OFF, OFF)
      break;

    case GENERATOR_STARTED:
    
    case SETTING_RANGE_MAX_NUM_STARTED:
    
    case SETTING_RANGE_MIN_NUM_STARTED

  }

}
