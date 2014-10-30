/*
  ReadATweet

  Demonstrates retrieving the most recent Tweet from a user's home timeline 
  using the Temboo Arduino Yun SDK.
  
  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
//ticker
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.
const String TWITTER_ACCESS_TOKEN = "2451645601-MMNHrpCO51ENZaKHllmpXwQGmN8OUn0c7R5olRn";
const String TWITTER_ACCESS_TOKEN_SECRET = "y8xuJtHWW44dL6T7YYv16mS2XP1xL9WmkUjKdOguUzPNV";
const String TWITTER_API_KEY = "mGJTg6WKpimcQuLprNPv17GFZ";
const String TWITTER_API_SECRET = "nrMP7rMKOuiiGJ7nfM4groaP6iTbCW330Qmdn8tlyCvbV61l91";

int numRuns = 1;   // execution count, so this doesn't run forever
int maxRuns = 1000;   // the max number of times the Twitter HomeTimeline Choreo should run

//ticker
int pinCS = 12; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 5;
int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String newTweet= "";
//String prevTweet= "";
int wait = 60; // In milliseconds

int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels




void setup() {
  matrix.setIntensity(1); // Use a value between 0 and 15 for brightness <-----ticker
  
  Serial.begin(9600);
  //Serial.println("ok");
  
  // For debugging, wait until a serial console is connected.
  delay(4000);
  while(!Serial);
  Bridge.begin();
}
void loop()
{
  // while we haven't reached the max number of runs...
  if (numRuns <= maxRuns) {
    //Serial.println("Running ReadATweet - Run #" + String(numRuns++));
    
    TembooChoreo MentionsChoreo;

    // invoke the Temboo client.
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    MentionsChoreo.begin();
    
    // set Temboo account credentials
    MentionsChoreo.setAccountName(TEMBOO_ACCOUNT);
    MentionsChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    MentionsChoreo.setAppKey(TEMBOO_APP_KEY);

    // tell the Temboo client which Choreo to run (Twitter > Timelines > HomeTimeline)
    MentionsChoreo.setChoreo("/Library/Twitter/Timelines/Mentions");
    
    
    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Timelines/HomeTimeline/
    // for complete details about the inputs for this Choreo

    MentionsChoreo.addInput("Count", "1"); // the max number of Tweets to return from each request
    MentionsChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    MentionsChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    MentionsChoreo.addInput("ConsumerKey", TWITTER_API_KEY);    
    MentionsChoreo.addInput("ConsumerSecret", TWITTER_API_SECRET);

    // next, we'll define two output filters that let us specify the 
    // elements of the response from Twitter that we want to receive.
    // see the examples at http://www.temboo.com/arduino
    // for more on using output filters
   
    // we want the text of the tweet
    MentionsChoreo.addOutputFilter("tweet", "/[]/text", "Response");
    
    // and the name of the author
    MentionsChoreo.addOutputFilter("author", "/[]/user/screen_name", "Response");


    // tell the Process to run and wait for the results. The 
    // return code will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = MentionsChoreo.run();
    
   // a response code of 0 means success; print the API response
    if(returnCode == 0) {
      
      String author; // a String to hold the tweet author's name
      String tweet; // a String to hold the text of the tweet


      // choreo outputs are returned as key/value pairs, delimited with 
      // newlines and record/field terminator characters, for example:
      // Name1\n\x1F
      // Value1\n\x1E
      // Name2\n\x1F
      // Value2\n\x1E      
      
      // see the examples at http://www.temboo.com/arduino for more details
      // we can read this format into separate variables, as follows:
      
      while(MentionsChoreo.available()) {
        // read the name of the output item
        String name = MentionsChoreo.readStringUntil('\x1F');
        name.trim();

        // read the value of the output item
        String data = MentionsChoreo.readStringUntil('\x1E');
        data.trim();

        // assign the value to the appropriate String
        if (name == "tweet") {
          tweet = data;
        } else if (name == "author") {
          author = data;
        }
      }
     
     //ticker 
      newTweet=("@" + author + " - " + tweet);

        for ( int i = 0 ; i < width * newTweet.length() + matrix.width() - 1 - spacer; i++ ) 
        {

            matrix.fillScreen(LOW);
        
            int letter = i / width;
            int x = (matrix.width() - 1) - i % width;
            int y = (matrix.height() - 8) / 2; // center the text vertically
        
            while ( x + width - spacer >= 0 && letter >= 0 ) {
              if ( letter < newTweet.length() ) {
                matrix.drawChar(x, y, newTweet[letter], HIGH, LOW, 1);
              }
        
              letter--;
              x -= width;
            }
        
            matrix.write(); // Send bitmap to display
        
            delay(wait);
          }

        }
    
     else {
      // there was an error
      // print the raw output from the choreo
      while(MentionsChoreo.available()) {
        char c = MentionsChoreo.read();
        Serial.print(c);
      }
    }

    MentionsChoreo.close();

  }

  //Serial.println("Tweet @KasilStudio");
  delay(61000); // wait 61 seconds between HomeTimeline calls
}
