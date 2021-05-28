#ifndef BUZZER_H
#define BUZZER_H

//Buzzer plays given frequency
void set_PWM(double frequency);

//Initialize Buzzer
void PWM_on();

//Turn Buzzer off
void PWM_off();

#endif