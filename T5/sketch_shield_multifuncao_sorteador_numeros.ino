
//Lívia Lutz dos Santos - 2211055
// Ana Luiza Pinto Marques - 2211960

#include <TimerOne.h> // Bibliotec TimerOne
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
byte RangeStatusValue = RANGE_NOT_SET;
byte AnimationModeValue = ANIMATION_STOPPED;
int u,d,c,m,numero,maxi = RANGE_MAX_NUMBER,mini = RANGE_MIN_NUMBER;

void setup() {
 
  Timer1.initialize(); 
  MFS.initialize(&Timer1);
  MFS.write("OFF"); 
  MFS.writeLeds(LED_ALL, OFF);
  Serial.begin(9600);
}

//falta fazer o interrupt tbm
void loop() {
  
  Serial.write(GeneratorModeValue + '0');
 
  byte btn = MFS.getButton();

  switch(GeneratorModeValue){
    case GENERATOR_STOPPED:
      ledModeValue = LED_ALL_OFF;
      MFS.write("OFF");

      switch(btn){
        case BUTTON_1_SHORT_RELEASE:
          if(RangeStatusValue == RANGE_SET){
            GeneratorModeValue = GENERATOR_STARTED;
            AnimationModeValue = ANIMATION_STARTED;
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

      //tem q consertar o beep aqui e a animacao
      case GENERATOR_STARTED:
        // deve tocar um beep curto de 50 ms no buzzer e iniciar a animação dos 4 displays de 7 segmentos e a animação sequencial dos 4 leds.
        MFS.beep(5);
        AnimationModeValue = ANIMATION_STARTED;
        numero = random(RANGE_MIN_NUMBER,RANGE_MAX_NUMBER);

          for(int i = 0; i < 9; i++){
            switch(AnimationModeValue){
              AnimationModeValue = ANIMATION_STAGE1;

              case ANIMATION_STAGE1:
                u = numero % 10;
                for(int i = 0;i < u;i++){
                  MFS.write(i); 
                }

                AnimationModeValue = ANIMATION_STAGE2;
                break;
                
              case ANIMATION_STAGE2:
                d = (numero / 10) % 10;
                for(int i = 0;i < d;i++){
                  MFS.write(i); 
                }

                AnimationModeValue = ANIMATION_STAGE3;
                break;
                
              case ANIMATION_STAGE3:
                c = (numero / 100) % 10;
                for(int i = 0;i < c;i++){
                  MFS.write(i); 
                }

                AnimationModeValue = ANIMATION_STAGE4;
                break;
              
              case ANIMATION_STAGE4:
                m = (numero / 1000) % 10;
                for(int i = 0;i < m;i++){
                  MFS.write(i); 
                }

                //implementar o interrupt
                AnimationModeValue = ANIMATION_INTERRUPTED;
                break;
            
            }
            
            switch (ledModeValue){
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
            
           }
        break;

      case SETTING_RANGE_MAX_NUM_STARTED:
        MFS.write(maxi);
        MFS.blinkDisplay(DIGIT_ALL, ON);

        if(btn == BUTTON_1_SHORT_RELEASE){
            RangeStatusValue = RANGE_SET;
            GeneratorModeValue = GENERATOR_STOPPED;
            ledModeValue = LED_1_ON; 
        }

        else if(btn == BUTTON_2_SHORT_RELEASE || btn == BUTTON_2_LONG_PRESSED ){
            maxi += 100;
            if(maxi > 9999){
              maxi = 9999;
            }
            MFS.write(maxi);
        }

         else if(btn == BUTTON_3_SHORT_RELEASE || btn == BUTTON_3_LONG_PRESSED ){
            maxi -= 100;
            if(maxi <= mini){
                maxi += 100; 
            }
            
            MFS.write(maxi);
        }
        
        break;
    
      case SETTING_RANGE_MIN_NUM_STARTED:
        MFS.write(mini);
        MFS.blinkDisplay(DIGIT_ALL, ON);
        
        if(btn == BUTTON_1_SHORT_RELEASE){
            RangeStatusValue = RANGE_SET;
            GeneratorModeValue = GENERATOR_STOPPED;
            ledModeValue = LED_2_ON; 
        }

        else if(btn == BUTTON_2_SHORT_RELEASE || btn == BUTTON_2_LONG_PRESSED ){
            mini += 100;
            if(mini >= maxi){
              mini -= 100; 
            }
            MFS.write(mini);
        }

         else if(btn == BUTTON_3_SHORT_RELEASE || btn == BUTTON_3_LONG_PRESSED ){
            mini -= 100;
            if(mini < 0000){
              mini = 0000;
            }
            MFS.write(mini);
        }
        
        MFS.blinkDisplay(DIGIT_ALL,OFF);
        break;
  }
      
  delay(100);
}
