/* Let's define a CRGB "string" as starting with a C,  
 *  then having 3 comma separates values... 
 *  where the first value is the R value, second the G, third the B.   
 *  Those values can be 1, 2, or 3 digits.   
 *   
 *  Some examples: 
"C100,0,0" just red 
"C255,255,255" bright white 
"C1,0,1" really dim purple 

Write a function that takes a CRGB string,  
and returns a CRGB number based on the three numbers in that string.   
If the string is badly formed, return an all black CRGB (0,0,0).   
This includes all strings that *DON'T* start with a C...like: 
"G1,1,1" 

if there is stuff AFTER the CRGB string, ignore it.  So: 

"C10,20,30Hello" should still give you a CRGB with 10,20,30.  

Note:  you'll need to include FastLED.h in this one... 

Prototype: 

CRGB convertCRGBString(char *input ) 

*/ 

#include "SoftwareSerial.h" 
#include "FastLED.h" 
CRGB leds; 

char input_1[]="C130,25,56"; 
char input_2[] ="C255,255,255"; 
char input_3[]="C1,0,1"; 
char input_4[] ="G1,1,1"; 
char input_5[] ="C10,20,30Hello"; 
char input_6[] ="pizzaC0,99,256donkey"; 

typedef enum
{
  LOOK_FOR_C,
  BUILD_STRING
} parse_state_type;

parse_state_type parse_state = LOOK_FOR_C;

#define BUFF_SIZE 12
char myBuffer[BUFF_SIZE];
int buffer_index=0;


void setup( ) 
{ 
  Serial.begin(9600); 

#if 0
  leds = CRGB(convertCRGBString(input_5)); 
  Serial.println(); 
  Serial.println("CRGB color value");  
  Serial.print("R = "); 
  Serial.print(leds[1].red); 
  Serial.print(" G = "); 
  Serial.print(leds[1].blue);   
  Serial.print(" B = "); 
  Serial.print(leds[1].green);   
#endif

  buffer_index = 0;
  parse_state = LOOK_FOR_C;
} 

void loop ( ) 
{ 
   char c;
   CRGB light_value;

   while (Serial.available())
   {
     c = Serial.read();

     if (c == 'C')
     {
       // This is (potentially) the start of a CRGB string.  Store that 'C' at the 
       // beginning of our buffer.
       myBuffer[0] = c;

       // get ready to collect the other characters.
       buffer_index = 1;
       parse_state = BUILD_STRING;

       Serial.println("** Found a C.  Starting to build");
       
     }
     else if (parse_state == BUILD_STRING)
     {
       Serial.print("** Building string, index ");
       Serial.print(buffer_index);
       Serial.print(" , char=");
       Serial.println(c);
       
       myBuffer[buffer_index] = c;

       // try what we've got so far...
       light_value = convertCRGBString(myBuffer);

       if (light_value)
       {
         Serial.println("FOUND VALID CRGB!!!");

         //go back to looking for a C...
         parse_state = LOOK_FOR_C;
         buffer_index = 0;
       }
       else
       {
         // no valid CRGB (at least yet).  Did we run out of characters?
         buffer_index++;
         if (buffer_index >= BUFF_SIZE)
         {
           // We've seen too many characters.  This can't be a CRGB.
           parse_state = LOOK_FOR_C;
           buffer_index = 0;

           Serial.println("** Too many chars.  back to looking for C");
         }
         
       }
     }
   }
} 

CRGB convertCRGBString(char *input ) 
{ 
  int Rvalue = 0; 
  int Gvalue = 0; 
  int Bvalue = 0; 
  int CommaCount = 0; 
  bool Cfound = false; 
  bool RvalueGood = false; 
  bool GvalueGood = false; 
  bool BvalueGood = false; 
  while (*input) // while anything is available to read from the passed string 
  { 
    if (*input == 'C') // if the current char in the string is 'C' 
    {  
      Cfound = true; 
      Serial.println("C found!"); 
      input++; 
    }  
    if (*input == ',') 
    { 
       CommaCount++; 
       Serial.println("Comma found"); 
       input++; 
    }   
    if ((makeCharAnInt(*input) >= 0) &&  
              (makeCharAnInt(*input) <= 9) && (Cfound == true))    
    { 
        if (CommaCount == 0) 
          {   
            Rvalue = Rvalue * 10; 
            Rvalue = Rvalue + makeCharAnInt(*input);   
          }  
                     
        else if (CommaCount == 1) 
          {     
            Gvalue = Gvalue * 10; 
            Gvalue = Gvalue + makeCharAnInt(*input);   
          }  

        else if (CommaCount == 2) 
          { 
            Bvalue = Bvalue * 10; 
            Bvalue = Bvalue + makeCharAnInt(*input);  
          }  
        input++;    
     } 
     else 
        input++; 
   } 
   if (Rvalue <= 255) 
      RvalueGood = true; 
   if (Bvalue <= 255) 
      BvalueGood = true; 
   if (Gvalue <= 255) 
      GvalueGood = true; 
                   
  Serial.println(); 
  Serial.print ("CommaCount = "); 
  Serial.println(CommaCount); 
  Serial.print ("Rvalue = "); 
  Serial.println(Rvalue); 
  Serial.print ("Gvalue = "); 
  Serial.println(Gvalue); 
  Serial.print ("Bvalue = "); 
  Serial.println(Bvalue); 
  Serial.print("Cfound? = "); 
  Serial.println(Cfound); 
       
  if (Cfound && (CommaCount >= 2) && RvalueGood && BvalueGood && GvalueGood )  
    return CRGB (Rvalue, Bvalue, Gvalue); 
  else  
    return CRGB (0,0,0); 
} 

int getRValue( char *input ) 
{ 
  int Rvalue = 0; 
  bool Rfound = false; 
  while ((*input) && (!Rfound)) // while anything is available to read from the passed string 
  { 
    if (*input == 'R') // if the current char in the string is 'R' 
    {  

      input++; 
      while ((makeCharAnInt(*input) >= 0) && //current char is digit between 0-9 
              (makeCharAnInt(*input) <= 9)) 
      { 
        Rfound = true;  // R has been found in the string 
        Rvalue = Rvalue * 10; 
        Rvalue = Rvalue + makeCharAnInt(*input); 
        input++; 
      }    
    } 
     input++; 
  } 
   
  if (Rfound) // return Rvalue if there is one, otherwise error code -1 
    return Rvalue; 
  else  
    return -1; 
} 

int makeCharAnInt( char c )  
{ 
   int newInteger; 
   if ((c >= 48) && (c <= 57))  
      { 
         newInteger = c - 48; 
         return newInteger; 
      } 
   else  
      { 
          return -1; 
      }     
}   

\
