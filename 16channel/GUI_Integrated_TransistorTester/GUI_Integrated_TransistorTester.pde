
import processing.serial.*;
import controlP5.*;

Serial myPort;
String[] ports;

int[][] analogData = new int[16][200];  // buffer
// 16-channel analog buffer
int[][] data = new int[16][200];  // 16 channels, 200 samples each

// Index 0 = pin 1, Index 15 = pin 16
int[] pinMap = {
  2, 3, 4, 5, 6, 7, 8, 9,   // Pins 1–8
  19, 18, 17, 16, 15, 14, 11, 10  // Pins 9–16
  //10, 11, 14, 15, 16, 17, 18, 19  // Pins 9–16
};

// Index 0 = pin 1, Index 15 = pin 16
int[] pinMap10k = {
  22, 24, 26, 28, 30, 32, 34, 36,   // Pins 1–8
  52, 50, 48, 46, 44, 42, 40, 38  // Pins 9–16
  //38, 40, 42, 44, 46, 48, 50, 52  // Pins 9–16  
};


boolean showContextMenu = false;
int contextPin = -1;
int contextX, contextY;
String[] menuOptions = { "Blink 160Ω", "Pulse High", "Set as Input", "Set as Output", "Label as..." };





ArrayList<String> terminalLines = new ArrayList<String>();
String serialBuffer = "";
String lastTransistorResult = "";

ControlP5 cp5;
Textfield commandField;

float thresholdLow = 200;
float thresholdHigh = 410;

int centerY;
int centerX;

void setup() {
  size(800, 600);
  cp5 = new ControlP5(this);

  centerY = height / 2;
  centerX = width / 2;

  // Tabs
  cp5.getTab("default").activateEvent(true);
  cp5.addTab("COM PORT").setColorBackground(color(40)).setHeight(20);
  cp5.addTab("THRESHOLDS").setColorBackground(color(40)).setHeight(20);
  cp5.addTab("SETTINGS").setColorBackground(color(40)).setHeight(20);

  commandField = cp5.addTextfield("Command")
    .setPosition(10, height - 50)
    .setSize(400, 30)
    .setAutoClear(true)
    .setLabel("Command Line");  

  // COM Port Dropdown
  ports = Serial.list();
  DropdownList portList = cp5.addDropdownList("SelectPort")
    .setPosition(20, 40)
    .setSize(150, 150)
    .setItemHeight(20)
    .setBarHeight(20)
    .moveTo("COM PORT");

  for (int i = 0; i < ports.length; i++) {
    portList.addItem(ports[i], i);
  }

  // Threshold sliders
  cp5.addSlider("thresholdLow")
     .setPosition(20, 40)
     .setSize(200, 20)
     .setRange(0, 1023)
     .setValue(200)
     .setLabel("LOW Threshold")
     .moveTo("THRESHOLDS");

  cp5.addSlider("thresholdHigh")
     .setPosition(20, 70)
     .setSize(200, 20)
     .setRange(0, 1023)
     .setValue(800)
     .setLabel("HIGH Threshold")
     .moveTo("THRESHOLDS");

  cp5.addButton("ResetAll")
     .setPosition(20, 40)
     .setSize(100, 30)
     .setLabel("Reset")
     .moveTo("SETTINGS");

  cp5.addButton("TEST3")
     .setPosition(width/2 - 50, centerY - 175)
     .setSize(100, 30)
     .setLabel("Test Transistor");     

  println("Ports available:");
  for (String p : ports) println(p);
}

void draw() {
  background(0);
  fill(255);
  textAlign(LEFT, TOP);
  text("Expandable Top Menu GUI", 10, 5);

  
  drawICSocket();
  drawScope();
  drawTerminal();
  drawTransistorSocket(centerX - 50, 100);
  drawPinHighlightLines();  // Optional pin link lines
  
 if (showContextMenu && contextPin > 0) {
  fill(30);
  stroke(200);
  int menuWidth = 120;
  int lineHeight = 20;

  for (int i = 0; i < menuOptions.length; i++) {
    rect(contextX, contextY + i * lineHeight, menuWidth, lineHeight);
    fill(255);
    textAlign(LEFT, CENTER);
    text(menuOptions[i], contextX + 5, contextY + i * lineHeight + lineHeight / 2);
    fill(30);
  }
} 
}

void drawTerminal() {  
  int bottomMargin = 40;
  int icBottomY = centerY + 105;
  int terminalTopY = icBottomY + bottomMargin;
  int terminalHeight = height - terminalTopY;

  fill(20);
  noStroke();
  rect(0, terminalTopY, width, terminalHeight);

  fill(255);
  textAlign(LEFT, TOP);
  textSize(12);

  for (int i = 0; i < terminalLines.size(); i++) {
    text(terminalLines.get(i), 10, terminalTopY + 10 + i * 14);
  }
}

void Command(String command) {
  println("Sending command: " + command);
  if (myPort != null) {
    myPort.write(command + "\n");
  }
}





void drawScope() {
  int pinSpacing = 30;
  int scopeHeight = 15;

  for (int ch = 0; ch < 16; ch++) {
    int py = centerY - 116 + (ch % 8) * pinSpacing;

    for (int i = data[ch].length - 2; i >= 0; i--) {
      int val1 = data[ch][i];
      int val2 = data[ch][i + 1];

      float y1 = py - map(val1, 0, 1023, -scopeHeight, scopeHeight);
      float y2 = py - map(val2, 0, 1023, -scopeHeight, scopeHeight);

      float x1, x2;

      if (ch < 8) {
        x1 = centerX - 40 - (data[ch].length - 1 - i);
        x2 = centerX - 40 - (data[ch].length - 2 - i);
      } else {
        int flippedCh = 8 + (7 - (ch - 8));  // flip channels 8–15
        val1 = data[flippedCh][i];
        val2 = data[flippedCh][i + 1];
      
        y1 = py - map(val1, 0, 1023, -scopeHeight, scopeHeight);
        y2 = py - map(val2, 0, 1023, -scopeHeight, scopeHeight);
      
        x1 = centerX + 40 + (data[ch].length - 1 - i);
        x2 = centerX + 40 + (data[ch].length - 2 - i);
      }

      // Set color for first value in the segment
      if (val2 < thresholdLow) stroke(0, 255, 0); // Green
      else if (val2 > thresholdHigh) stroke(255, 0, 0); // Red
      else stroke(255); // White

      line(x1, y1, x2, y2);
    }
  }
}




void drawScope1() {  //master it works
  int pinSpacing = 30;
  
  int scopeHeight = 18;

  for (int ch = 0; ch < 16; ch++) {
    int py = centerY - 116 + (ch % 8) * pinSpacing;

    noFill();
    //stroke(0, 255, 255);
    beginShape();

    for (int i = data[ch].length - 1; i >= 0; i--) {
      float signal = map(data[ch][i], 0, 1023, -scopeHeight, scopeHeight);
      float y = py - signal;
      
           // Determine stroke color based on thresholds
      if (data[ch][i] < thresholdLow) stroke(0, 255, 0);
      else if (data[ch][i] > thresholdHigh) stroke(255, 0, 0);
      else stroke(0, 255, 255);
      
      

      // LEFT side (pins 1–8)
      if (ch < 8) {
        float x = centerX - 40 - (data[ch].length - 1 - i);  // draw leftward from pin
        vertex(x, y);
      }
      // RIGHT side (pins 9–16)
      else {
        float x = centerX + 40 + (data[ch].length - 1 - i);  // draw rightward from pin
        vertex(x, y);
      }
    }

    endShape();
  }
}

void drawTransistorSocket(int x, int y) {
  int padSpacing = 40;
  fill(120);
  textAlign(CENTER, CENTER);
  textSize(14);
  for (int i = 0; i < 3; i++) {
    ellipse(x + i * padSpacing, y, 20, 20);
    fill(255);
    text("BCE".charAt(i), x + i * padSpacing, y);
    fill(120);
  }
  fill(255);
  text("Transistor Test (Pins 1–3)", x + padSpacing, y - 25);
  textSize(12);
  //text(lastTransistorResult, x + padSpacing, y + 30);
}

void drawICSocket() {
  int pinSpacing = 30;
  int pinRadius = 12;

  fill(50);
  rect(centerX - 30, centerY - 120, 60, 240);

  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;
    fill(0, 255, 0);
    ellipse(centerX - 40, py, pinRadius, pinRadius);
    fill(255);
    textAlign(CENTER, CENTER);
    text(i + 1, centerX - 70, py);
    stroke(0, 255, 0);
    //line(centerX - 40, py, 10, py);
  }

  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;
    fill(0, 255, 0);
    ellipse(centerX + 40, py, pinRadius, pinRadius);
    fill(255);
    textAlign(CENTER, CENTER);
    text(16 - i, centerX + 70, py);
    stroke(0, 255, 0);
    //line(centerX + 40, py, width - 10, py);
  }
}

void drawPinHighlightLines() {
  stroke(255, 100, 100);
  line(centerX - 40, centerY - 105, centerX - 40, 120);  // pin 1 → B
  line(centerX - 40, centerY - 75, centerX, 120);        // pin 2 → C
  line(centerX - 40, centerY - 45, centerX + 40, 120);   // pin 3 → E
}

void mousePressed() {
  int pinSpacing = 30;
  int pinRadius = 12;

  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;
    if (dist(mouseX, mouseY, centerX - 40, py) < pinRadius) {
      handlePinClick(i + 1);
      return;
    }
    if (dist(mouseX, mouseY, centerX + 40, py) < pinRadius) {
      //handlePinClick(16 - i);
      println("i=" + i);
      handlePinClick(i+9);
      return;
    }
  }
  
  
  
  
  
  
  
if (showContextMenu && mouseButton == LEFT) {
  int menuWidth = 120;
  int lineHeight = 20;

  for (int i = 0; i < menuOptions.length; i++) {
    int mx = contextX;
    int my = contextY + i * lineHeight;
    if (mouseX >= mx && mouseX <= mx + menuWidth &&
        mouseY >= my && mouseY <= my + lineHeight) {

      // Execute based on menuOptions[i]
      println("Action: " + menuOptions[i] + " on pin " + contextPin);
      println("/wb" + pinMap[contextPin - 1] );

      if (menuOptions[i].equals("Blink 160Ω")) {
        if (myPort != null) myPort.write("/wb" + pinMap[contextPin - 1] + "\n");
      }

      // TODO: handle other cases...

      showContextMenu = false;  // hide after click
      break;
    }
  }  
}
  
  
if (mouseButton == RIGHT && showContextMenu) {
  showContextMenu = false;
}
  
  
  
}

void handlePinClick(int pin) {
  if (pin < 1 || pin > 16) return;

  int real_pin = pinMap10k[pin - 1];  // adjust for 0-based index

  if (mouseButton == LEFT) {
    println("Blink pin " + pin + " → Arduino D" + real_pin);
    if (myPort != null) myPort.write("/wb" + real_pin + "\n");
  } else if (mouseButton == RIGHT) {
    println("Right click - pin " + pin + " → Arduino D" + real_pin);
    contextPin = pin;
    contextX = mouseX;
    contextY = mouseY;
    showContextMenu = true;    
    // dropdown here
  }
}


void serialEvent(Serial p) {
  
  
  String input = p.readStringUntil('\n');
  if (input != null) {
    serialBuffer += input.trim();
    

    int start = serialBuffer.indexOf("<P>");
    int end = serialBuffer.indexOf("</P>");
    while (start >= 0 && end > start) {
      String msg = serialBuffer.substring(start + 3, end);
      terminalLines.add(msg);
      lastTransistorResult = msg;
      if (terminalLines.size() > 20) terminalLines.remove(0);
      serialBuffer = serialBuffer.substring(end + 4);
      start = serialBuffer.indexOf("<P>");
      end = serialBuffer.indexOf("</P>");
    }
    
  // Parse <A>...</A> tags
  int startA = serialBuffer.indexOf("<A>");
  int endA = serialBuffer.indexOf("</A>");
  while (startA >= 0 && endA > startA) {
    String packet = serialBuffer.substring(startA + 3, endA);  // extract CSV inside <A>
    String[] tokens = split(packet, ',');
  
    if (tokens.length == 16) {
      for (int i = 0; i < 16; i++) {
        int val = int(tokens[i]);
        arrayCopy(data[i], 1, data[i], 0, data[i].length - 1);
        data[i][data[i].length - 1] = val;
      }
    }
  
    // Remove the processed <A>...</A> from the buffer
    serialBuffer = serialBuffer.substring(0, startA) + serialBuffer.substring(endA + 4);
    startA = serialBuffer.indexOf("<A>");
    endA = serialBuffer.indexOf("</A>");
  }  

    if (serialBuffer.length() > 200) {
      serialBuffer = "";
    }
  }
}




void controlEvent(ControlEvent e) {
  if (e.isFrom("SelectPort")) {
    int index = int(e.getValue());
    println("Selected port: " + ports[index]);
    myPort = new Serial(this, ports[index], 115200);
  }
}

void ResetAll() {
  println("Reset triggered!");
  if (myPort != null) myPort.write("/reset" + "\n");  
}
