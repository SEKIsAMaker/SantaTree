/*
testing the modules of the project SantaTree.
*/

#include<time.h>
#include<stdlib.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

//3mm leds for indication
int routine1_indicator= 12;
int routine2_indicator= 13;

//buttons
int nb_motor_button_pushes = 0;
int nb_rgb_button_pushes = 0;

//stepper motor
int motor_pin[4] = {2, 4, 7, 8};

//RGB leds
int R[2]={3, 9};
int G[2]={5, 10};
int B[2]={6, 11};

const int nb_colors = 7;
int white[3] = {255, 255, 255};
int red[3] = {255, 0, 0};
int green[3] = {0, 255, 0};
int blue[3] = {0, 0, 255};
int yellow[3] = {255, 255, 0};
int magenta[3] = {255, 0, 255};
int cyan[3] = {0, 255, 255};
int* colors[nb_colors] = {red, magenta, yellow, white, blue, green, cyan};

void setup(){
  
  Serial.begin(9600);
  srand(time(NULL));
  
  pinMode(routine1_indicator, OUTPUT);
  pinMode(routine2_indicator, OUTPUT);
  for(int i=0; i<2; i++){
    pinMode(R[i], OUTPUT);
    pinMode(G[i], OUTPUT);
    pinMode(B[i], OUTPUT);
  }
  for(int i=0; i<4; i++)
    pinMode(motor_pin[i], OUTPUT);

  for(int i=0; i<2; i++)
    set_led_color( R[i], G[i], B[i], white, 255);
    
}

void motor_write(int rotation_values[]){
  for(int i=0; i<4; i++)
    digitalWrite(motor_pin[i], rotation_values[i]);
}

void one_motor_step(int motor_delay){
  int rotation_values[8][4] = {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, 
                              {0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1} };
  int stop[4] = {0, 0, 0, 0};
  for(int i=0; i<8; i++){
    if( nb_motor_button_pushes%2 == 0 )
      motor_write( rotation_values[i] );
    else
      motor_write(stop);
    listen();
    delay(motor_delay);
  }
}


void set_led_color(int rPin, int gPin, int bPin, int color[], int max_light_intensity){

  int R = 255 - min(color[0], max_light_intensity);
  int G = 255 - min(color[1], max_light_intensity);
  int B = 255 - min(color[2], max_light_intensity);

  analogWrite(rPin, R);
  analogWrite(gPin, G);
  analogWrite(bPin, B);
 }

void led_routine_1(){

/*
Each set of leds has a different color.
Light flashes in decreasing time intervals.
*/
  Serial.println(SP, HEX);
  Serial.println(" at the top of led_routine1\n");

  Serial.print("Routine1,\nnb_rgb_button_pushes= ");
  Serial.print(nb_rgb_button_pushes);
  Serial.print("\nnb_motor_button_pushes= ");
  Serial.print(nb_motor_button_pushes);
  Serial.println();
  Serial.println();

//  Serial.print("Stack size: ");  Serial.println(RAMEND - SP);
//  Serial.println();

  int color_duration[4] = {2000, 1000, 500, 100};
  for(int d=0; d<4; d++){
    //estimate the necessary number of motor steps in order 
    //to keep the rgb color ON for color_duration[d] milliseconds
    int nb_motor_steps = int(color_duration[d]/8); 
    
    for(int c=0; c<nb_colors; c++){
      //set rgb leds
      set_led_color( R[0], G[0], B[0], colors[c], 255);
      set_led_color( R[1], G[1], B[1], colors[nb_colors - c - 1], 255);
      
      //while waiting, rotate motor and listen for button pushes
      for(int i=0; i<nb_motor_steps; i++)
        one_motor_step(1);
    }
    if(d==3){
      for(int k=0; k<10; k++){
        set_led_color( R[0], G[0], B[0], white, 255);
        set_led_color( R[1], G[1], B[1], white, 255);
  
        for(int i=0; i<nb_motor_steps; i++)
          one_motor_step(1);
        }
      }
  }
  Serial.print("End of routine1.\n\n");
}

void led_routine_2(){
/*
All leds have the same color, which is chosen randomly.
Light intensity increases gradually.
*/

  Serial.println(SP, HEX);
  Serial.println("at the top of led_routine2");
  
  Serial.print("Routine2,\nnb_rgb_button_pushes= ");
  Serial.print(nb_rgb_button_pushes);
  Serial.print("\nnb_motor_button_pushes= ");
  Serial.print(nb_motor_button_pushes);
  Serial.println();
  Serial.println();

  Serial.print("Stack size: ");  Serial.println(RAMEND - SP);
  Serial.println();

  int max_light_intensity = 256;
  int min_light_intensity = 25;
  int color_duration = 500;
  int nb_motor_steps = int(color_duration/8);
  
  //randomly choose a color
  int color_index = rand()%nb_colors;

  for(int l=min_light_intensity; l<max_light_intensity; l+=25){
    for(int j=0; j<2; j++)
      set_led_color( R[j], G[j], B[j], colors[color_index], l);
    for(int m=0; m<nb_motor_steps; m++)
      one_motor_step(1);
  }
  Serial.print("End of routine2.\n\n");
}

void listen(){
  int buttonState = analogRead(A1);
  if(buttonState > 800 && buttonState < 850 ){
    while(buttonState > 800 && buttonState < 850 ){
      buttonState = analogRead(A1);
      Serial.println();
      Serial.print(analogRead(A1));
      Serial.println();
      }
    Serial.print("Button1");
    Serial.println();
    nb_rgb_button_pushes++;
    if(nb_rgb_button_pushes%2==0){
      digitalWrite(routine1_indicator, HIGH);
      digitalWrite(routine2_indicator, LOW); 
      led_routine_1();
    }
    else{
      digitalWrite(routine1_indicator, LOW);
      digitalWrite(routine2_indicator, HIGH);
      led_routine_2(); 
      Serial.println("in listen after led_routine2()");
      Serial.println(SP, HEX);
    }
  }
  else{
    if( buttonState > 900 ){
      while(buttonState > 900 ){
        buttonState = analogRead(A1);
        Serial.println();
        Serial.print(analogRead(A1));
        Serial.println();
        }
      Serial.print("Button2");
      Serial.println();
      nb_motor_button_pushes++;
      
    }
  }
}


void loop(){
  Serial.print("\nLoop Start,\nnb_rgb_button_pushes= ");
  Serial.print(nb_rgb_button_pushes);
  Serial.print("\nnb_motor_button_pushes= ");
  Serial.print(nb_motor_button_pushes);
  Serial.println();
  Serial.println();

//  Serial.print("Stack size: ");  Serial.println(RAMEND - SP);
//  Serial.println();
  
  if( nb_rgb_button_pushes%2==0 ){
    digitalWrite(routine1_indicator, HIGH);
    digitalWrite(routine2_indicator, LOW);
    led_routine_1();
  }
  if( nb_rgb_button_pushes%2==1 ){
    digitalWrite(routine1_indicator, LOW);
    digitalWrite(routine2_indicator, HIGH);
    led_routine_2();
    Serial.println(" in loop after led_routine2()");
    Serial.println(SP, HEX);
  }
}
