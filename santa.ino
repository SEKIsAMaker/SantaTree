/*
Author: Salwa EL KADDAOUI
*/

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef enum{
  GOTOLOOP,
  STAY
  } flag;


/*-------------------------------------------------------------------*/
/*----------------------------- SETUP -------------------------------*/
/*-------------------------------------------------------------------*/

//3mm leds for indicating the mode of the lights effects
uint8_t lightsmode1_indicator= 12;
uint8_t lightsmode2_indicator= 13;

//number of pushes on each button
unsigned long nbpushes_motorbutton = 0;
unsigned long nbpushes_lightsbutton = 0;

//stepper motor pins
uint8_t motor_pin[4] = {2, 4, 7, 8};

//pins for RGB leds
uint8_t R[2]={3, 9};
uint8_t G[2]={5, 10};
uint8_t B[2]={6, 11};

//light colors for rgb leds
const uint8_t nb_colors = 7;
uint8_t OFF[3]={0, 0, 0};
uint8_t white[3] = {255, 255, 255};
uint8_t red[3] = {255, 0, 0};
uint8_t green[3] = {0, 255, 0};
uint8_t blue[3] = {0, 0, 255};
uint8_t yellow[3] = {255, 255, 0};
uint8_t magenta[3] = {255, 0, 255};
uint8_t cyan[3] = {0, 255, 255};
uint8_t* colors[nb_colors] = {red, magenta, yellow, white, blue, green, cyan};


void setup(){

  Serial.begin(9600);
  randomSeed(analogRead(A0));

  pinMode(lightsmode1_indicator, OUTPUT);
  pinMode(lightsmode2_indicator, OUTPUT);
  for(uint8_t i=0; i<2; i++){
    pinMode(R[i], OUTPUT);
    pinMode(G[i], OUTPUT);
    pinMode(B[i], OUTPUT);
  }
  for(uint8_t i=0; i<4; i++)
    pinMode(motor_pin[i], OUTPUT);

  for(uint8_t i=0; i<2; i++)
    set_rgb_light( R[i], G[i], B[i], white, 255);

}

/*-------------------------------------------------------------------*/
/*----------------------------- MOTOR -------------------------------*/
/*-------------------------------------------------------------------*/

void motor_write(bool motor_coil_pulses[]){
  for(uint8_t i=0; i<4; i++)
    digitalWrite(motor_pin[i], motor_coil_pulses[i]);
}

flag one_motor_step(uint16_t motor_delay){
  /*
  Performs an angle increment of the stepper motor
  */

  //a sequence of current pulses we apply to the pins
  //of the motor in this order to rotate it one step
  bool motor_coil_pulses[8][4] = {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0},
                              {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 1},
                              {0, 0, 0, 1}, {1, 0, 0, 1} };
  bool motor_stop[4] = {0, 0, 0, 0};

  //for each coil pulse
  for(uint8_t i=0; i<8; i++){

    //check the number of pushes on motor button to know
    //wether to stop the motor or keep rotating it
    if( nbpushes_motorbutton%2 == 0 )
      motor_write( motor_coil_pulses[i] );
    else
      motor_write(motor_stop);

    //before applying the delay check if there's
    //a button event that requires to go to loop
    flag listening_status = listen();
    if(listening_status == GOTOLOOP)
      return GOTOLOOP;
    else
      delay(motor_delay);
  }
  return STAY;
}

/*-------------------------------------------------------------------*/
/*---------------------------- RGB Leds -----------------------------*/
/*-------------------------------------------------------------------*/

void set_rgb_light(uint8_t rPin, uint8_t gPin, uint8_t bPin, uint8_t color[], uint8_t max_light_intensity){
  /*
  Set the light color of a common anode rgb Led.
  */

  uint8_t R = 255 - min(color[0], max_light_intensity);
  uint8_t G = 255 - min(color[1], max_light_intensity);
  uint8_t B = 255 - min(color[2], max_light_intensity);

  analogWrite(rPin, R);
  analogWrite(gPin, G);
  analogWrite(bPin, B);
 }

void lights_mode1(){
  /*
  Each set of RGB leds is set to a different color.
  Light flashes in decreasing time intervals.
  */

  //set the 3mm leds to indicate which mode of light effects is being run
  digitalWrite(lightsmode1_indicator, HIGH);
  digitalWrite(lightsmode2_indicator, LOW);

  //perform RGB light animations while rotating the motor
  //(or not, depending on the events on the motor button):

  uint16_t RGBlight_pulseDuration[4] = {1000, 500, 300, 100};

  for(uint8_t d=0; d<4; d++){
    //calculate the number of motor steps we need to do sequentially
    //so as to keep the rgb color ON for RGBlight_pulseDuration[d] milliseconds
    //(a motor step lasts 8 milliseconds)
    uint16_t nb_motor_steps = uint16_t(RGBlight_pulseDuration[d]/8);

    //for each color c
    for(uint8_t c=0; c<nb_colors; c++){

      //set the first set of rgb leds to color c
      set_rgb_light( R[0], G[0], B[0], colors[c], 255);

      //set the second set of rgb leds to a color that's complementary to color c
      set_rgb_light( R[1], G[1], B[1], colors[nb_colors - c - 1], 255);

      //rotate the motor while listening for button pushes
      for(uint16_t i=0; i<nb_motor_steps; i++){

        flag motor_status = one_motor_step(1);

        //motor_status is a flag that indicates what to do in case a button
        //was pushed during motor rotation
        if(motor_status == GOTOLOOP)
          return;
        }
    }
    if(d==3){
      for(uint8_t k=0; k<10; k++){
        set_rgb_light( R[0], G[0], B[0], white, 255);
        set_rgb_light( R[1], G[1], B[1], white, 255);

        for(uint16_t i=0; i<nb_motor_steps; i++){
          flag motor_status = one_motor_step(1);
          if(motor_status == GOTOLOOP)
            return;
          }

        set_rgb_light( R[0], G[0], B[0], OFF, 255);
        set_rgb_light( R[1], G[1], B[1], OFF, 255);

        for(uint16_t i=0; i<nb_motor_steps; i++){
          flag motor_status = one_motor_step(1);
          if(motor_status == GOTOLOOP)
            return;
          }
        }
      }
  }
}

void lights_mode2(){
/*
All leds have the same color, which is chosen randomly.
Light intensity increases gradually.
*/
  //set the 3mm leds to indicate which mode of light effects is being run
  digitalWrite(lightsmode1_indicator, LOW);
  digitalWrite(lightsmode2_indicator, HIGH);

  uint8_t max_light_intensity = 255;
  uint8_t min_light_intensity = 25;
  uint16_t RGBlight_pulseDuration = 50;
  uint16_t nb_motor_steps = uint16_t(RGBlight_pulseDuration/8);

  //from the 7 colors we defined at the beginning of the program,
  //pick one randomly
  uint8_t color_index = rand()%nb_colors;

  //while gradually increasing RGB light intensity
  for(uint8_t l=min_light_intensity; l<max_light_intensity; l+=10){

    //set the 2 sets of RGB leds to the picked color
    for(uint8_t j=0; j<2; j++)
      set_rgb_light( R[j], G[j], B[j], colors[color_index], l);

    //keep rotating the stepper motor until RGBlight_pulseDuration is elapsed
    for(uint16_t m=0; m<nb_motor_steps; m++){

      //if a button event occurs during a motor step
      //go to loop() so that we can handle the event
      flag motor_status = one_motor_step(1);
      if(motor_status == GOTOLOOP)
        return;
    }
  }
}


/*-------------------------------------------------------------------*/
/*---------------------------- PUSH BUTTONS -------------------------*/
/*-------------------------------------------------------------------*/

flag listen(){
  /*
  Catch button presses
  Each button returns a specific range of values in analog input A1
  */
  int buttonState = analogRead(A1);
  if(buttonState > 800 && buttonState < 850 ){

    //this range is particular to the lights button
    //wait until the user finishes the press
    while(buttonState > 800 && buttonState < 850 ){
      buttonState = analogRead(A1);
      }

    //increment the global variable that stores the number
    //of pushes on the lights button
    nbpushes_lightsbutton++;

    //return from all functions until you reach the loop()
    //where we decide which light effects mode to trigger
    //depending on the value of nbpushes_lightsbutton
    return GOTOLOOP;
  }
  else{
    if( buttonState > 900 ){
      while(buttonState > 900 ){
        buttonState = analogRead(A1);
        }
      nbpushes_motorbutton++;
      return STAY;
    }
  }
}


/*------------------------------------------------------------------*/
/*----------------------------- LOOP -------------------------------*/
/*------------------------------------------------------------------*/

void loop(){
  if( nbpushes_lightsbutton%2==0 )
    lights_mode1();
  else
    lights_mode2();

}
