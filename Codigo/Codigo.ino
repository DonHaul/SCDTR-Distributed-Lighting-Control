//com quantos valores se vai fazer a media
#define tamluxs 5

//pin onde se localiza o led (MUDAR PARA DEFINE)
int led = 9;

//pin onde se localiza o LDR (MUDAR PARA DEFINE)
int analogPin = 1;
int val = 0;


int brightness = 0;
float tensao = 0.0;


//Resistencia que nao e' o LDR (MUDAR PARA DEFINE)
float r1 = 10000.0;
int contador = 0;
int primeiro = 0;
int contador2 = 0;
float luxs = 0;
int i = 0;
float vetor[256];

int ilum_min = 3;
float last_luxs[tamluxs];

//novas variaveis de controlo
float K1, K2, Kp, Ki, b, y, e , k, p,u, y_ant = 0, i_ant = 0, e_ant = 0, T;


//função que calcula a média dos luxs
float average()
{
  float somatodos = 0, media = 0;
  int i;

  //soma todos os valores
  for (i = 0; i < tamluxs; i++)
  {
    somatodos = last_luxs[i] + somatodos;
  }
  
  //divide pelo numero de valores
  media = somatodos / tamluxs;

  return media;
}



//função que calcula o valor de luxs
float calc_luxs(int val)
{
  float R2;
  
  //regra de 3 simples para converter de 0-1023 para 0-5V
  tensao = (5 * val) / 1023.0;

  //equacao do divisor de tensao para obter resistencia R2
  R2 = ((r1 * 5) / tensao) - r1;

  //uso de reta para converter de R para lux
  luxs = ((log10(R2) - 4.8451) / -0.7186);
  luxs = pow(10, luxs);

  return luxs;

}

//função que preenche look up table
void calibration()
{
  //itera por todos os valores possiveis para o led
  for (i = 0; i < 256; i++)
  {
    delay(50);

    //poe i como pwm do led corresponde à brightnedd
    analogWrite(led, i);

    //calcula lux atraves do valor lido no pino
    luxs = calc_luxs(analogRead(analogPin));

    //define valor na lookuptable
    vetor[i] = luxs;

    //prints para ver o que esta a acontecer
    Serial.print(luxs);
    Serial.print(' ');
    Serial.println(brightness);

  }

  Serial.print("Calibration Complete");
}

//faz shif left dos last_lux e adicion o ultimo valor (TROCAR ISTO POR FUNCAO QUE MIRAGAIA QUERIA)
void shift_left(float current_luxs)
{
  for (int i = 1; i < tamluxs; i++)
  {
    last_luxs[i - 1] = last_luxs[i];
  }
  last_luxs[tamluxs - 1] = current_luxs;
}

//funcao onde e controlado a luminosidade do led atraves de outras cenas
void controlo()
{
  float integ;      //termo integrador

  //le pino
  val = analogRead(analogPin);

  //obtem luxs
  y = calc_luxs(val);

  //calcula erro em relacao a referencia ilum_min
  e = ilum_min - y;

  //obtem parte proporcional
  p = K1*ilum_min-Kp*y;

  //obtem parte integral
  integ = i_ant + K2*(e + e_ant);

  //descobre valor led
  u = p + integ;

  //faz analog write de tal valor
  analogWrite(led, u);

  //faz set das variaveis para o proximo loop
  y_ant = y;
  i_ant = k;
  e_ant = e;
    
}


void setup() {

  //inicializa tamluxs a -1
  for (int i = 0; i < tamluxs; i++)
  {
    last_luxs[i] = -1;
  }

  //calcula constantes com base nos parametros
  K1 = Kp*b;
  K2 = Kp*Ki*T/2;

  //comecao serial comunication  
  Serial.begin(9600);

  //calibra sistema
  calibration();
}


// the loop function runs over and over again forever
void loop() {

  //set the led to something
  analogWrite(led, 228);   // turn the LED on (HIGH is the voltage level)

  delay(25);

  //le valor do led
  val = analogRead(analogPin);

  //calcula luxs
  luxs = calc_luxs(val);

  //faz shift left dos luxs
  shift_left(luxs);
  
  //faz print da average
  Serial.println(average());


}
