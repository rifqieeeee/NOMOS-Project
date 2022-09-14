/*YF- S201 water Flow sensor code for SenseCAP K1100 */
const int Output_Pin = D0;
volatile int  Pulse_Count;
unsigned int  Liter_per_hour;
unsigned long Current_Time, Loop_Time;

void setup()
{ 
   Serial.begin(115200); 
   pinMode(Output_Pin, INPUT);
   attachInterrupt(0, Detect_Rising_Edge, RISING);
                                     
   Current_Time = millis();
   Loop_Time = Current_Time;
} 

void loop ()    
{
   Current_Time = millis();
   if(Current_Time >= (Loop_Time + 1000))
   {
      Loop_Time = Current_Time;
      Liter_per_hour = (Pulse_Count * 60 / 7.5);
      Pulse_Count = 0;
      Serial.print(Liter_per_hour, DEC);
      Serial.println(" Liter/hour");
   }
}
void Detect_Rising_Edge ()
{ 
   Pulse_Count++;
} 
