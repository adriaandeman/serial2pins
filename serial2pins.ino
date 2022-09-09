class SerialToPins {

  private:

    byte *pins;
    char *inputBuffer;
    byte *pinValue;

    int pinAmount;
    int openChar;
    int closeChar;
    byte buffSize;

    boolean readInProgress = false;
    byte bytesRecvd = 0;

  public:

    boolean newData = false;

    SerialToPins(int pinAmount, char openChar, char closeChar) {
      pinValue = new byte[pinAmount];
      this->openChar = openChar;
      this->closeChar = closeChar;
    }

    void setBufferSize(int buffSize) {
      inputBuffer = new char[buffSize];
    }

    void setPins(byte* pins) {
      this->pins = pins;
      for (int i = 0; i < 8; i++) {
        pinMode(pins[i], OUTPUT);
        analogWrite(pins[i], 0);
      }
    }

    void getData() {

      if (Serial.available() > 0) {
        char x = Serial.read();
        
        if (x == ']') {
          readInProgress = false;
          newData = true;
          inputBuffer[bytesRecvd] = 0;
        }

        if (readInProgress) {
          inputBuffer[bytesRecvd] = x;
          bytesRecvd ++;
          if (bytesRecvd == buffSize) {
            bytesRecvd = buffSize - 1;
          }
        }

        if (x == '[') {
          bytesRecvd = 0;
          readInProgress = true;
        }
      }
    }

    void printNewData() {
      for (int i = 0; i < 8; i++) {
        Serial.print(pinValue[i]); Serial.print(" ");
      }
      Serial.println(" ");
    }

    void writeToPins() {
      for (int i = 0; i < 8; i++) {
        analogWrite(pins[i], pinValue[i]);
      }
    }

    void parseData() {

      char * strtokIndx;
      strtokIndx = strtok(inputBuffer, ", ");
      pinValue[0] = atoi(strtokIndx);

      for (int i = 1; i < 8; i++) {
        strtokIndx = strtok(NULL, ", ");
        pinValue[i] = atoi(strtokIndx);
      }
    }
};

SerialToPins S2P = SerialToPins(8, '[', ']');

void setup() {
  Serial.begin(19200);
  byte pins[] = {9, 10, 6, 20, 23, 22, 21, 18};
  S2P.setPins(pins);
  S2P.setBufferSize(40);
}

void loop() {
  S2P.getData();

  if (S2P.newData) {
    S2P.parseData();
    S2P.writeToPins();
    S2P.printNewData();
    S2P.newData = false;
  }
}
