//TEM Q BOTAR OS NOMES

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
        // deve tocar um beep curto de 50 ms no buzzer e iniciar a animação dos 4 displays de 7 segmentos e a animação sequencial dos 4 leds.
        MFS.beep(5); 
        
        break;

      case SETTING_RANGE_MAX_NUM_STARTED:
        /*Neste estado, o sorteador deve apresentar o valor máximo da faixa de números aleatórios
atualmente configurado piscando no display de 7 segmentos*/
        MFS.writeLeds(LED_ALL_ON, RANGE_MAX_NUMBER);
        
         switch(btn){
          case BUTTON_1_SHORT_RELEASE:
            /*encerrar o estado de configuração do valor máximo da faixa
de números aleatórios, mudar a situação de configuração desse valor para
RANGE_SET, acender o LED 1 e voltar para o estado GENERATOR_STOPPED.*/
            RangeStatusValue = RANGE_SET;
            GeneratorModeValue = GENERATOR_STOPPED;
            ledModeValue = LED_1_ON;
            
            break;
  
          case BUTTON_1_LONG_PRESSED:
            //ignorar n sei como faz isso
            break;
  
          case BUTTON_2_SHORT_RELEASE:
            /*incrementar o valor máximo da faixa de números aleatórios
de 100 unidade e apresentar esse valor piscando no display de 7 segmentos */
            
            break;
  
          case BUTTON_2_LONG_PRESSED:
            /*incrementar o valor máximo da faixa de números aleatórios
de 100 unidade e apresentar esse valor piscando no display de 7 segmentos */
            break;
          
          case BUTTON_3_SHORT_RELEASE:
            /*decrementar o valor máximo da faixa de números aleatórios
de 100 unidade e apresentar esse valor piscando no display de 7 segmentos*/
            break;
          
          case BUTTON_3_LONG_PRESSED:
            /*decrementar o valor máximo da faixa de números aleatórios
de 100 unidade e apresentar esse valor piscando no display de 7 segmentos*/
            break;
        }
        
      break;
    
      case SETTING_RANGE_MIN_NUM_STARTED:
        MFS.writeLeds(LED_ALL_ON, RANGE_MIN_NUMBER);

        switch(btn){
          case BUTTON_1_SHORT_RELEASE:
            RangeStatusValue = RANGE_SET;
            GeneratorModeValue = GENERATOR_STOPPED;
            ledModeValue = LED_2_ON;
            break;
  
          case BUTTON_1_LONG_PRESSED:
            //ignorar n sei como faz isso
            break;
  
          case BUTTON_2_SHORT_RELEASE:
            /*ncrementar o valor mínimo da faixa de números aleatórios
de 100 unidade e apresentar esse valor piscando no display de 7 segmentos */
            break;
  
          case BUTTON_2_LONG_PRESSED:
            /*ncrementar o valor mínimo da faixa de números aleatórios
de 100 unidade e apresentar esse valor piscando no display de 7 segmentos */
            break;
          
          case BUTTON_3_SHORT_RELEASE:
            /*decrementar o valor mínimo da faixa de números aleatórios
de 100 unidade e apresentar esse valor piscando no display de 7 segmentos*/
            break;
          
          case BUTTON_3_LONG_PRESSED:
           /*decrementar o valor mínimo da faixa de números aleatórios
de 100 unidade e apresentar esse valor piscando no display de 7 segmentos*/
            break;
        }
        
        break;
  }
      

}


