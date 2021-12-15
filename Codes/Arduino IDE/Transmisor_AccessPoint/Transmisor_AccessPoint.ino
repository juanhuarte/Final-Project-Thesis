/*
 * GPIO 22 ...... SCL
 * GPIO 21 ...... SDA
 * 3.3 conecto al OLED y la pata derecha del pote
 * GND conecto al OLED y la pata izquierda del pote
 * GPIO 34 ADC1 ..... pata del medio del pote
 */
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h> //libreria para la pantalla OLED
#include <Adafruit_SSD1306.h> //libreria para la pantalla OLED

// Definir constantes
#define ANCHO_PANTALLA 128 // ancho pantalla OLED
#define ALTO_PANTALLA 64 // alto pantalla OLED

// Objeto de la clase Adafruit_SSD1306
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);
 
// PUNTO DE ACCESO 

const char* ssid = "ESP32 WiFi";                        //SSID de nuestra red
const char* password = "wifiesp32";                //Contraseña de nuestra red

const int   port = 80;  // puerto para realizar la conexion TCP/IP

WiFiServer server(port);
WiFiClient client ;

// Variables de la Interrupcion
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile int interruptCounter;

int volt=0 ;  // variable que utilizo para leer el nivel de la bateria
const int potPin = 34; // Pin del ADC

int marca=0; // flag para la conexion y desconexion del cliente
int tiempoconectado = 0; // para que imprima en la pantalla OLED el estado de la conexion

short muestra = 0;  // varialbe que utilizo para almacenar muestras
int contador = 0;   // Cuenta los ciclos de reloj hasta llegar a 1 segundo
byte b1[2];               //Variables de la funcion de buffer

const int LedPin=2; // declarar el pin del led de la esp32, que se utilizara para la señal de apagado

const int circularBufferLength = 512    ;  //largo del buffer circular
byte circularBufferUNO[circularBufferLength];     //array del buffer
unsigned int circularBufferAccessorUNO = 0;   //puntero del buffer

byte circularBufferDOS[circularBufferLength];     //array del buffer
unsigned int circularBufferAccessorDOS = 0;   //puntero del buffer


int banderaBufferUNO = 0;  // Se pone en alto cuando el buffer se encuentra completo
int banderaBufferDOS = 0;  // Se pone en alto cuando el buffer se encuentra completo
int punteroBuffers = 0;   // cuenta la cantidad de datos que entraron para verificar a cual de los dos buffer lo envia

/// VARIABLES Señal apagado ///
int tiempoDesconectadoUNO = 0;  
int tiempoDesconectadoDOS = 0;
//////////////////////////////


void MuestrasBufferUNO(byte value)
{
    if (circularBufferAccessorUNO == circularBufferLength)
  {
     circularBufferAccessorUNO = 0;
  }
  circularBufferUNO [circularBufferAccessorUNO] = value;
  circularBufferAccessorUNO++;
}

void MuestrasBufferDOS(byte value)
{
    if (circularBufferAccessorDOS == circularBufferLength)
  {
     circularBufferAccessorDOS = 0;
  }
  circularBufferDOS [circularBufferAccessorDOS] = value;
  circularBufferAccessorDOS++;
}

void enviar ( byte* dato)
  {    
        client.write(dato,circularBufferLength);        
  }
  
void NivelBateria()
{
       volt= map(analogRead(potPin), 0, 4095, 0, 100); // primeros dos terminos rango del ADC de 0 a 4096-1 y los ultimos dos rangos son el porcentaje de 0 a 100
       //Limpiar buffer
      display.clearDisplay();
      // Tamaño del texto
      display.setTextSize(1);
      // Color del texto
      display.setTextColor(SSD1306_WHITE);
      // Posición del texto
      display.setCursor(90, 0);
      display.print(volt);
      display.print("%");
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("Battery Level");
}  
  
  // ISR to Fire when Timer is triggered
 void IRAM_ATTR onTimer() {  // interrupción por igualdad de comparación en TIMER1
  portENTER_CRITICAL_ISR(&timerMux); 
  interruptCounter=interruptCounter+1;
 if (marca == 1)
 {
  
      if (punteroBuffers == circularBufferLength) // Cuando se llena el buffer 2 se activa la bandera de envio 
          {
            punteroBuffers = 0;
            banderaBufferDOS = 1;  
          }
      else if (punteroBuffers == circularBufferLength/2)  // Cuando se llena el buffer 1 se activa la bandera de envio 
          {
            banderaBufferUNO = 1; 
          }
  
      if (muestra == 4096)  // funcion diente de sierra
            {
                muestra = 0;
            } 

     // Recibir muestras del ADC       
     /*   muestra = int(adcEnd(potPin));
        adcStart(potPin); */
          
  // Almacenar en el buffer 1
      if (punteroBuffers < circularBufferLength/2)
            {
                b1[1]= muestra& 0xff;  // extraemos el byte bajo 
                MuestrasBufferUNO(b1[1]);  //llamado de la funcion del buffer de muestras
                b1[0]=(muestra>>8) & 0xff;  // roto los bits 8 veces a la derecha y de nuevo me quedo con el ultimo byte
                MuestrasBufferUNO(b1[0]);   //llamado de la funcion del buffer de muestras
                muestra = muestra + 1;   // funcion diente de sierra
                punteroBuffers = punteroBuffers + 1;
            }
   // Almacenar en el buffer 2
      else if (punteroBuffers >= circularBufferLength/2 && punteroBuffers < circularBufferLength)
            {
                b1[1]= muestra& 0xff;
                MuestrasBufferDOS(b1[1]);
                b1[0]=(muestra>>8) & 0xff;
                MuestrasBufferDOS(b1[0]);   
                muestra = muestra + 1; 
                punteroBuffers = punteroBuffers + 1;
            }
 }  
 
  contador = contador + 1;
  if (contador == 20000)    
  {
    tiempoconectado = 1;
    contador=0;
  }
 portEXIT_CRITICAL_ISR(&timerMux);         
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200) ;
    pinMode(LedPin, OUTPUT);
    digitalWrite(LedPin, LOW);  
    
  delay(100);
  Serial.println("Iniciando pantalla OLED");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   
  NivelBateria();  // llamado a la funcion
   
  //Tamaño del texto
  display.setTextSize(1);
  //color del texto
  display.setTextColor(SSD1306_WHITE);
  //posicion del texto
  display.setCursor(10,32);
  //escribir texto
  display.println("Display prendido");
  //enviar a pantalla
  display.display();

  delay(2000);

// Punto de acceso 
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);
  server.begin();
  Serial.println("Esperando al cliente"); 

  NivelBateria();
  
  //Tamaño del texto
  display.setTextSize(1);
  //color del texto
  display.setTextColor(SSD1306_WHITE);
  //posicion del texto
  display.setCursor(10,32);
  //escribir texto
  display.println("Iniciando la red:");
  display.setCursor(20,40);
  //escribir otro texto
  display.println(ssid);
  //enviar a pantalla
  display.display();

  delay(10000);

  NivelBateria();
  
  //Tamaño del texto
  display.setTextSize(1);
  //color del texto
  display.setTextColor(SSD1306_WHITE);
  //posicion del texto
  display.setCursor(20,32);
  //escribir texto
  display.println("Password:");
  display.setCursor(20,40);
  //escribir otro texto
  display.println(password);
  //enviar a pantalla
  display.display();
  delay(10000);
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");   
  IPAddress IP = WiFi.softAPIP();
  Serial.println(IP);    
  
 // adcAttachPin(potPin);  // le asignas el pin del ADC
 // adcStart(potPin);  // empieza la convercion
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // Fire Interrupt every 1m ticks, so 1s
  timerAlarmWrite(timer, 50, true);   // 50 significa 20k, 25 significa 40k
  timerAlarmEnable(timer);

  
}

void loop() {
  // put your main code here, to run repeatedly:
    // Verificar si el cliente esta conectado 
        if (marca == 0)
        {
          tiempoDesconectadoDOS = millis();
          client = server.available();
           if (!client)
           {
           }
           else 
           {
            Serial.println ("Cliente Conectado");
            marca = 1;
            if (client.connected()) 
             {
              Serial.println("Connected to client");
               NivelBateria();
             //Tamaño del texto
               display.setTextSize(1);
              //color del texto
             display.setTextColor(SSD1306_WHITE);
            //posicion del texto
            display.setCursor(10,32);
            //escribir texto
             display.println("Cliente Conectado");
              //enviar a pantalla
              display.display();
            }
           }
        }

   if (tiempoconectado == 1)
    {
      tiempoconectado=0;
       if (!client)
       { 
          if (marca == 1)
          {
            tiempoDesconectadoUNO = millis();
          }
          marca = 0;
          Serial.println ("Cliente Desconectado");
           NivelBateria();
           //Tamaño del texto
           display.setTextSize(1);
           //color del texto
           display.setTextColor(SSD1306_WHITE);
          //posicion del texto
         display.setCursor(40,30);
         //escribir texto
         display.print("Cliente");
         //Tamaño del texto
          display.setTextSize(1);
          //color del texto
          display.setTextColor(SSD1306_WHITE);
          //posicion del texto
         display.setCursor(25,40);
         //escribir texto
         display.print("Desconectado");
         //enviar a pantalla
         display.display();
       }
      else 
      {
      Serial.println ("Cliente Conectado");
      marca = 1;
      if (client.connected()) 
        {
              Serial.println("Connected to client");
               NivelBateria();
             //Tamaño del texto
               display.setTextSize(1);
              //color del texto
             display.setTextColor(SSD1306_WHITE);
            //posicion del texto
            display.setCursor(10,32);
            //escribir texto
             display.println("Cliente Conectado");
              //enviar a pantalla
              display.display();
        }
      }      
    }

    // Verificamos que el tiempo de desconexion es mayor a 3 minutos (180000)
    if (tiempoDesconectadoDOS - tiempoDesconectadoUNO >= 60000)  
    {
      Serial.println("ENVIO LA SEÑAL DE APAGADO");
      digitalWrite(LedPin, HIGH);  
    }

// si la bandera se encuentra en alto se envia el paquete de datos de cada buffer
if (marca == 1)
{
    tiempoDesconectadoDOS = millis();
    tiempoDesconectadoUNO = millis();
    if ( banderaBufferUNO == 1) 
        {
          banderaBufferUNO = 0;
          enviar (circularBufferUNO);
        }
    else if (banderaBufferDOS == 1)
        {
          banderaBufferDOS = 0;
          enviar (circularBufferDOS);
        }
    if(circularBufferAccessorUNO == 0 || circularBufferAccessorDOS == 0)
    {
      client.write(255);
      client.write(255);
    } 
  
}
}
