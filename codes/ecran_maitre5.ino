#include <Wire.h>  //Library for I2C Communication functions
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <FreeDefaultFonts.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000


//3.5 Calibration
const int XP = 8, XM = A2, YP = A3, YM = 9;  //320x480 ID=0x9486
const int TS_LEFT = 944, TS_RT = 101, TS_TOP = 73, TS_BOT = 907;


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;

Adafruit_GFX_Button on_btn, off_btn, page1_btn, page2_btn, page3_btn;
Adafruit_GFX_Button ok_btn, cncl_btn, plus_btn, minus_btn;
Adafruit_GFX_Button menu_btn, info_btn, back_btn;

int pixel_x, pixel_y;  //Touch_getXY() updates global vars

// Button calibration
int margin = 5;
int btnWidth = 100;
int btnHeight = 40;
int btnY = 200;

// Software variable
bool enable_nuit = false;
int parameter = 50, old_parameter = 50;

long temp0 = 60;
long temp1 = 25.5;
long temp2 = 40;
long temp3 = 35.6;


#define BLACK 0x0000
#define GREY 0x5555
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define DARKGREEN 0x05C0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GOLD 0xDDC0
#define WHITE 0xFFFF


int donnees[] = { 0, 0, 0, 0, 0, 0, false };
int chambre = donnees[0];
bool validation = donnees[6];
int choix_chambre = 0;
int i = 0;


enum pageId {
  MENU,
  CHAMBRES,
  VALIDATION,
  PARAMETRE,
  SERVICE
};

unsigned int currentPage = MENU, oldPage = -1;


void setup() {
  Serial.begin(9600);  //Begins Serial Communication at 9600 baud rate
  tft.reset();
  tft.begin(0x9341);  //Démarrage du shield
  tft.setRotation(1);

  currentPage = MENU;  // Indicates that we are at Home Screen
  Serial.println("Home Page");
  Wire.beginTransmission(8);  // start transmit to slave arduino (8)
  Wire.begin();               //Begins I2C communication at pin (A4,A5)
}




bool down;
void loop() {

  switch (currentPage) {
    case MENU:  //Menu page
      if (currentPage != oldPage) drawMenuScreen();
      page1_btn.press(down && page1_btn.contains(pixel_x, pixel_y));
      page2_btn.press(down && page2_btn.contains(pixel_x, pixel_y));

      if (page1_btn.justReleased())
        page1_btn.drawButton();

      if (page1_btn.justPressed()) {
        page1_btn.drawButton(true);
        currentPage = CHAMBRES;
        for (int  i= 0; i < 6; i++) {
          donnees[i] = 0;
        }
        choix_chambre = 0;
      }

      if (page2_btn.justReleased())
        page2_btn.drawButton();

      if (page2_btn.justPressed()) {
        page2_btn.drawButton(true);
        currentPage = PARAMETRE;
      }

      break;

    case CHAMBRES:
      if (currentPage != oldPage) drawChambresScreen();
      updateChambre();
      menu_btn.press(down && menu_btn.contains(pixel_x, pixel_y));
      ok_btn.press(down && ok_btn.contains(pixel_x, pixel_y));



      if (menu_btn.justReleased())
        menu_btn.drawButton();
      if (ok_btn.justReleased())
        ok_btn.drawButton();
      if (menu_btn.justPressed()) {
        menu_btn.drawButton(true);
        currentPage = MENU;
      } else if (ok_btn.justPressed()) {
        ok_btn.drawButton(true);
        Serial.println(F("Valider"));
        delay(1000);
        currentPage = VALIDATION;
        Serial.println(currentPage);
        Serial.println(currentPage);
        Serial.println(chambre);

      } else {
        TSPoint p = ts.getPoint();

        if (p.z > ts.pressureThreshhold) {

          if ((170 < p.x) && (p.x < 370) && (100 < p.y) && (p.y < 250)) {
            donnees[choix_chambre] = 1;
            choix_chambre = choix_chambre + 1;
          } else if ((170 < p.x) && (p.x < 370) && (305 < p.y) && (p.y < 455)) {
            donnees[choix_chambre] = 2;
            choix_chambre = choix_chambre + 1;
          } else if ((170 < p.x) && (p.x < 370) && (515 < p.y) && (p.y < 670)) {
            donnees[choix_chambre] = 3;
            choix_chambre = choix_chambre + 1;
          } else if ((490 < p.x) && (p.x < 700) && (100 < p.y) && (p.y < 250)) {
            donnees[choix_chambre] = 4;
            choix_chambre = choix_chambre + 1;
          } else if ((520 < p.x) && (p.x < 730) && (305 < p.y) && (p.y < 455)) {
            donnees[choix_chambre] = 5;
            choix_chambre = choix_chambre + 1;
          } else if ((520 < p.x) && (p.x < 730) && (515 < p.y) && (p.y < 670)) {
            donnees[choix_chambre] = 0;
            choix_chambre = choix_chambre + 1;
          } else if ((770 < p.x) && (p.x < 900) && (370 < p.y) && (p.y < 470)) {
            choix_chambre = choix_chambre - 1;
          } else if ((770 < p.x) && (p.x < 900) && (500 < p.y) && (p.y < 600)) {
            choix_chambre = choix_chambre + 1;
          } else {
            chambre = 0;
          }
        }
        delay(100);
      }

      break;

    case VALIDATION:  //Validation page
      if (currentPage != oldPage) drawValidationScreen();
      delay(2000);
      currentPage = SERVICE;
      robotStart(chambre);


      break;

    case SERVICE:  //Validation page
      if (currentPage != oldPage) drawServiceScreen();
      menu_btn.press(down && menu_btn.contains(pixel_x, pixel_y));

      if (menu_btn.justReleased())
        menu_btn.drawButton();


      if (menu_btn.justPressed()) {
        menu_btn.drawButton(true);
        Validation();
        //currentPage = MENU;
      }

      break;

    case PARAMETRE:  
      if (currentPage != oldPage) drawParametreScreen();
      menu_btn.press(down && menu_btn.contains(pixel_x, pixel_y));

      if (menu_btn.justReleased())
        menu_btn.drawButton();

      if (menu_btn.justPressed()) {
        menu_btn.drawButton(true);
        currentPage = MENU;
      }

      break;
  }
  if (oldPage == currentPage) {
    down = Touch_getXY();
  } else {
    down = false;
  }
}





/// PAAMETRAGE DES DIFFERANTES PAGES ///

// ECRAN MENU
void drawMenuScreen() {
  tft.fillScreen(0xDA69);     //0xB79C
  tft.setTextSize(3);

  // Title
  tft.setTextColor(BLACK, WHITE);
  tft.setCursor(0, 10);
  tft.print("CuraBot");                              // Prints the string on the screen
  tft.drawLine(0, 32, tft.width() * 0.6, 32, WHITE);  // Draws the red line
  tft.setTextColor(WHITE, BLACK);                    //((255, 255, 255), (0,0,0));
  tft.setCursor(0, 80);
  tft.setTextColor(GREEN, BLACK);  //((255, 255, 255), (0,0,0));

  // Button
  page1_btn.initButton(&tft, tft.width() / 2., tft.height() / 2. - (1. * btnHeight + margin), 2 * btnWidth, btnHeight, WHITE, WHITE, BLACK, "CHAMBRES", 2);
  page1_btn.drawButton(false);

  page2_btn.initButton(&tft, tft.width() / 2., tft.height() / 2. - (1. * btnHeight + margin) + btnHeight + 10, 2 * btnWidth, btnHeight, WHITE, WHITE, BLACK, "PARAMETRE", 2);
  page2_btn.drawButton(false);

  //Button frame
  tft.drawRoundRect(tft.width() / 2. - 1.5 * btnWidth, tft.height() / 2. - (1.5 * btnHeight + 2 * margin), 2 * btnWidth + btnWidth, 3 * btnHeight + 4 * margin, 10, WHITE);
  oldPage = currentPage;
}

// ECRAN CHOIX CHAMBRE
void drawChambresScreen() {
  tft.fillScreen(0xDA69);
  tft.fillRect(240, 0, 80, 240, 0xC924);  //rectangle à droite
  tft.fillRect(0, 190, 320, 50, 0xA0C3);  //rectangle en bas

  tft.fillRect(10, 20, 60, 60, 0xC924);
  tft.fillRect(90, 20, 60, 60, 0xC924);
  tft.fillRect(170, 20, 60, 60, 0xC924);

  tft.fillRect(10, 110, 60, 60, 0xC924);
  tft.fillRect(90, 110, 60, 60, 0xC924);
  tft.fillRect(170, 110, 60, 60, WHITE);

  tft.setCursor(30, 35);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(4);  //Couleur du texte et taille (1-5)
  tft.println("1");

  tft.setCursor(110, 35);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(4);  //Couleur du texte et taille (1-5)
  tft.println("2");

  tft.setCursor(190, 35);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(4);  //Couleur du texte et taille (1-5)
  tft.println("3");

  tft.setCursor(30, 125);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(4);  //Couleur du texte et taille (1-5)
  tft.println("4");

  tft.setCursor(110, 125);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(4);  //Couleur du texte et taille (1-5)
  tft.println("5");

  tft.setCursor(190, 125);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(4);  //Couleur du texte et taille (1-5)
  tft.println("0");

  tft.setTextSize(1);
  // bouton centré X,Y
  menu_btn.initButton(&tft, tft.width() / 2. - btnWidth - margin, tft.height() - btnHeight / 2., btnWidth, btnHeight, WHITE, WHITE, BLACK, "MENU", 2);
  menu_btn.drawButton(false);

  ok_btn.initButton(&tft, tft.width() / 2. + btnWidth + margin, tft.height() - btnHeight / 2., btnWidth, btnHeight, WHITE, WHITE, BLACK, "Valider", 2);
  ok_btn.drawButton(false);

  tft.setCursor(250, 5);  //Position du curseur pour l'écriture
  tft.setTextColor(WHITE);
  tft.setTextSize(2);  //Couleur du texte et taille (1-5)
  tft.print("Liste");
  updateChambre();

  tft.fillRect(115, 195, 40, 40, WHITE);
  tft.setCursor(125, 205);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(3);  //Couleur du texte et taille (1-5)
  tft.println("<");

  tft.fillRect(165, 195, 40, 40, WHITE);
  tft.setCursor(175, 205);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(3);  //Couleur du texte et taille (1-5)
  tft.println(">");


  oldPage = currentPage;
}

// ECRAN DE VALIDATION
void drawValidationScreen() {
  tft.fillScreen(0xDA69);


  tft.setCursor(60, 50);  //Position du curseur poir l'écriture
  tft.setTextColor(WHITE);
  tft.setTextSize(2.5);  //Couleur du texte et taille (1-5)
  tft.println("Chambres validees");
  tft.setCursor(50, 90);
  tft.print("Chambres selectionees: ");
  tft.setCursor(80, 120);
  tft.println(donnees[0]);
  tft.setCursor(100, 120);
  tft.println(donnees[1]);
  tft.setCursor(120, 120);
  tft.println(donnees[2]);
  tft.setCursor(140, 120);
  tft.println(donnees[3]);
  tft.setCursor(160, 120);
  tft.println(donnees[4]);
  tft.setCursor(180, 120);
  tft.println(donnees[5]);
}

// ECRAN DE SERVICE
void drawServiceScreen() {
  tft.fillScreen(0xDA69);

  tft.setCursor(10, 50);  //Position du curseur poir l'écriture
  tft.setTextColor(WHITE);
  tft.setTextSize(2.5);  //Couleur du texte et taille (1-5)
  tft.println("Veuillez recuperer votre");
  tft.setCursor(120, 80);
  tft.println("commande");

  menu_btn.initButton(&tft, tft.width() / 2, tft.height() / 2 + 25, 3 * btnWidth, btnHeight * 1.5, WHITE, WHITE, BLACK, "Recuperee", 2);
  menu_btn.drawButton(false);

  oldPage = currentPage;
}

// ECRAN DES PARAMETRES
void drawParametreScreen() {
  tft.fillScreen(BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Parametre:");
  tft.setCursor(0, 40);
  tft.setTextSize(2);
  tft.print("Chambre a livrer: ");
  tft.setCursor(210, 40);
  tft.println(chambre);

  menu_btn.initButton(&tft, tft.width() / 2. - btnWidth - margin, tft.height() - btnHeight / 2., btnWidth, btnHeight, WHITE, WHITE, BLACK, "MENU", 2);
  menu_btn.drawButton(false);

  oldPage = currentPage;
}






bool Touch_getXY(void) {
  p = ts.getPoint();
  pinMode(YP, OUTPUT);  //restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    if (tft.width() <= tft.height()) {                     //Portrait
      pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());  //.kbv makes sense to me
      pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    } else {
      pixel_x = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
      pixel_y = map(p.x, TS_RT, TS_LEFT, 0, tft.height());
    }
  }
  return pressed;
}


void updateChambre() {
  if (choix_chambre == 0) {
    tft.fillRect(273, 30, 14, 20, WHITE);
  } else {
    tft.fillRect(273, 30, 14, 20, 0xC924);
  }
  tft.setCursor(275, 33);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(2);  //Couleur du texte et taille (1-5)
  tft.println(donnees[0]);

  if (choix_chambre == 1) {
    tft.fillRect(273, 58, 14, 20, WHITE);
  } else {
    tft.fillRect(273, 58, 14, 20, 0xC924);
  }
  tft.setCursor(275, 61);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(2);  //Couleur du texte et taille (1-5)
  tft.println(donnees[1]);

  if (choix_chambre == 2) {
    tft.fillRect(273, 86, 14, 20, WHITE);
  } else {
    tft.fillRect(273, 86, 14, 20, 0xC924);
  }
  tft.setCursor(275, 89);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(2);  //Couleur du texte et taille (1-5)
  tft.println(donnees[2]);

  if (choix_chambre == 3) {
    tft.fillRect(273, 114, 14, 20, WHITE);
  } else {
    tft.fillRect(273, 114, 14, 20, 0xC924);
  }
  tft.setCursor(275, 117);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(2);  //Couleur du texte et taille (1-5)
  tft.println(donnees[3]);

  if (choix_chambre == 4) {
    tft.fillRect(273, 142, 14, 20, WHITE);
  } else {
    tft.fillRect(273, 142, 14, 20, 0xC924);
  }
  tft.setCursor(275, 145);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(2);  //Couleur du texte et taille (1-5)
  tft.println(donnees[4]);

  if (choix_chambre == 5) {
    tft.fillRect(273, 170, 14, 20, WHITE);
  } else {
    tft.fillRect(273, 170, 14, 20, 0xC924);
  }
  tft.setCursor(275, 173);  //Position du curseur poir l'écriture
  tft.setTextColor(BLACK);
  tft.setTextSize(2);  //Couleur du texte et taille (1-5)
  tft.println(donnees[5]);

  if (choix_chambre == 6) {
    choix_chambre = 0;
  } else if (choix_chambre == -1) {
    choix_chambre = 5;
  }
}



void robotStart(int chambre) {
  Serial.println("le robot commence son parcourt");
  Serial.println(chambre);
  //donnees[0] = chambre;
  donnees[6] = false;

  Wire.beginTransmission(8);  // start transmit to slave arduino (8)
  Wire.write((uint8_t*)&donnees, sizeof(donnees));
  Wire.endTransmission();  // stop transmitting
  delay(500);
  Serial.println("info envoyee");
  tft.reset();
  tft.begin(0x9341);  //Démarrage du shield
  tft.setRotation(1);
}

void Validation() {
  validation = true;
  donnees[0] = -1;
  donnees[6] = validation;
  tft.fillScreen(BLACK);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 90);
  tft.print("Commande ok");
  delay(1000);

  Wire.beginTransmission(8);  // start transmit to slave arduino (8)
  Wire.write((uint8_t*)&donnees, sizeof(donnees));
  Wire.endTransmission();  // stop transmitting
  delay(500);
  Serial.println("info envoyee validation");
  Serial.println(validation);
  i += 1;
  if (donnees[i] == 0){
    currentPage = MENU;
    i = 0;
  } else {
    currentPage = SERVICE;
  }


  tft.reset();
  tft.begin(0x9341);  //Démarrage du shield
  tft.setRotation(1);
}