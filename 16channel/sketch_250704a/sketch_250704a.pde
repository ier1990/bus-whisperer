import processing.serial.*;

Serial myPort;
int[][] data = new int[16][200]; // [channel][history]
int scopeHeight = 400;
int scopeWidth = 800;

void settings() {
  size(scopeWidth, scopeHeight);  // ✅ This is correct
}

void setup() {  
  myPort = new Serial(this, Serial.list()[1], 115200);
  myPort.bufferUntil('\n');
  background(0);
}

void draw() {
  background(0);
  stroke(0, 255, 0);
  noFill();

  for (int ch = 0; ch < 16; ch++) {
    float yOffset = ch * (height / 16.0);
    beginShape();
    for (int x = 0; x < data[ch].length; x++) {
      float y = map(data[ch][x], 0, 1023, yOffset + (height / 16.0), yOffset);
      vertex(x * (scopeWidth / float(data[ch].length)), y);
    }
    endShape();
  }
}

void serialEvent(Serial p) {
  String inString = p.readStringUntil('\n');
  if (inString == null) return;

  String[] tokens = trim(inString).split(",");
  if (tokens.length != 16) return;

  for (int i = 0; i < 16; i++) {
    int val = int(tokens[i]);
    // shift left, append new value
    arrayCopy(data[i], 1, data[i], 0, data[i].length - 1);
    data[i][data[i].length - 1] = val;
  }
}
