

#include "LedControl.h"
// Programa para utilizar con matriz con dezplazamiento vertical

                         //DIN,ClCK,CS,#pantalla
LedControl lc = LedControl(51,52,53,1);

int velocidad_msg = 70; // determina la velocidad del mensaje
unsigned long tiempo = 0; // utilizado en la velocidad del mensaje
int  val   = analogRead(A0);
int fila[] = {22,23,24,25,26,27,28,29};
int col[]  = {30,31,32,33,34,35,36,37};

int contador; // para ver que letra del vector 'palabras' se enviara al vector pantalla
bool sentido_movimiento; // direccion hacia donde se esta moviendo las letras ( true = abajo, false = arriba ) 

int pinPausa = 4;
int pin_interruptor = 38; 
int actual_intp;    // para saber el estado actual en el que se encuentra el interruptor antes de ser modificado
int monitoreo_intp; // variable que se mantiene en constante cambio para determinar un cambio en el interruptor
int sizeof_palabras; // determina el tamaño del vector palabras, para realizar verificaciones
int modo; // 2 posibles modos ( mensaje, juego)
int tiempo_transcurrido; // determina la presion del boton

// VARAIBLES PARA CUENTA REGRESIVA


bool inicio = false;


int pantalla[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; // bytes


 byte palabras[]= {
   
                    0,0,0,0,0,0,0,0, // ESPACIO 
                    0,8,8,8,8,24,8,0, // numero 1
                    0,36,36,60,36,36,24,0, // letra A
                    0,28,32,32,32,28,0,0,  // letra C 
                    0,126,24,24,24,24,126,0, // letra I
                    0,24,24,24,24,126,0,0, // letra T
                    0,28,32,32,32,28,0,0,  // letra C
                    0,36,36,60,36,36,24,0, // letra A
                    0,36,40,48,40,36,56,0, // letra R
                    0,32,32,32,56,36,56,0, // letra P
                    0,0,0,126,126,0,0,0, // Guion
                    0,36,36,60,36,36,24,0, // letra A
                    0,34,38,42,50,34,0,0, // letra N 
                    0,60,66,66,66,66,60,0,   // letra O
                    0,126,24,24,24,24,126,0, // letra I
                    0,28,32,32,32,28,0,0,  // letra C
                    0,28,32,32,32,28,0,0,  // letra C 
                    0,60,32,60,32,60,0,0, // letra E
                    0,56,4,24,32,32,28,0, // letra S  
                    0,0,0,126,126,0,0,0, // Guion
                    0,4,4,60,36,36,0,0, // numero 4
                    0,62,66,66,78,64,62,0 // letra G
                                           
 };

// DIGITOS QUE SE PUEDEN GRAFICAR EN LA PANTALLA (matriz de led)
byte cero[] =    { 0,0,60,36,36,36,60,0 };
byte uno []    = { 0,8,24,8,8,8,8,0 };
byte dos []    = { 0,0,56,8,16,32,56,0 };
byte tres []   = { 0,56,8,16,8,56,0,0 };
byte cuatro [] = { 0,0,36,36,60,4,4,0 };
byte cinco []  = { 0,0,60,32,32,28,4,60 };
byte seis []   = { 0,60,32,32,60,36,60,0};
byte siete []  = { 0,62,4,8,16,32,0,0 };
byte ocho []   = { 0,24,36,36,24,36,36,24};
byte nueve []  = { 0,0,60,36,60,4,4,0};





// ================= DECLARACION DE VARIABLES PARA JUEGO ROADFIGHTER

int bi=2;
int bd=3;
unsigned long tt = 0;
unsigned long tt2 = 0;
int lectura = LOW;
int lectura2 = LOW;

unsigned long t_shift = 0;
int spawn_enemigo = 2500; // la velocidad con la que inicializamos el juego (varia cada 10 segundos)
unsigned long t_actual; // para saber en que momento debemos incrementar la dificultad del juego
byte matriz1[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int contaE = 0;
byte enemigo[] = { 0,0,0,16, 56}; // para visualizar el enemigo hay que ver el byte en bits
byte enemigo2[] = {0,0,0, 64, 224};
byte enemigo3[] = {0,0, 0,8, 28};
byte enemigo4[] = {0,0,0, 2, 7};
long  nRandom;
byte *nuevo_enemigo; 
bool colision; // valida si el jugador ya ha colisionado
bool cuentaR = false;

byte jugador[] = {56,16};
int nivelJuego;
unsigned long segundos; // para poder calcular los segundos 
int tiempoJugado; // total se segundos que ha jugado el jugador max (99 segundos)






void setup() 
{
  inicializar_pin();
  lc.shutdown(0,false);
  lc.setIntensity(0,4);
  lc.clearDisplay(0);
  Serial.begin(9600);

  modo = 1; // modo inicial sera el del mensaje
  contador = -1; // para poder dar en sentido de movimiento hacia abajo
  sentido_movimiento = true; // el sentido del movimiento sera hacia abajo siempre
  actual_intp = digitalRead(pin_interruptor); // lectura inicial del interruptor
  sizeof_palabras = sizeof(palabras); // al iniciar determinamos el tamaño del vector palabras
  tiempo_transcurrido = 0;

  randomSeed(analogRead(4));
  nRandom = random(0,5); // random de 0 - 4 (antes de iniciar el programa)
  nuevo_enemigo = get_Enemigo_Random(nRandom); // obtenemos el primer enemigo con su aparicion random
}





void loop() 
{
  if(modo == 1) // modo mensaje
  {
    val = analogRead(A0); // leer constantemente el valor del potenciometro
    Modificar_velocidad(val);
    monitorear_sentido_movimiento();
    corrimiento_enPantalla();

  }else if(modo == 0){ // modo == 0 (juego)
    
    if(!inicio) // ingresa solo la primera vez que se inicia el juego
    {

      inicio = true;   
      t_actual = millis();
      nivelJuego = 0; // inicia en 0 siempre
      segundos = 0;
      tiempoJugado = 0;
    }
    // conteo regresivo
    if(!cuentaR)
      cuentaRegresiva(); // solo ejecutar 1 vez
    jugarRoadFighter();
    tiempo = millis(); // mantener actualizado el tiempo del mensaje, aunq se este jugando 
    
  }
  else if(modo == 2) // modo pausa
  {
   // unsigned long juego_tiempo = (millis() - t1_juego)/1000;
    //Serial.println(juego_tiempo);
    //int t = (ttpruebajuego2 - t1_juego)/1000;
      //Serial.println(t);
      //int ttt = juego_tiempo;  
    segundos = millis(); // para mantener segundos actualizado..
    stoi_tiempo(tiempoJugado); 

  }

  deteccionPausa();

  
}


// ====================== CASTEAR UNSIGNED LONG  =============

void stoi_tiempo(int tiempo) // castear int a array de caracteres
{
  if(tiempo > 99)
    tiempo = 99;
  char snum[5];
  sprintf(snum, "%i", tiempo);
  //Serial.println(snum[0]);

  char valor1, valor2;

  if(tiempo < 10){

    valor1 = '0';
    valor2 = snum[0];
    
  }else{

      valor1 = snum[0];
      valor2 = snum[1];
  }

  Graficar_tiempo(valor1, valor2);  
}


void Graficar_tiempo(char val1, char val2)
{
  lc.clearDisplay(0);
        
  byte * v1 = getArray_deNumeros(val1); // graficar en la matriz sin driver
  for(int i=0; i< 8; i++)
    lc.setRow(0, i, v1[i]);
  
  byte * v2 = getArray_deNumeros(val2); // graficar en la matriz nativa
  for(int i=0; i< 8; i++)
  {
    PORTC = v2[i];
    digitalWrite(fila[i],LOW);
    delay(1);
    digitalWrite(fila[i],HIGH);  
  }

  
}


byte * getArray_deNumeros(char numero)
{
  switch(numero)
  {
    case '0':{return cero;}break;
    case '1':{return uno;}break;
    case '2':{return dos;}break;
    case '3':{return tres;}break;
    case '4':{return cuatro;}break;
    case '5':{return cinco;}break;
    case '6':{return seis;}break;
    case '7':{return siete;}break;
    case '8':{return ocho;}break;
    case '9':{return nueve;}break;
  }
 return 0;
}

// ======================= CUENTA REGRESIVA ============

void cuentaRegresiva()
{
  unsigned long t1 = millis();
  int estado = 0;
  int intervalo = 1000;
  while (estado < 3)
  {
    
    if(estado == 0)
      renderizar_Pantalla(tres);
    else if(estado == 1)
      renderizar_Pantalla(dos);
    else 
      renderizar_Pantalla(uno);
      
    if(millis() > t1+ intervalo) // al pasar 1 segundo
    {
      t1 = millis();
      estado = estado + 1; // cambiamos al siguiente estado  
    }
    
  } 
  cuentaR = true; 
}

// ====================================== DETECCION DE BOTON DE PAUSA O CAMBIO DE JUEGO ==============================

void deteccionPausa()
{
  if(digitalRead(pinPausa)) // boton presionado
  {
    if(tiempo_transcurrido == 0)
      tiempo_transcurrido = millis(); // momento exacto en donde se presiono el boton 
      
  }else{ // boton liberado
    
    if(tiempo_transcurrido != 0)
    {
      int t2 = millis();
      if( ((t2 - tiempo_transcurrido)/1000) >= 3) // el boton fue presionado 3 o mas segundos
      {
        // cambia el modo de juego
        if(modo == 1) // actualmente esta en modo mensaje, debe cambiar al juego
        {
          //Serial.println("Cambiar al modo de juego");
          modo = 0;
          
        }else{       //  actualmente esta en modo juego, debe cambiar al mensaje
          
          modo = 1; // pasar a modo mensaje
          // Si colisiono debe reiniciar el juego completamente
          if(colision){
            
            inicio = false; 
            colision = false;
            for(int i = 0; i < 16; i++)
              matriz1[i] = 0;
            jugador[0]= 56;
            jugador[1] = 16;
            contaE = 0;
          }
          
        }
        
      }else{ // El boton fue precionado menos de 3 segundos
          
        if(modo == 0)
        {
          modo = 2 ; // modo pausa
        }
        else if(modo == 2)
        {
          modo=0; // pasar a modo de juego = 0
          cuentaR = false; // para indicar que debe realizar nuevamente la cuenta regresiva
        }   
      }   
      tiempo_transcurrido = 0;
    }
  }  
  
}


// =================== LOGICA DEL JUEGO ROAD FIGHTER ===========

void jugarRoadFighter()
{
  if(!colision)
  {
    // Leer periodicamente
      if(millis() > tt + 150)
      {
        tt = millis();
        lectura = digitalRead(bi);
        lectura2 = digitalRead(bd);
      }
  
      if(lectura == HIGH)
      {
        //Serial.println("botoni pulsado");
        botonIzquierda(jugador);
        lectura = LOW;
      }

      if(lectura2 == HIGH)
      {
        //Serial.println("botond pulsado");
        botonDerecha(jugador);
        lectura2 = LOW;
      }

      if(millis() > t_shift + spawn_enemigo)// mueve al enemigo
      {
          Shift_Derecha();
          t_shift = millis();


          matriz1[0] = nuevo_enemigo[contaE];
      
          contaE++;
          if(contaE > 4) // Debe aparecer un nuevo enemigo
          {
              contaE = 0; //reiniciamos
              // buscamos un nuevo enemigo
              nRandom = random(0,5); // random de 0 - 4
              nuevo_enemigo = get_Enemigo_Random(nRandom);
          }        

      }       
   // Graficar en la matriz
    for(int i = 0; i < 16; i++) 
    {
      if(i<8){ // Driver
        
        lc.setRow(0, i, matriz1[i]);
      
      }else {
        
        PORTC = matriz1[i];
         digitalWrite(fila[i-8],LOW);
          delay(1);
          digitalWrite(fila[i-8],HIGH);
    
       }
    }

    for(int i =0; i < 2; i++) // solo para graficar al jugador
    {
      PORTC = jugador[i];
         digitalWrite(fila[7-i],LOW);
          delay(1);
          digitalWrite(fila[7-i],HIGH);
    }
 
    if(millis() > t_actual + 10000){ // nivel del juego incrementa (cada 10 segundos)
      
      nivelJuego ++;
      t_actual = millis();
      switch(nivelJuego)
      {
        case 1:{spawn_enemigo = 2000;}break;
        case 2:{spawn_enemigo = 1700;}break;
        case 3:{spawn_enemigo = 1400;}break;
        case 4:{spawn_enemigo = 1100;}break;
        case 5:{spawn_enemigo = 900;}break;
        case 6:{spawn_enemigo = 600;}break;
        case 7:{spawn_enemigo = 350;}break;
        case 8:{spawn_enemigo = 250;}break;
        case 9:{spawn_enemigo = 200;}break;  
        default:{spawn_enemigo = 200;}break;
      }  
  
    }

    if(millis() > segundos + 1000){ // para llevar el control de cada segundo
      segundos = millis();
      tiempoJugado++; // incrementamos el conteo del juego actual
    }

    detectarColision(); // true si detecta colision
    
  }else {
    
    // mostar el tiempo que duro la partida   
    stoi_tiempo(tiempoJugado);
  }
}


// =========================================================

void renderizar_Pantalla(byte vector[]) // para la cuenta regresiva
{
  for(int i = 0; i < 16; i++) // modificar el valor de la pantalla ( para que aparezca en ambas pantallas)
  {
    if(i > 3 && i <12){
      pantalla[i] = vector[i-4];
    }else{
      pantalla[i] = 0;
    }
  }
  
  for(int i = 0; i < 16; i++) // Para recorrer el vector pantalla
  {
    if(i < 8)
    { 
          lc.setRow(0, i, pantalla[i]);   
      
    }else{ // i (8 - 15)

            PORTC = pantalla[i]; // escribe todo el byte en columna
    
            digitalWrite(fila[i-8],false);
            delay(1);
            digitalWrite(fila[i-8],true); 
    }
  }
}


//=============================================================================
void reiniciar_pantalla() 
{
  int reinicio[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  for(int i=0; i<16; i++) 
      pantalla[i]= reinicio[i];
}


//===============================================================================
void corrimiento_enPantalla() //Para Agregar +1 a todos los valores internos del vector "pantalla" 
{

  if( millis() > tiempo + velocidad_msg) // Aqui  se determina la velocidad del mensaje
  {
    tiempo += velocidad_msg; // incrementamos nuevamente el tiempo

    if(sentido_movimiento ) // true = abajo
    {
      contador++; // indica que byte de la letra
      for(int i = 15; i > 0; i--) // realizar un shift a la derecha en el vector "pantalla"
      {
        pantalla[i] = pantalla[i-1]; // traslada de izquierda a derecha
      }
      pantalla[0] = contador; // inserta el nuevo valor en la primera posicion
      //contador++; // indica que byte de la letra

    }else { // false = arriba

      contador--; // indica que byte de la letra
      for(int i = 0; i < 15; i++) // realizar un shift a la izquierda en el vector "pantalla"
      {
        pantalla[i] = pantalla[i+1]; // traslada de izquierda a derecha
      }
      pantalla[15] = contador; // inserta el nuevo valor en la ultima posicion
      //contador--; // indica que byte de la letra

    }
    
  }
 
  mostrar_enPantalla(); // SIEMPRE GRAFICAR LO QUE ESTE EN LAS MATRICES

   if( pantalla[15] >= sizeof_palabras) // llego hasta abajo verticalmente...
   { 
     reiniciar_pantalla();
     contador = -1;  
     delay(1000);
     
   } else if(pantalla[0] < -1){ // llego al final arriba verticalmente
      reiniciar_pantalla();
      contador = sizeof_palabras; 
      delay(1000);
   }

}



//===================================================================================
void mostrar_enPantalla() // La pantalla con driver sera la primera, y la pantalla sin driver la segunda
{   
  for(int x=0;  x<16; x++)// para recorrer el vector "pantalla"
  {
    
    if(x < 8)// La primera matriz utiliza las columnas de 0 - 7
    {
  
      if((pantalla[x]>=0 && pantalla[x]<sizeof_palabras)) // Solo ingresa SI y solo si el "valor" interno del vector pantalla esta en el rango (0, 128) 
      {   
          byte b = palabras[pantalla[x]]; // recuperamos el byte de la columna actual   
          lc.setRow(0, x, b);       
      }
      
    }
    else if(x < 16) // La segunda matriz utiliza las columnas de 8 - 15
    {
       if((pantalla[x]>=0 && pantalla[x]<sizeof_palabras)) // los valores que esten adentro del vector pantalla deben estar en el rango establecido
        { 
            int valor = pantalla[x]; // recuperar el valor del vector "pantalla" dependiendo donde esta actualmente la posicion x.  
            byte b = palabras[valor]; // extraemos el byte del vector "palabras" en la posicion donde indique valor.
            
            PORTC = b; // escribe todo el byte en columna
    
            digitalWrite(fila[x-8],false);
            delay(1);
            digitalWrite(fila[x-8],true);   
        }
      
    }      
  }

}

//  ===================================================================
void inicializar_pin() // inicializar multiples pines
{
  pinMode(pinPausa, INPUT);
  pinMode(bd, INPUT);
  pinMode(bi, INPUT);
  
  pinMode(pin_interruptor, INPUT);
  
  for(int pin=22; pin<38; pin++)
     pinMode(pin,OUTPUT);

  for(int pin=22; pin<30; pin++)
    digitalWrite(pin,true);

    for(int pin=30; pin<38; pin++)
    digitalWrite(pin,false);
}






//  =============================================================================
void Modificar_velocidad(int val){
 
  if(val>=0&&val<255){
    velocidad_msg=350;
    //Serial.println("velocidad baja");
  }
  if(val>=255&&val<511){
    velocidad_msg=200;
    //Serial.println("velocidad despues");    
  }
  if(val>=511&&val<767){
     
    velocidad_msg=150;   
    //Serial.println("velocidad media");
  }
  if(val>=767&&val<=1023){
    
    velocidad_msg = 90; 
    //Serial.println("velocidad alta");
  }
   
}//fin del metodo  control_velocidad



// =============================MONITOREAR SENTIDO DE MOVIMIENTO ==================
void monitorear_sentido_movimiento()
{
  monitoreo_intp = digitalRead(pin_interruptor);
   //Serial.println(monitoreo_intp);

   if(actual_intp != monitoreo_intp)
   {
    actual_intp = monitoreo_intp; 
    if(sentido_movimiento) // si va hacia abajo( true )  
    {
      contador = pantalla[15];
      
    }else {
     contador = pantalla[0];
    }
    
    sentido_movimiento = !sentido_movimiento; // para invertir el sentido del movimiento
    
   }
  
}


// ====================================== DETECTAR COLISION ===========================================

void detectarColision()
{ // la colision se puede dar cuando comparamos ambas matrices 
  for(int i = 0; i < 2; i ++)
  {
    byte resultado = matriz1[15-i] & jugador[i] ;
    for(int bit_id = 0; bit_id < 8; bit_id++)
    {
      if(bitRead(resultado,bit_id) == 1) 
      {
        colision = true;
      }
    }
    // 1 0 1 0 1 0 1 0 1 0
    // 1 0 1 0 1 0 1 0 1 1
  }    
}


// ====================================== MOVER CARRO DERECHA ==========================================

void botonDerecha(byte * array_jugador)
{
  if(bitRead(*array_jugador, 0) != 1) // leer el bit menos significativo para ver si ya llego al final
  {
    *array_jugador = *array_jugador>>1; 
    *(array_jugador+1) = *(array_jugador+1)>>1; 
  }else{
    colision =  true;
  }

}

// ===================================== MOVER CARRO IZQUIERDA =======================================

void botonIzquierda(byte * array_jugador)
{
  if(bitRead(*array_jugador, 7) != 1) // leer el bit menos significativo para ver si ya llego al final
  {
    *array_jugador = *array_jugador<<1; 
    *(array_jugador+1) = *(array_jugador+1)<<1; 
  }else{
    colision = true;
  }

}

// ======================================== Generar un enemigo Random ======================================
byte * get_Enemigo_Random(long valRandom)
{
  
  switch(valRandom) // determinar la posicion del enemigo
  {
    case 1:{return enemigo;}break;
    case 2:{return enemigo2;}break;
    case 3:{return enemigo3;}break;
    case 4:{return enemigo4;}break;  
  }  
  return enemigo; // improbable que llegue aqui pero por defecto se pondra
}



// ========================================= CAIDA DE ENEMIGOS ===============================================

void Shift_Derecha(){

  for(int i = 15; i > 0; i--) // realizar un shift a la derecha en el vector "matriz1"
  {
    matriz1[i] = matriz1[i-1]; // traslada de izquierda a derecha
  } 
}
