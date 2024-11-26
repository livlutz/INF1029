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
byte GeneratorModeValue = GENERATOR_STOPPED;
byte RangeStatusValue = RANGE_NOT_SET;
byte AnimationModeValue = ANIMATION_STOPPED;

void setup() {
  // put your setup code here, to run once:
  Timer1.initialize(); // inicializa o Timer 1
  MFS.initialize(&Timer1);
  MFS.write(0); // imprime o valor 0 no display
  MFS.writeLeds(LED_ALL, OFF);
}

void loop() {
  // put your main code here, to run repeatedly:

  //pega o botão
  byte btn = MFS.getButton();

  switch(GeneratorModeValue){
    case GENERATOR_STOPPED:
      GeneratorModeValues = GENERATOR_STOPPED;
      ledModeValue = LED_ALL_OFF;
      MFS.writeLeds(ledModeValue, OFF);

      switch(btn){
        case BUTTON_1_SHORT_RELEASE:
          if(RangeStatusValue == RANGE_SET){
          GeneratorModeValue = GENERATOR_STARTED;
          AnimationModeValue = ANIMATION_STARTED;
          }
        break;

        case BUTTON_1_LONG_PRESSED:
          //nao faz nada n sei oq botar aqui
          break;

        case BUTTON_2_SHORT_RELEASE:
          MFS.writeLeds(LED_ALL_ON, RANGE_MAX_NUMBER);
          break;

        case BUTTON_2_LONG_PRESSED:
          GeneratorModeValue = SETTING_RANGE_MAX_NUM_STARTED;
          break;
        
        case BUTTON_3_SHORT_RELEASE:
          MFS.writeLeds(LED_ALL_ON, RANGE_MIN_NUMBER);
          break;
        
        case BUTTON_3_LONG_PRESSED:
          GeneratorModeValue = SETTING_RANGE_MIN_NUM_STARTED;
          break;
          
      }

      break;

      case GENERATOR_STARTED:
        break;

      case SETTING_RANGE_MAX_NUM_STARTED:
        break;
    
      case SETTING_RANGE_MIN_NUM_STARTED:
        break;
  }
      

}


