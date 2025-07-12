#include "screens/blackjackscreen.h"
#include "screens/homescreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

char deck[13] =  {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
int phIdx = 0;
int dhIdx = 0;
char playerHand[10] = {' ', ' '};
char dealerHand[10] = {' ', ' '};
bool gameRunning = false;
String gameOverMessage = "";

int money = 1000;
int bet = 0;

void bsFullScreenHandler(String gesture, int x, int y);
void handleDealerDraw();
void handlePlayerHit();
void handlePlayerStand();
void handlePlayerDouble();
int getMinHandValue(bool isDealerHand);
int getMaxHandValue(bool isDealerHand);
int getHandValue(bool isDealerHand);
void handlePlayerLoss();
void handlePlayerWin(bool isBlackjack);
void handleDraw();
void drawGameboard(TFT_eSprite* spr);
void dealHand();


void BlackjackScreen::init(TFT_eSprite* spr, int width, int height) {
    // setup
    iHLen = 0;
    this->spr = spr;
    spr->loadFont(FontLight20);
    spr->setColorDepth(8);
    spr->createSprite(width, height);
    spr->fillSprite(TFT_BLACK);
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    spr->setTextDatum(MC_DATUM);
    spr->setTextWrap(true);
    registerInteractionHandler(bsFullScreenHandler, 0, 240, 0, 240);
}

void BlackjackScreen::update() {
    spr->fillSprite(TFT_BLACK);
    if (gameRunning) {
        if (getHandValue(false) > 21) {
            gameRunning = false;
            gameOverMessage = "You went over 21!";
            handlePlayerLoss();
        }
    }
    drawGameboard(spr);
}

void BlackjackScreen::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void BlackjackScreen::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void BlackjackScreen::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void BlackjackScreen::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with watchface at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
            break;
        }
    }
}

void bsFullScreenHandler(String gesture, int x, int y) {
    Serial.println("Test handler called");
    if (gesture == "SWIPE DOWN") {
        
    }
    else if (gesture == "SWIPE UP") {

    }
    else if (gesture == "SWIPE LEFT") {

    }
    else if (gesture == "SWIPE RIGHT") {
        switchScr(&hs);
    }
    else if (gesture == "SINGLE CLICK") {
        if (!gameRunning) {
            if (x >= 60 && x <= 180 && y <= 160 && y >= 120) {
                dealHand();
            }
        } else {
            /*
            Hit: x=80-160, y=100-130
            Stand: x=80-160, y=140-170
            Double: x=80-160, y=180-210
            */
            if (x >= 80 && x <= 160) {
                if (y >= 100 && y <= 130) {
                    // Hit
                    handlePlayerHit();
                } else if (y >= 140 && y <= 170) {
                    // Stand
                    handlePlayerStand();
                } else if (y >= 180 && y <= 210) {
                    // Double
                    handlePlayerDouble();
                }
            }
        }
    }
    else if (gesture == "DOUBLE CLICK") {

    }
    else if (gesture == "LONG PRESS") {

    }
}


char chooseRandomCard() {
    uint8_t idx = (uint8_t) (esp_random() % 13);
    return deck[idx];
}

void dealHand() {
    //TODO: get player bet - add a slider like brightness
    playerHand[0] = chooseRandomCard();
    playerHand[1] = chooseRandomCard();
    dealerHand[0] = chooseRandomCard();
    dealerHand[1] = chooseRandomCard();
    phIdx = 2;
    dhIdx = 2;
    gameRunning = true;
    bool dealerBJ = (getMaxHandValue(true) == 21);
    bool playerBJ = (getMaxHandValue(false) == 21);
    if (dealerBJ && playerBJ) {
        gameRunning = false;
        gameOverMessage = "You both had blackjack!";
        handleDraw();
    } else if (playerBJ) {
        gameRunning = false;
        gameOverMessage = "You had blackjack!";
        handlePlayerWin(true);
    } else if (dealerBJ) {
        gameRunning = false;
        gameOverMessage = "Dealer had blackjack!";
        handlePlayerLoss();
    }
}

int getMinHandValue(bool isDealerHand) {
    char* checkingHand = playerHand;
    int chi = phIdx;
    if (isDealerHand) {
        checkingHand = dealerHand;
        chi = dhIdx;
    }
    int minVal = 0;
    for (int i=0;i<chi;i++) {
        char card = checkingHand[i];
        switch (card) {
            case 'A':
            minVal += 1;
            break;
            case '2':
            minVal += 2;
            break;
            case '3':
            minVal += 3;
            break;
            case '4':
            minVal += 4;
            break;
            case '5':
            minVal += 5;
            break;
            case '6':
            minVal += 6;
            break;
            case '7':
            minVal += 7;
            break;
            case '8':
            minVal += 8;
            break;
            case '9':
            minVal += 9;
            break;
            case 'T':
            case 'J':
            case 'Q':
            case 'K':
            minVal += 10;
            break;
        }
    }
    return minVal;
}

int getMaxHandValue(bool isDealerHand) {
    char* checkingHand = playerHand;
    int chi = phIdx;
    if (isDealerHand) {
        checkingHand = dealerHand;
        chi = dhIdx;
    }
    int maxVal = 0;
    for (int i=0;i<chi;i++) {
        char card = checkingHand[i];
        switch (card) {
            case 'A':
            maxVal += 11;
            break;
            case '2':
            maxVal += 2;
            break;
            case '3':
            maxVal += 3;
            break;
            case '4':
            maxVal += 4;
            break;
            case '5':
            maxVal += 5;
            break;
            case '6':
            maxVal += 6;
            break;
            case '7':
            maxVal += 7;
            break;
            case '8':
            maxVal += 8;
            break;
            case '9':
            maxVal += 9;
            break;
            case 'T':
            case 'J':
            case 'Q':
            case 'K':
            maxVal += 10;
            break;
        }
    }
    return maxVal;
}

int getHandValue(bool isDealerHand) {
    int max = getMaxHandValue(isDealerHand);
    int min = getMinHandValue(isDealerHand);
    if (min > 21) {
        return min;
    }
    if (max <= 21) {
        return max;
    }
    if (max > 21 && min <= 21) {
        return min;
    }
    return min;
}

void drawGameboard(TFT_eSprite* spr) {
    String dhs = "Dealer: ";
    if (gameRunning) {
        dhs = dhs + dealerHand[0];
        dhs = dhs + " ?";
    } else {
        for (int i=0;i<dhIdx;i++) {
            dhs = dhs + dealerHand[i];
        }
    }

    String phs = "You: ";
    for (int i=0;i<phIdx;i++) {
        phs = phs + playerHand[i];
    }

    String pms = "Money: ";
    pms = pms + money;

    spr->drawString(pms, 120, 20);
    spr->drawString(dhs, 120, 40);
    spr->drawString(phs, 120, 60);

    if (gameRunning) {
        /*
        Hit: x=80-160, y=100-130
        Stand: x=80-160, y=140-170
        Double: x=80-160, y=180-210
        */
        spr->fillSmoothRoundRect(80, 100, 80, 30, 10, TFT_XON_BLUE);
        spr->setTextColor(TFT_WHITE, TFT_XON_BLUE, true);
        spr->drawString("Hit", 120, 115);
        spr->setTextColor(TFT_WHITE, TFT_BLACK, true);

        spr->fillSmoothRoundRect(80, 140, 80, 30, 10, TFT_XON_BLUE);
        spr->setTextColor(TFT_WHITE, TFT_XON_BLUE, true);
        spr->drawString("Stand", 120, 155);
        spr->setTextColor(TFT_WHITE, TFT_BLACK, true);

        spr->fillSmoothRoundRect(80, 180, 80, 30, 10, TFT_XON_BLUE);
        spr->setTextColor(TFT_WHITE, TFT_XON_BLUE, true);
        spr->drawString("Double", 120, 195);
        spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    } else {
        spr->drawString(gameOverMessage, 120, 80);

        spr->fillSmoothRoundRect(60, 120, 120, 40, 10, TFT_XON_BLUE);
        spr->setTextColor(TFT_WHITE, TFT_XON_BLUE, true);
        spr->drawString("New Game", 120, 120);
        spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    }
}

void handleDealerDraw() {
    while (getMaxHandValue(true) < 17) {
        dealerHand[dhIdx] = chooseRandomCard();
        dhIdx++;
    }
    gameRunning = false;
    int dealerValue = getHandValue(true);
    int playerValue = getHandValue(false);
    if (playerValue > 21) {
        gameOverMessage = "You went over 21!";
        handlePlayerLoss();
    }
    if (dealerValue > playerValue) {
        gameOverMessage = "You lost!";
        handlePlayerLoss();
    }
    if (playerValue > dealerValue) {
        gameOverMessage = "You won!";
        handlePlayerWin(false);
    }
    if (playerValue == dealerValue) {
        gameOverMessage = "You drew!";
        handleDraw();
    }
}

void handlePlayerHit() {
    playerHand[phIdx] = chooseRandomCard();
    phIdx++;
}

void handlePlayerStand() {
    handleDealerDraw();
}

void handlePlayerDouble() {
    if (money < bet || phIdx > 2) {
        return;
    }
    money -= bet;
    bet = bet * 2;
    playerHand[phIdx] = chooseRandomCard();
    phIdx++;
    handleDealerDraw();
}

void handlePlayerLoss() {
    bet = 0;
}

void handlePlayerWin(bool isBlackjack) {
    money += bet * 2;
    if (isBlackjack) {
        money += bet;
    }
    bet = 0;
}

void handleDraw() {
    money += bet;
    bet = 0;
}