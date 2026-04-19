const int MISO_PIN = 25;  // Enter the MISO pin number here
const int MOSI_PIN = 32;  // Enter the MOSI pin number here
const int SCLK_PIN = 26;  // Enter the SCLK pin number here

void setup() {
  // put your setup code here, to run once:
  pinMode(SCLK_PIN, INPUT);
  pinMode(MISO_PIN, OUTPUT);
  pinMode(MOSI_PIN, INPUT);

  digitalWrite(MISO_PIN, LOW);
  digitalWrite(MOSI_PIN, LOW);
  digitalWrite(SCLK_PIN, LOW);

  Serial.begin(115200);
  Serial.println("\n\n");
}

#define STATE_IDLE_SETUP -1
#define STATE_IDLE  0
#define STATE_RX1   1
#define STATE_OP    6
#define STATE_RX2   2
#define STATE_TX_SETUP  3
#define STATE_TX    4

#define TIMEOUT_THRESHOLD 5000
#define TX_PERIOD         750
#define RXBITS      4

#define OP_MULT 0
#define OP_ADD  1
#define OP_SUB  2
#define OP_DIV  3

int prevClkState=0;
int clkState;
int stateMachine = STATE_IDLE_SETUP;
int risingEdge = 0;
long lastClkTime = 0;
int bitSig = 7;
int txbit = 0;

int num1 = 0;
int num2 = 0;
int op   = 0;
long product = 0;
int txlen = 0;

void loop() {
  // detect a rising edge, with timeout for debouncing
  prevClkState = clkState;
  clkState = digitalRead(SCLK_PIN);
  if(clkState == HIGH && prevClkState == LOW)
  {
    int debounceCtr = 0;
    for(int i = 0; i < 10; i++)
    {
      // read voltage on SCLK pin
      if(digitalRead(SCLK_PIN) == HIGH) {
        debounceCtr++;
      }
      else
      {
        break;  // Leave the for-loop
      }
      // wait before next read
      delay(2);
    }

    if(debounceCtr == 10)
      risingEdge = 1;
  }
  
  switch(stateMachine)
  {
    case STATE_IDLE_SETUP:
      stateMachine = STATE_IDLE;
      Serial.print(" Num1 Num2 OP #Result#\n ");
      pinMode(SCLK_PIN, INPUT);
      num1 = num2 = product = 0;
      bitSig = RXBITS-1;

      stateMachine = STATE_IDLE;
      
    case STATE_IDLE:
      if(risingEdge == 0)
      {
        break;
      }
      else
      {
        stateMachine = STATE_RX1;
      }

    case STATE_RX1:
      if(risingEdge == 1)
      {
        Serial.print(digitalRead(MOSI_PIN));
        num1 += digitalRead(MOSI_PIN) << bitSig;
        bitSig--;
        if(bitSig < 0)
        {
          bitSig = RXBITS-1;
          Serial.print(" ");
          stateMachine = STATE_RX2;
        }
      }
      break;

    case STATE_RX2:
      if(risingEdge == 1)
      {
        Serial.print(digitalRead(MOSI_PIN));
        num2 += digitalRead(MOSI_PIN) << bitSig;
        bitSig--;
        if(bitSig < 0)
        {
          bitSig = 2-1;
          Serial.print(" ");
          stateMachine = STATE_OP;
        }
      }
      break;

    case STATE_OP:
      if(risingEdge == 1)
      {
        Serial.print(digitalRead(MOSI_PIN));
        op += digitalRead(MOSI_PIN) << bitSig;
        bitSig--;
        if(bitSig < 0)
        {
          Serial.print(" ");
          switch(op)
          {
            case OP_MULT:
              product = num1*num2;
              break;
            case OP_ADD:
              product = num1+num2;
              break;
            case OP_SUB:
              product = num1-num2;
              if(product < 0)
              {
                product = 0;
              }
              break;
            case OP_DIV:
              product = num1/num2;
              break;
          } 
          op = 0;
          stateMachine = STATE_TX_SETUP;
        }
      }
      break;

    case STATE_TX_SETUP:
      digitalWrite(MISO_PIN, LOW);
      digitalWrite(SCLK_PIN, LOW);
      pinMode(SCLK_PIN, OUTPUT);

      txlen = 2*RXBITS-1;
/*      while(((product >> txlen) % 2) == 0)
      {
        txlen--;
        if(txlen == 0)
          {
            txlen = 3;
            break;
          }
      } */
      bitSig = txlen;
      txbit = (product >> bitSig--) % 2;
      digitalWrite(MISO_PIN, txbit);
      stateMachine = STATE_TX;
    
    case STATE_TX:
      if(risingEdge == 1)
      {
        Serial.print(txbit);
        while(digitalRead(SCLK_PIN));
        if(bitSig >= 0)
        {
          txbit = (product >> bitSig--) % 2;
          digitalWrite(MISO_PIN, txbit);
        }
        else if(bitSig < 0)
        {
          digitalWrite(MISO_PIN, LOW);
          Serial.print("\n\n");
          stateMachine = STATE_IDLE_SETUP;
        }

      }
      break;
  }

  risingEdge = 0;
}
