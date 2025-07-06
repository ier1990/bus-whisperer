
import processing.serial.*;
import controlP5.*;

Serial myPort;
String[] ports;

ArrayList<String> terminalLines = new ArrayList<String>();
String serialBuffer = "";
ControlP5 cp5;
Textfield commandField;

int centerY;
int centerX;
void setup() {
  size(1000, 600);
  cp5 = new ControlP5(this);
  
  centerY = height / 2;  // or wherever your IC is vertically centered  
  centerX = width /2;

  // Set up Tabs
  cp5.getTab("default").activateEvent(true);  // Keep default tab active
  cp5.addTab("COM PORT").setColorBackground(color(40)).setHeight(20);
  cp5.addTab("THRESHOLDS").setColorBackground(color(40)).setHeight(20);
  cp5.addTab("SETTINGS").setColorBackground(color(40)).setHeight(20);
  
  
  commandField = cp5.addTextfield("Command")
    .setPosition(10, height - 50)  //commandField.setPosition(10, height - 40);  // still fine
    .setSize(400, 30)
    .setAutoClear(true)
    .setLabel("Command Line");  

  // COM PORT Tab
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

  // THRESHOLDS Tab
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

  // SETTINGS Tab (future expansion)
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
  drawTerminal();
  drawTransistorSocket(centerX-50, 100);
  //drawTransistorSocket(centerX/2 - 60, centerY - 140);  // socket above IC
}


void drawTerminal() {  
  int bottomMargin = 40;  // space below last pin
  int icBottomY = centerY + 105;  // last pin Y + spacing
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
    myPort.write(command + "\\n");
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
  text("Transistor Test (Pins 1–3)", x + padSpacing, y - 25 );
}

void TEST3() {
  println("Testing transistor...");
  if (myPort != null) myPort.write("/TEST3\\n");
}


void drawICSocket() {
   
  int pinSpacing = 30;
  int pinRadius = 12;

  fill(50);
  rect(centerX - 30, centerY - 120, 60, 240);  // IC body

  // Left side pins (1-8)
  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;
    fill(0, 255, 0);
    ellipse(centerX - 40, py, pinRadius, pinRadius);
    fill(255);
    textAlign(CENTER, CENTER);
    text(i + 1, centerX - 70, py);

    stroke(0, 255, 0);
    line(centerX - 40, py, 10, py);
  }

  // Right side pins (9-16)
  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;
    fill(0, 255, 0);
    ellipse(centerX + 40, py, pinRadius, pinRadius);
    fill(255);
    textAlign(CENTER, CENTER);
    text(i + 9, centerX + 70, py);

    stroke(0, 255, 0);
    line(centerX + 40, py, width - 10, py);
  }
}

void mousePressed() {
;
  int pinSpacing = 30;
  int pinRadius = 12;

  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;

    if (dist(mouseX, mouseY, centerX - 40, py) < pinRadius) {
      handlePinClick(i + 1);
      return;
    }

    if (dist(mouseX, mouseY, centerX + 40, py) < pinRadius) {
      handlePinClick(i + 9);
      return;
    }
  }
}

void handlePinClick(int pin) {
  if (mouseButton == LEFT) {
    println("Blink pin " + pin);
    if (myPort != null) myPort.write("/WB" + (pin - 1) + "\n");
  } else if (mouseButton == RIGHT) {
    println("Right click - show menu for pin " + pin);
    // Dropdown or menu logic can go here
  }
}


void serialEvent(Serial p) {
  String input = p.readStringUntil('\n');
  
  
  if (input != null) {
    serialBuffer += input.trim();
    if (serialBuffer.contains("<P>") && serialBuffer.contains("</P>")) {
      String msg = serialBuffer.substring(serialBuffer.indexOf("<P>") + 3, serialBuffer.indexOf("</P>"));
      terminalLines.add(msg);
      if (terminalLines.size() > 20) terminalLines.remove(0);  // limit lines
      serialBuffer = "";  // reset buffer after processing
    } else if (serialBuffer.length() > 200) {
      serialBuffer = "";  // flush junk
    }
  }
}




void controlEvent(ControlEvent e) {
  if (e.isFrom("SelectPort")) {
    int index = int(e.getValue());
    println("Selected port: " + ports[index]);
    // myPort = new Serial(this, ports[index], 9600);
  }
}

void ResetAll() {
  println("Reset triggered!");
}
