#include <SoftwareSerial.h>
#include <Ultrasonic.h>

//Definindo Serial de comunicação com Esp8266
SoftwareSerial ArduinoUno(3,2);

//Define os pinos conectados com base nas funções
#define pino_trigger 4
#define pino_echo 5
#define bomba 7
#define agualvl 8 

//Inicializa o sensor de distância nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo); 

int ativ = 0;

void setup(){

 //Inicia A conexão Serial com o Esp8266
	ArduinoUno.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  
  //Inicia os pinos configurados
  pinMode(bomba, OUTPUT);
  pinMode(agualvl, INPUT);//captando informação "INPUT"
}


//Função para enviar informações para o Esp8266
void SendInf(String mensagem){
  //interpreta oque o Esp pediu e envia o Nível de Água
  if(mensagem.indexOf("aqualvl")>-1){
    int lvl = digitalRead(agualvl);
    ArduinoUno.println("inflvl "+String(lvl));
  }
  
  //envia o sinal para o Esp avisando que a bomba foi ativada
  if(mensagem.indexOf("bebeu")>-1){
    ArduinoUno.println("drink");
  }
}


void loop(){
  //Lê as informações do sensor de distância, em cm
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  //Lê as informações do sensor de nivel de água
  int estado = digitalRead(agualvl);
  
  //Limita a ativação da bomba com base na proximidade(sensor de distância) e pelo nìvel de água(sensor de nivel de água)
  if(cmMsec < 20 && estado == 1){
    //atrasar a ativação para não ligar sem nescessidade e não enviar informação errada
    delay(2000);
    //Re-Lê as informações dos sensores Sensor de distância e de nivel de água 
    microsec = ultrasonic.timing();
    cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
    estado = digitalRead(agualvl);
    //verifica se as condiçôes ainda estão batendo
    //se estiverem ativa se não, então não ativa
   if(cmMsec < 20 && estado == 1){
    ativ = 1;
    //Chama a função para informar ao Esp que a bomba foi ativada
    SendInf("bebeu");
   }
  }
  
//desativando a bomba por fora para ter certeza que ela não ative sozinha derrepente
  digitalWrite(bomba, 0);
//ativando a bomba
  if(ativ == 1){
    digitalWrite(bomba, 1);
     delay(5000);
     ativ = 0;
     digitalWrite(bomba, 0);
  }
  
  //Lendo informações que o Esp manda
  if(ArduinoUno.available()>0){
  String val = ArduinoUno.readString();
  //Chamando função para interpretar a "mensagem" recebida
  SendInf(val);
  }
}
