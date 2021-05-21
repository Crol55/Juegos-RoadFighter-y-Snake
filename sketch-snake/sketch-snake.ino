#include "LedControl.h"
#include "juegoSnake.h"


LedControl lc = LedControl(13,11,12,2);

//======== Variables Globales
// snake
snake *serpiente;
unsigned long tiempo_snake = 0;
int intervalo = 875; // movimiento del juego
long randomNumber;

// pines
int pinArriba = 10;
int pinAbajo = 8;
int pinDerecha = 9;
int pinIzquierda = 7;
// var del programa
char modo_juego; // m:mensaje, s:snake

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  /*1er Matriz*/
  lc.shutdown(0, false);
  lc.setIntensity(0,4);
  lc.clearDisplay(0);
  /*2da Matriz*/
  lc.shutdown(1, false);
  lc.setIntensity(1,4);
  lc.clearDisplay(1);

  //======== Inicializar pines ======  
  pinMode(pinArriba,INPUT); 
  pinMode(pinDerecha,INPUT);
  pinMode(pinAbajo,INPUT);
  pinMode(pinIzquierda,INPUT);
 // ======== Inicializar variables del snake ==========
  direccionM = 'R'; // El movimiento inicial sera hacia la derecha (right)
  serpiente = new snake(); // Unica instancia del snake
 // ======== Inicializar variables del programa =======
  modo_juego = 'm'; // Inicia en modo mensaje.
}


void loop() {
  // put your main code here, to run repeatedly:
  if(modo_juego == 'm'){
    if(millis() > 1000){
      modo_juego = 's';
      randomSeed(millis()); // para que los enemigos sean RANDOM
    }
  }else{
    for(int i = 0; i < 16; i++)
    {
      if(i <8)
        lc.setColumn(0,i,pantallaSnake[i]);
      else
      lc.setColumn(1,i-8,pantallaSnake[i]);
    }
  
    // leer direccion de movimient
    if(digitalRead(pinArriba) && (direccionM != 'D'))
      direccionM = 'U';// up
    else if(digitalRead(pinDerecha) && (direccionM != 'L'))
      direccionM = 'R';// Right
    else if(digitalRead(pinAbajo) && (direccionM != 'U'))
      direccionM = 'D'; // Down
    else if(digitalRead(pinIzquierda) && (direccionM != 'R'))
      direccionM = 'L'; // Left
  
    if(millis() > tiempo_snake + intervalo){
    
      tiempo_snake = millis();
      int estado = 0; // 0:El juego sigue normal, 1:Generar vida/comida, 2:El jugador perdio
      switch(direccionM){ // Que boton pulso
        case 'U':{estado = movVertical(serpiente, 'U'); }break;
        case 'R':{estado = movHorizontal(serpiente,'R');}break;
        case 'D':{estado = movVertical(serpiente, 'D');}break;
        case 'L':{estado = movHorizontal(serpiente,'L');}break; //estado = movHorizontal(serpiente,'L'); movIzquierda(serpiente);
      }
      switch(estado){
        case 1:{ 
          
          colocar_vida(serpiente); // implica que comio
          intervalo = intervalo-5; // incrementar la velocidad del snake
        }break;
        case 2:{ // perdio el juego
          Serial.println("Terminar juego");
          //Serial.println(serpiente->get_comidaIngerida());
          // Si desea volver a jugar debemos reiniciar todas las variables de snake y luego retornar al mensaje.
          serpiente = new snake();
          reiniciar_snake();
          modo_juego = 'm';
          
        }break;
      }  
    }
  }  
}
