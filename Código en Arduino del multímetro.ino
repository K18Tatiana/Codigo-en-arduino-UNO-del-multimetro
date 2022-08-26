// Librerías para la pantalla
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128,64,&Wire,-1);
//Librerías para el módulo ADS1115
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;

// Entradas y salidas
#define res_2k 6
#define res_20k 7
#define res_470k 8
#define cap_pinAnalogo A0
#define carga_cap 13
#define descarga_cap 11
#define pulso_inductor 5
#define oscilacion_inductor 9
#define pin_salida A2
#define boton_escala 2
#define boton_modo 3

// Modo de las variables
  int modo = 0;
  int escala_res = 0;
  bool estado_boton_modo= true;
  bool estado_boton_escala = true;

  // Variables para el modo voltaje
  float voltaje = 0.0;
  float voltaje_resistencia = 0.0;
  float voltaje_bateria = 0.0;
  float medida_resistencia = 0.0;

  // Variables para el modo resistencia
  int valor_res_2k = 1963; //Resistencia de 2k
  float valor_res_20k = 19.48; //Resistencia de 20k
  float valor_res_470k = 0.465; //Resistencia de 470k

  // Variables para el modo capacitancia
  #define valorResistencia 9650.0F //Resistencia de 10k para cargar el capacitor
  unsigned long tiempoInicio;
  unsigned long tiempoTranscurrido;
  float microFaradios;
  float nanoFaradios;
  const float cap_interno PROGMEM = 47.48;
  const float cap_a_tierra PROGMEM = cap_interno;
  const float res_pullup PROGMEM = 34.8;
  const int valor_max_ADC PROGMEM = 1023;
  float capacitancia;

  // Variables para el modo inductancia
  double pulso, frecuencia, capacitor, inductancia;

  void dibujoCuadrado();

void setup(){
  pinMode(boton_escala,INPUT_PULLUP); //Definido en alto por el PullUp
  pinMode(boton_modo,INPUT_PULLUP);
  pinMode(res_2k,OUTPUT);
  pinMode(res_20k,INPUT);
  pinMode(res_470k,INPUT);
  pinMode(pin_salida,OUTPUT);
  pinMode(cap_pinAnalogo,OUTPUT);
  pinMode(pulso_inductor,OUTPUT);
  pinMode(oscilacion_inductor,INPUT);

  digitalWrite(res_2k,LOW);
  
  pinMode(carga_cap,OUTPUT);
  digitalWrite(carga_cap,LOW);

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC,0x3C); //Iniciar la pantalla OLED
  delay(100);
  display.clearDisplay(); // Limpiar la pantalla

  dibujoCuadrado();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(16,16);
  display.print("PROYECTO");
  display.setTextSize(2);
  display.setCursor(4,36);
  display.print("MULTIMETRO");
  display.display();

  while(digitalRead(boton_modo) && digitalRead(boton_escala)){
    delay(100);
  }
  ads.begin();
}

void loop(){
  if(!digitalRead(boton_modo) && estado_boton_modo){
    modo++;
    escala_res = 0;
    if(modo > 3){
      modo = 0; 
    }
    delay(100);
  }

  if(!digitalRead(boton_escala) && estado_boton_escala){
    escala_res++;
    if(escala_res > 2){
      escala_res = 0;
    }
    tiempoInicio = micros(); // Resetear el contador
    tiempoTranscurrido = micros() - tiempoInicio;
    delay(100);
  }

  // -----> MODO VOLTAJE
  if(modo == 0){
    int16_t adc0;
    adc0 = ads.readADC_SingleEnded(0);
    voltaje = 11 * (adc0 * 0.1875)/1000;
    if(voltaje > 0.5){
      voltaje += 0.5; //caída de voltaje del diodo solo si se aplica voltaje
    }
    int16_t adc2;
    adc2 = ads.readADC_SingleEnded(2);
    voltaje_bateria = (adc2 * 0.1875)/1000;

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE); 
    display.setCursor(0,0);
    display.print("Voltios: "); 
    display.setTextSize(1);
    display.setTextColor(BLACK,WHITE); 
    display.setCursor(95,0);    
    display.print(voltaje_bateria,1); 
    display.print("V");

    if(voltaje > 0){    
      display.setTextSize(3);
      display.setTextColor(WHITE); 
      display.setCursor(0,28);
      display.print(voltaje,4);        
      display.display();
      delay(100);
    } else{    
      display.setTextSize(3);
      display.setTextColor(WHITE); 
      display.setCursor(0,28);
      display.print("0.0000");         
      display.display();
      delay(100);
    }
  }

   // -----> MODO RESISTENCIA
   if(modo == 1){
    if(escala_res == 0){
      pinMode(res_2k,OUTPUT);
      pinMode(res_20k,INPUT);
      pinMode(res_470k,INPUT);     
      digitalWrite(res_2k,LOW);
     
      int16_t adc1;
      adc1 = ads.readADC_SingleEnded(1);
      voltaje_resistencia = (adc1 * 0.1875)/1000;
      int16_t adc2;
      adc2 = ads.readADC_SingleEnded(2);
      voltaje_bateria = (adc2 * 0.1875)/1000;
      medida_resistencia = (valor_res_2k) * ((voltaje_bateria/voltaje_resistencia)-1);

      if(voltaje_resistencia <= 0.01){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Ohms: ");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(voltaje_bateria,1); 
        display.print("V");

        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print("0.0000");  
        display.display();
        delay(100); 
      } else if(medida_resistencia < 10000){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Ohms: ");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(voltaje_bateria,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print(medida_resistencia,2);  
        display.display();
        delay(100);        
      } else{
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Ohms: ");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(voltaje_bateria,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print(">10k");  
        display.display();          
        delay(100);
      }
    } else if(escala_res == 1){
      pinMode(res_2k,INPUT);
      pinMode(res_20k,OUTPUT);
      pinMode(res_470k,INPUT);     
      digitalWrite(res_20k,LOW); 
      int16_t adc1;
      adc1 = ads.readADC_SingleEnded(1);
      voltaje_resistencia = (adc1 * 0.1875)/1000; 
      int16_t adc2; 
      adc2 = ads.readADC_SingleEnded(2);
      voltaje_bateria = (adc2 * 0.1875)/1000;      
      medida_resistencia = (valor_res_20k) * ((voltaje_bateria/voltaje_resistencia)-1);
      if(voltaje_resistencia <= 0.1){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Kohms: ");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(voltaje_bateria,1); 
        display.print("V");

        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print("0.0000");  
        display.display();
        delay(100); 
      } else if(medida_resistencia < 1000){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Kohms");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(voltaje_bateria,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print(medida_resistencia,3);  
        display.display();          
        delay(100);        
      } else{
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Kohms");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(voltaje_bateria,1); 
        display.print("V"); 
        
        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print(">1M");  
        display.display();          
        delay(100);
      }
    } else if(escala_res == 2){
      pinMode(res_2k,INPUT); 
      pinMode(res_20k,INPUT);
      pinMode(res_470k,OUTPUT);     
      digitalWrite(res_470k,LOW); 
      int16_t adc1;
      adc1 = ads.readADC_SingleEnded(1);
      voltaje_resistencia = (adc1 * 0.1875)/1000;
      int16_t adc2;
      adc2 = ads.readADC_SingleEnded(2);
      voltaje_bateria = (adc2 * 0.1875)/1000;
      medida_resistencia = (valor_res_470k) * ((voltaje_bateria/voltaje_resistencia)-1);
      if(voltaje_resistencia <= 0.1){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Mohms: ");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(voltaje_bateria,1); 
        display.print("V");

        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print("0.0000");  
        display.display();
        delay(100);
      } else if(medida_resistencia > 0 && medida_resistencia < 20){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("Mohms");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE);
        display.setCursor(95,0);
        display.print(voltaje_bateria,1);
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(0,28);
        display.print(medida_resistencia,4);
        display.display();
        delay(200);
      } else{
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Mohms"); 
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(voltaje_bateria,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print(">20M");  
        display.display();          
        delay(200);
      }
    }
  }

  // -----> MODO CAPACITANCIA
  if(modo == 2){
    int16_t adc2;
    adc2 = ads.readADC_SingleEnded(2);
    voltaje_bateria = (adc2 * 0.1875)/1000;
    pinMode(cap_pinAnalogo,INPUT);  
    pinMode(pin_salida,OUTPUT);
    digitalWrite(pin_salida,LOW); 
    pinMode(carga_cap,OUTPUT);
    digitalWrite(carga_cap,HIGH); //Aplica 5V
    tiempoInicio = micros(); //Empieza el contador
    while(analogRead(cap_pinAnalogo) < 600){ //Solo espera que pase el tiempo cuando el valor de carga_pin sea menor a 648display.setTextSize(3);
    }
    tiempoTranscurrido = micros() - tiempoInicio;
    microFaradios = ((float)tiempoTranscurrido / valorResistencia) ; //Calcula el valor de la capacitancia
  
    if (microFaradios > 1){
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE); 
      display.setCursor(0,0);
      display.print("uF: ");
      display.setTextSize(1);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(95,0);    
      display.print(voltaje_bateria,1); 
      display.print("V");
        
      display.setTextSize(3);
      display.setTextColor(WHITE); 
      display.setCursor(0,28);
      display.print(microFaradios);  
      display.display();          
      delay(2000);   

      digitalWrite(carga_cap,LOW);            
      pinMode(descarga_cap,OUTPUT);            
      digitalWrite(descarga_cap,LOW); //Descarga del capacitor     
      while(analogRead(cap_pinAnalogo) > 0){ //Solo espera mientras el capacitor se descarga  
      }
      pinMode(descarga_cap,INPUT); //Establece el pin en alta impedancia
    } else{
      pinMode(carga_cap,INPUT);
      pinMode(descarga_cap,INPUT);
      pinMode(cap_pinAnalogo,INPUT);
      digitalWrite(pin_salida,HIGH);
      int val = analogRead(cap_pinAnalogo);
      digitalWrite(pin_salida,LOW);
      Serial.println(val);
  
      if (val < 1000){
        pinMode(cap_pinAnalogo,OUTPUT);
        capacitancia = (float)val * cap_a_tierra / (float)(valor_max_ADC - val);
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("pF");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE);
        display.setCursor(95,0);
        display.print(voltaje_bateria,1);
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(WHITE); 
        display.setCursor(0,28);
        display.print(capacitancia);  
        display.display();
        delay(1000);
      } else{
        pinMode(cap_pinAnalogo,OUTPUT);
        delay(1);
        pinMode(pin_salida,INPUT_PULLUP);
        unsigned long u1 = micros();
        unsigned long t;
        int val_dig;
  
        do{
          val_dig = digitalRead(pin_salida);
          unsigned long u2 = micros();
          t = u2 > u1 ? u2 - u1 : u1 - u2;
        } while ((val_dig < 1) && (t < 400000L));
  
        pinMode(pin_salida,INPUT);  
        val = analogRead(pin_salida);
        digitalWrite(cap_pinAnalogo,HIGH);
        int tiempoDescarga = (int)(t/1000L) * 5;
        delay(tiempoDescarga);   
        pinMode(pin_salida,OUTPUT);  
        digitalWrite(pin_salida,LOW);
        digitalWrite(cap_pinAnalogo,LOW);
  
        capacitancia = -(float)t / res_pullup / log(1.0 - (float)val / (float)valor_max_ADC);
     
        if (capacitancia > 1000.0){
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE); 
          display.setCursor(0,0);
          display.print("uF");
          display.setTextSize(1);
          display.setTextColor(BLACK,WHITE); 
          display.setCursor(95,0);    
          display.print(voltaje_bateria,1); 
          display.print("V"); 
          
          display.setTextSize(3);
          display.setTextColor(WHITE); 
          display.setCursor(0,28);
          display.print(capacitancia/1000);  
          display.display();
          delay(1000);        
        } else{
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE); 
          display.setCursor(0,0);
          display.print("nF");
          display.setTextSize(1);
          display.setTextColor(WHITE); 
          display.setCursor(95,0);    
          display.print(voltaje_bateria,1); 
          display.print("V"); 
          
          display.setTextSize(3);
          display.setTextColor(WHITE); 
          display.setCursor(0,28);
          display.print(capacitancia);  
          display.display();
          delay(1000);
        }
      }
    }  
  }

  // -----> MODO INDUCTANCIA
  if(modo == 3){
    digitalWrite(pulso_inductor,HIGH);
    delay(4); // Dar tiempo para cargar el inductor
    digitalWrite(pulso_inductor,LOW);
    delayMicroseconds(100); // Medir resonancia
    pulso = pulseIn(oscilacion_inductor,HIGH,5000); // Regresa a 0 si se agota el tiempo
    
    if(pulso > 0.1){ // Si no se produjo un tiempo de espera, toma una lectura
      capacitor = 2E-6; 
       
      int16_t adc2;
      adc2 = ads.readADC_SingleEnded(2);
      voltaje_bateria = (adc2 * 0.1875)/1000;
      
      frecuencia = 1E6/(2*pulso);
      inductancia = 1./(capacitor * frecuencia * frecuencia * 4 * 3.14159 * 3.14159);
      inductancia *= 1E6; // Valor del capacitor = 1E6

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE); 
      display.setCursor(0,0);
      display.print("uH");
      display.setTextSize(1);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(95,0);    
      display.print(voltaje_bateria,1);
      display.print("V");    
          
      display.setTextSize(3);
      display.setTextColor(WHITE); 
      display.setCursor(0,28);
      display.print(inductancia);  
      display.display();
      delay(100);      
    }else{
      int16_t adc2;
      adc2 = ads.readADC_SingleEnded(2);
      voltaje_bateria= (adc2 * 0.1875)/1000;
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.print("uH");
      display.setTextSize(1);
      display.setTextColor(BLACK,WHITE);
      display.setCursor(95,0);
      display.print(voltaje_bateria,1);
      display.print("V");
          
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(0,28);
      display.print("0.0000");
      display.display();
      delay(100);
    }
  }
}

void dibujoCuadrado(){
  display.drawRect(0, 0, 128, 64, WHITE);
}