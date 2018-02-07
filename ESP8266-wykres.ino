//

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>
#include "Gsender.h"

#define BUF 600
#define BUF_NAZW 10


#define LDR   A0
#define ONE_WIRE_BUS 5


   float temper;
   float temp[BUF];
   int n = 0;

   int s[BUF];
   int m[BUF];
   int h[BUF];
   bool b[BUF];

  int tempLBase = 23;
  int tempHBase = 40;
  int tempL = tempLBase;
  int tempH = tempHBase;
  int delta = 2;

  bool czy_sa_zapiski = 0;
  char napis[BUF][20];
  char Napis[BUF_NAZW][20];
  int num_n[BUF_NAZW];
  int index_nazw = 0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);


 

const char* ssid = "Tilgin-xxQdR6SKXhzC";//type your ssid
const char* password = "hYDaRdS84zrh8";//type your password
const char* ssid_A = "AndroidAP";//type your ssid
const char* password_A = "bafk5969";//type your password
 
int ledPin = 2; // inside LED
int ledB = 12; // blue Led
int ledG = 13; // green Led
int ledR = 15; // red Led
int button = 4; // button
int time_elapce = 0;  //time between measuring temperature
WiFiServer server(80);//Service Port


void sendSMS(float temper) {
  Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
    String subject = "UAWAGA!!!!! ";
    if(gsender->Subject(subject)->Send("+48605450390@text.plusgsm.pl", (String)temper)) {
        Serial.println("Message send.");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(gsender->getError());
    }
}

 
void setup() {
  
  
  DS18B20.begin();
 
  Serial.begin(115200);
  delay(10);
 
  pinMode(LDR, INPUT);
  pinMode(button, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(ledB, OUTPUT);
  digitalWrite(ledB, LOW);
 
    pinMode(ledG, OUTPUT);
  digitalWrite(ledG, LOW);

    pinMode(ledR, OUTPUT);
  digitalWrite(ledR, LOW);
  
  // Connect to WiFi network
// Connect to WiFi network from Android
  int z = 1;

  
 
   /* Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid_A);
  
    WiFi.begin(ssid_A, password_A);
  
    while (WiFi.status() != WL_CONNECTED && z < 20) {
      delay(500);
      Serial.print(".");
      z++;
   }
   
    
 if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.print("Fail connected to  ");  */
    Serial.println(ssid_A);
    Serial.println();
  
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED && z < 30) {
      delay(500);
      Serial.print(".");
      z++;
    }
    z = 1;
 // }

 
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  // Connect to time serwer
  configTime(1 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(10);
}
}

float getTemperature() {
    float tempC;
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
 
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    return tempC;
}

 
  
void zapisz_dane(String nnapis="null") {
    time_t now = time(nullptr);
    char *czas = ctime(&now);
    temper = getTemperature();
    //Serial.print("Temperatura -");
    Serial.print(czas);
    //Serial.print(" - wynosi: ");
    Serial.println(temper);
    Serial.println("");
    
    
    temp[n] = temper;
    h[n] = (czas[11] - '0')*10 + czas[12] - '0';
    m[n] = (czas[14] - '0')*10 + czas[15] - '0';
    s[n] = (czas[17] - '0')*10 + czas[18] - '0';
    for(int i=0; i <= nnapis.length(); ++i) { 
      napis[n][i] = nnapis[i];
    }

    if (nnapis != "null") {
	    for(int i=0; i <= nnapis.length(); ++i) { 
    		Napis[index_nazw][i] = nnapis[i];
		    num_n[index_nazw++] = n;		
		    if (index_nazw == BUF_NAZW) { index_nazw = 0;}
	    }
    }
	
    //Serial.println(napis[n]);
    b[n] = digitalRead(button);
    n++;

    if(n == BUF) n = 0;
  
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, LOW);
    digitalWrite(ledB, LOW);
    digitalWrite(ledR, LOW);
        if( temper < tempLBase) {
           //if( temper < tempL) { sendSMS(temper); tempL = temper - delta; }
           digitalWrite(ledB, HIGH);
        } else if( temper < tempHBase ) {
          digitalWrite(ledG, HIGH);
          tempL = tempLBase;
          tempH = tempHBase;
        } else {
         // if( temper > tempH) { sendSMS(temper); tempH = temper + delta; }
            digitalWrite(ledR, HIGH);
        }
       //if(digitalRead(button) == 0) { sendSMS(temper);}
}



 
void loop() {
	bool in=0;
	time_elapce++;
	if(time_elapce == 1000000 ) {
		time_elapce = 0;
		zapisz_dane();
	}
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
	// Wait until the client sends some data
	Serial.println("[new client]");
	while(client.connected()){
		// read line by line what the client (web browser) is requesting
		if (client.available()) {
			// Read the first line of the request
			char date[200];
			int k = 1;
			String request = client.readStringUntil('\r');
//			Serial.println(request);
      if (request.length() > 3 && request[0] == 'G' && request[1] == 'E' && request[2] == 'T') {
			  for(int i = 33; i < (request.length()-9) && i < (17+33); ++i) {
  			  date[k++] = request[i];
			  }
  			if(k>1) {
				  date[0] = '\'';
				  date[k++] = '\''; 
				  date[k] = '\0';
				  Serial.println(date);
				  zapisz_dane(date);
			  }
      }
			if (request.length() == 1 && request[0] == '\n') {
				// Return the response
				client.println("HTTP/1.1 200 OK");
				client.println("Content-Type: text/html");
				client.println(""); //  do not forget this one
				client.println("<!DOCTYPE HTML>");
				client.println("<html>");

					client.println("<head>");
					client.println("<script type='text/javascript' src='https://www.google.com/jsapi'></script>");
					client.println("<script type='text/javascript'>");
					client.println("google.load('visualization', '1', {packages:['corechart', 'controls']});");
					client.println("google.setOnLoadCallback(drawChart);");
					client.println("function drawChart() {");
					client.println("var data = new google.visualization.DataTable();");
					client.println("data.addColumn('timeofday', 'Czas');");
					client.println("data.addColumn('number', 'Temperatura');");
					client.println("data.addColumn({type:'string', role:'annotation'}); ");
					client.println("data.addRows([");
           
					for(int i = n; (temp[i] != 0) && (i<BUF) ; ++i) {
						if(i != n) { client.print(","); }
						client.print("[");
						client.print("[");
						client.print(h[i]);
					        client.print(",");
					        client.print(m[i]);
						client.print(",");
					        client.print(s[i]);
						client.print("]");
						client.print(",");
					        client.print(temp[i]);
					        client.print(",");
						client.print(napis[i]);
					        client.print("]");
					        client.println("");
						in = 1;
					}
          
					for(int i = 0; (temp[i] != 0) && (i<n) ; ++i) {
						if((in == 0) && (i != 0)) { client.print(", "); } else if(in == 1) { client.print(", "); }
						client.print("[");
						client.print("[");
					        client.print(h[i]);
					        client.print(",");
					        client.print(m[i]);
					        client.print(",");
					        client.print(s[i]);
						client.print("]");
					        client.print(",");
					        client.print(temp[i]);
					        client.print(",");
					        client.print(napis[i]);
					        client.print("]");
					        client.println("");
					}
				        client.println("]);");
 
		        client.println("var options = {");
		        client.println("annotations: { textStyle: {fontName: 'Times-Roman',     fontSize: 28, bold: true, italic: true,      color: 'black',      opacity: 0.8    }  },");
		        client.println("hAxis: { title: 'Godzina [hh:mm]' },");
		        client.println("vAxis: { title: 'Temperatura [*C]' }");
		        client.println("};");
 
		        client.println("var chart = new google.visualization.LineChart(document.getElementById('chart_div'));");
		        client.println("chart.draw(data, options);");
			client.println("}");
			client.println("</script>");
			client.println("</head>");
			client.println("<body>");
			client.println("<div id='chart_div' style='width: 900px; height: 500px;'></div>");

		    client.println("<form action='/action_page.php' method='get'>");
		    client.println("<div>");
		    client.println("<label style='font-size: 25px;height:90px;width:100px;'for='co_zrobiono'>Co zrobiono:  </label>");
		    client.println("<input style='font-size: 25px;height:90px;width:300px;' type='text' name='co_zrobiono'><br>");
		    client.println("<input style='font-szie: 65px;height:90px;width:400px; padding: 15px 32px;'type='submit' value='Submit'>");
		    client.println("<div>");
		    client.println("</form>");
		    client.println("</body>");
		    client.println("</html>");
				break;
			}
		}
	}

    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");
  }
 
}
