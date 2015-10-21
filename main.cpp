#include "mbed.h"
#include "motordriver.h"
#include "Keypad.h"
//Motor A(p21,p7,p8,1); //pwm, fwd, rev, can brake
Serial PC(USBTX, USBRX);
DigitalOut forward(p7);
DigitalOut reverse(p8);
PwmOut PWM1(p22);
PwmOut Servo(p26);
InterruptIn trigger(p25);
Timer t;
AnalogIn request(p20);

/*
char Keytable[] = {
    '1','2','3','A',
    '4','5','6','B',
    '7','8','9','C',
    '*','0','#','D'
};
*/
int freq[] = {100,250,500,700,1000};
int32_t Index = -1;
int floorFreq;
int currentFreq;
int begin;
int end;
int pulseWidth;
int edge = 2;
int targetFloor;

uint32_t cbAfterInput(uint32_t index) {
     Index = index;
     return 0;
 }

void flip(){
    pulseWidth = t.read_us();
    currentFreq = (int)((1/(float)pulseWidth)*1000000);
    t.reset();
}
int main() {
    forward = 1;
    reverse = 0;
    PWM1.period(0.020);
    PWM1 = 0.8;
    PC.printf("PWM1:%f\n", PWM1.read());
    Servo.period(0.020);
    Servo = 0.00;
    PC.printf("Hello World");
    
    Keypad keypad(p15, p14, NC, NC, p18, p17, p16, NC);
    keypad.attach(&cbAfterInput);
    keypad.start();
    
    trigger.mode(PullUp);
    trigger.fall(&flip);
    t.start();
    
    //floorFreq = currentFreq;
    
    while (1) {
        //PWM1 = 0.1125;
        
        /*detect current frequency*/
        //wait(3);
        PC.printf("current frequency: %d Hz\n", currentFreq);
        
        /*process key press signal*/       
        if(Index > -1){
            PC.printf("Index:%d\n", Index);
            targetFloor = Index+1;
            floorFreq = freq[Index];
            PC.printf("target freq:%d\n", floorFreq);
            Index = -1;
        }
        
        /*process analog in signal*/
        //PC.printf("request: %f\n", request.read());
        if(request < 0.4 && request > 0.2){floorFreq = 100;}
        else if(request < 0.6 && request > 0.4){floorFreq = 250;}
        else if(request < 0.8 && request > 0.6){floorFreq = 500;}
        else if(request < 1.0 && request > 0.8){floorFreq = 700;}
        else if(request - 1.0 < 0.1){floorFreq = 1000;}
        
        /*determine direction*/
        if (abs(floorFreq-currentFreq)<10){ //stop
            //PWM1 = 0;
            forward = 0;
            reverse = 0;
            Servo.period(0.020);
            Servo = 0.05;
            //wait(5);
            //Servo.period(0.020);
            //Servo = 0.1;
            PC.printf("Stop.\n");
            PC.printf("target freq:%d\n", floorFreq);
            
        }        
        else if((floorFreq/10) > (currentFreq/10)){//go upwards
            Servo.period(0.020);
            Servo = 0.1;
            //wait(1);
            forward = 1;
            reverse = 0;
    
            PC.printf("Going upwards.\n");
            PC.printf("target freq:%d\n", floorFreq);
            
        }
        else if((floorFreq/10) < (currentFreq/10)){//go downwards
            Servo.period(0.020);
            Servo = 0.1;
            //wait(1);
            forward = 0;
            reverse = 1;
         
            PC.printf("Going downwards.\n");
            PC.printf("target freq:%d\n", floorFreq);
            
        }
    }
       
}



