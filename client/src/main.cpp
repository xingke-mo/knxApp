/* 
 * File:   main.cpp
 * Author: Joshua Johannson
 *
 * main
 */

// include
#include <unistd.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <Ui.h>
#include <Log.h>
#include <Text.h>
#include <Box.h>
#include <Style.h>
#include <Log.h>
#include <log/Log.h>
#include "ui/Alert.h"
#include "ui/ContainerView.h"
#include "ui/KnxUi.h"
#include "Save.h"
#include <PacketFactory.h>
#include <Packet.h>
#include <Client.h>
#include <to_string.cpp>
#include <Renderer.h>
#include "log/Log.h"

using ui::Ui;
using ui::Box;
using ui::Text;
using ui::Style;
using ui::StyleRule;
using ui::UI_ATTR_POSITION_ABSOLUTE;
using ui::Point;
using ui::str;
using home::Log;
using home::LOG_LEVEL_ALL;
using namespace std;


// var
const string    SERVER_IP   = "127.0.0.1";//"192.168.1.170";
const int       SERVER_PORT = 5049;
home::Client *client;

// ui
Ui *uiMain;
Box *root;
Text *headline;
Text *out;
Alert *alert;
Log logm;


// pre def
void onError(string msg);
void onDisconnect();
void onReceive(home::Packet *packet);


/*
 * main
 */
int main(int argc, char **argv)
{
    // init log
    ui::Log::setLogLevel(ui::UI_LOG_LEVEL_ERROR | ui::UI_LOG_LEVEL_WARNING);
    ui::Log::setUseColor(false);
    Log::setLogLevel(LOG_LEVEL_ALL);

    // start
    logm.setLogName("MAIN");
    logm.ok("start program");

    // init ui
    Ui::init();

    KnxUi::modeEdit = false;
    KnxUi::editing  = NULL;

    // ui components
    root     = new Box("root", "");
    KnxUi::root = root;
    headline = new Text();
    KnxUi::btEdit   = new Text();
    KnxUi::boxEdit  = new Box();
    out      = new Text();
    alert    = new Alert(root);
    Save::containerRoot = new ContainerView();
    Save::containerRoot->style->height->setPercent(100);
    //Save::containerRoot->style->scrollY->set(100);

    headline->text("Knx");
    headline->style->width->set(100);
    root->addChild(headline);

    KnxUi::btEdit->text("Bearbeiten");
    KnxUi::btEdit->style->backgroundColor->set("AACCCC");
    KnxUi::btEdit->style->width->set(500);
    KnxUi::btEdit->style->bottom->set(10);
    root->addChild(KnxUi::btEdit);
    KnxUi::btEdit->onTouchDown([&](View* v, Point a, Point b, Point c){
        if (KnxUi::modeEdit)
        {
            KnxUi::btEdit->text("Bearbeiten");
            KnxUi::btEdit->style->backgroundColor->set("AACCCC");
            KnxUi::removeEdit();
            KnxUi::modeEdit = false;
        }
        else
        {
            KnxUi::btEdit->text("Bearbeiten beenden");
            KnxUi::btEdit->style->backgroundColor->set("FFCCCC");
            KnxUi::modeEdit = true;
        }
    });

    KnxUi::boxEdit->style->position->set(UI_ATTR_POSITION_ABSOLUTE);
    //KnxUi::boxEdit->style->top->set(450);
    KnxUi::boxEdit->style->height->setPercent(100);
    KnxUi::boxEdit->style->backgroundColor->set("FFFFFF");


    root->addChild(Save::containerRoot);
    //root->addChild(KnxUi::boxEdit);

    headline->onTouchDown([&](View* v, Point a, Point b, Point c){
        cout << endl;
        for (auto entry : Save::knxValues)
        {
            logm.debug(entry->toSql(true, false));
        }
        cout << endl;
    });
    root->addChild(out);

    float fontScale = 1.0;
    // in android other font path
#ifdef pl_andr
    Style::getRule("*")->textFamily->set("FreeSans.ttf");
    //Style::getRule("*")->textSize->set(50);
    fontScale = 1.8;
#endif
    // style
    Style::getRule("*")->textSize->set(30 * fontScale);

    // container
    StyleRule stCont(".cont");
    StyleRule stContTxt(".contTxt");
    StyleRule stContCont(".contCont");
    stCont.backgroundColor->set("#33B5E5");
    stContTxt.textColor->set("#FFFFFF");
    //stContCont.height->set(400);
    stContTxt.paddingBottom->set(5);
    stContTxt.paddingLeft->set(5);
    stContTxt.paddingRight->set(5);
    stCont.top->set(5);
    stCont.left->set(5);
    stCont.right->set(5);
    stCont.bottom->set(5);
    //stContCont.height->set(200);
    //stContCont.overflow->set(UI_ATTR_OVERFLOW_VISIBLE);
    stContCont.paddingLeft->set(15);
    stContCont.paddingRight->set(5);
    stContCont.paddingTop->set(5);
    stContCont.paddingBottom->set(5);
    stContCont.backgroundColor->set("#FFFFFF");
    stContCont.left->set(2);
    stContCont.bottom->set(2);
    stContCont.right->set(2);

    // device value
    StyleRule stVal(".val");
    StyleRule stValName(".valName");
    StyleRule stValBtOn(".valBtOn");
    StyleRule stValTxtKnxEntry(".valTxtKnxEntry");
    StyleRule stValEditName(".valEditName");
    stVal.top->set(5);
    stVal.left->set(5);
    stVal.right->set(5);
    stVal.bottom->set(5);
    stVal.paddingBottom->set(5);
    stVal.paddingTop->set(5);
    stVal.paddingRight->set(5);
    stVal.paddingLeft->set(5);
    stVal.width->setPercent(45);
    stValName.textSize->set(20 * fontScale);
    stValName.textColor->set("#000000AA");
    stValName.paddingBottom->set(5);
    stValName.paddingLeft->set(5);
    stValName.paddingRight->set(5);
    stValName.bottom->set(5);
    stValBtOn.textColor->set("#000000CC");
    stValBtOn.paddingBottom->set(5);
    stValBtOn.paddingLeft->set(5);
    stValBtOn.paddingRight->set(5);
    stValBtOn.textSize->set(22 * fontScale);
    stValBtOn.left->setPercent(50);
    stValTxtKnxEntry.width->set(200);
    stValEditName.backgroundColor->set("#33B5E5");
    stValEditName.textColor->set("#FFFFFF");
    stValEditName.paddingTop->set(5);
    stValEditName.paddingBottom->set(8);
    stValEditName.paddingLeft->set(5);
    stValEditName.paddingRight->set(5);




    // open screen
    uiMain = new Ui(500, 600, "Knx Client");
    uiMain->setRootView(root);
    uiMain->frameRenderer->start();

    // init database
    Save::init();

    // wait until height is know
    sleep(1);
    KnxUi::boxEdit->style->top->set(root->renderer->renderAttributes.height - 200 * fontScale);
    logm.debug("height: " + str(root->renderer->renderAttributes.height));


    /*
    for (int i = 0; i < 10; ++i)
    {
        SwitchDeviceValue *switchDeviceValue = new SwitchDeviceValue();

        SqlEntry *entry = new SqlEntry("containerValues", 3);
        entry->addField(new SqlEntry::Field("name", "lich,,t" + str(i), entry));
        entry->addField(new SqlEntry::Field("value", "0", entry));
        entry->addField(new SqlEntry::Field("parentId", str(i), entry));
        entry->onNewEntryOfTable(entry->tableName, entry);
        entry->syncPush();

        switchDeviceValue->bindToSqlEntry(entry);
        Save::containerRoot->addContend(switchDeviceValue);
        switchDeviceValue->updateParent();
    }

    */





    /*
    ContainerView *container = new ContainerView();
    container->parentId = 7;
    Save::dbAddNewElement(container);
    container->setName(": 9 - parent 7");

    ContainerView *container2 = new ContainerView();
    container2->parentId = 9;
    Save::dbAddNewElement(container2);
    container2->setName(": 10 - parent 9");
    */

    // setup home server connection
    client = new home::Client(SERVER_IP, SERVER_PORT);
    client->onError(&onError);
    client->onReceive(&onReceive);
    client->onDisconnect(&onDisconnect);

    /*Log log1;
    log1.setLogName("bla", "blalallala");
    log1.ok("hallo?");
    */

    // init
    client->init();

    // connect
    client->connectToServer();


    // not close programm
    while (true)
    {
        sleep(100000);
    }
}


// ## NETWORK ###########################
// -- ON RECEIVE ------------------------
void onReceive(home::Packet *packet)
{
    logm.info("receive packet '" + str(packet->type) + "'");

    // cast to knx packet
    home::KnxPacket *clientPacket = ((home::KnxPacket*)packet);

    // if knx packet
    if (clientPacket)
    {
        // get data from home packet
        int main, middle, sub,
                area, groupe, line;
        clientPacket->getDestinationAddr(main, middle, sub);
        clientPacket->getSourcAddr(area, groupe, line);

        // value
        string value = "UNKNOWN VALUE";
        if (((SwitchValue*)clientPacket->value))
        {
            value = ((SwitchValue*)clientPacket->value)->get() ? "on" : "off";
        }

        out->text(out->text() + "[GET ] src: " + to_string(main) + "/" + to_string(middle) + "/" + to_string(sub) + " (" + value + ")\n");
    }
}


// -- ON ERROR --------------------------
void onError(string msg)
{
    cout << msg << endl;

    // show alert
    alert->show(msg);
}


// -- ON DISCONNECT ---------------------
void onDisconnect()
{
    logm.err("disconnected");

    //alert
    Text *button = new Text();
    button->text("Reconnect");
    button->onTouchDown([&](View *v, Point relativeSelf, Point relativeParent, Point absolute)
    {
        alert->hide();

        // reconnect
        client->connectToServer();
    });

    alert->show("no connection to server", button);
}