const int p1 = 7;
const int p2 = 8;

const int button = 12;

const int r = 3;
const int g = 5;
const int b = 6;

const int white[3] = {255, 255, 255};
const int red[3] = {255, 0, 0};
const int green[3] = {0, 255, 0};
const int blue[3] = {0, 0, 255};

int buttonState = 0;
int nb_pushes = 0;

void setup() {
  //for printing on the console
  Serial.begin(9600);

  // indicator leds
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);

  //RGB Led
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);

  //push button
  pinMode(button, INPUT);
}

void setProgIndicator(int prog_number){
    if(prog_number == 1){
      digitalWrite(p1, HIGH);
      digitalWrite(p2, LOW);
    }
    if(prog_number == 2){
      digitalWrite(p2, HIGH);
      digitalWrite(p1, LOW);
    }
  }

void prog1(){
  Serial.print("prog1\n");
  setProgIndicator(1);
  setColor(red);
  delay(100);
}

void prog2(){
  Serial.print("prog2\n");
  setProgIndicator(2);
  setColor(blue);
  delay(100);
}

void setColor(int color[]){
  if(digitalRead(button) == HIGH){

    while( digitalRead(button)== HIGH )
    {
      }

    nb_pushes++;
    Serial.print(nb_pushes);
    Serial.println();

    if(nb_pushes%2 == 0)
      prog1();
    else
      prog2();
  }
  else{
    Serial.print(nb_pushes);
    Serial.println();
    int R = 255 - color[0];
    int G = 255 - color[1];
    int B = 255 - color[2];
    analogWrite(r, R);
    analogWrite(g, G);
    analogWrite(b, B);
  }
}

void loop() {

  if(nb_pushes%2 == 0)
    prog1();
  else
    prog2();

}
