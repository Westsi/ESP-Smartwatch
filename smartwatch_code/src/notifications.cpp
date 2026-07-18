#include "notifications.h"

#include <Arduino.h>
#include <string>

Notification* notifications[64];
Notification* deletedNotifications[DELETED_SIZE];
int idx = 0;
int deletedIdx = 0;

void updateNotifications(Notification* notif) {
    if (notif->app == "MISFORMATTED" || notif->contents == "MISFORMATTED" || notif->title == "MISFORMATTED"){
        delete notif;
        return;
    }
    for (int i=0;i<idx;i++) {
        if (notifications[i]->app == notif->app && notifications[i]->contents == notif->contents && notifications[i]->title == notif->title) {
            delete notif;
            return; // identical notification already exists
        }
    }
    for (int i=0;i<deletedIdx;i++) {
        if (deletedNotifications[i]->app == notif->app && deletedNotifications[i]->contents == notif->contents && deletedNotifications[i]->title == notif->title) {
            delete notif;
            return; // identical notification was deleted, don't add it back
        }
    }
    notifications[idx++] = notif;
}

Notification* parseNotification(std::string text) {
    Serial.println("CALLED PARSE NOTIFICATION");
    Notification* n = new Notification();
    Serial.println("Notification Text:");
    Serial.printf("\t%s\n", text.c_str());
    // get start indices of sections
    int appIndex = text.find("~APP:");
    if (appIndex == std::string::npos) {
        Serial.println("misformatted app name in notification");
        n->app = "MISFORMATTED";
        n->contents = "MISFORMATTED";
        n->title = "MISFORMATTED";
        return n;
    }
    int titleIndex = text.find("~TITLE:");
    if (titleIndex == std::string::npos) {
        Serial.println("misformatted title in notification");
        n->app = "MISFORMATTED";
        n->contents = "MISFORMATTED";
        n->title = "MISFORMATTED";
        return n;
    }
    int contentsIndex = text.find("~CONTENTS:");
    if (contentsIndex == std::string::npos) {
        Serial.println("misformatted contents in notification");
        n->app = "MISFORMATTED";
        n->contents = "MISFORMATTED";
        n->title = "MISFORMATTED";
        return n;
    }

    // ensure that contents are arranged in format ~APP:...~TITLE:...~CONTENTS:...
    if (appIndex > titleIndex || titleIndex > contentsIndex) {
        Serial.println("bad order in notification");
        n->app = "BAD ORDER";
        n->contents = "BAD ORDER";
        n->title = "BAD ORDER";
        return n;
    }
    
    // app name index will be appIndex+5 and length will be titleIndex-(appIndex+5)
    // same for others with different magic numbers due to different lengths of strings in find

    n->app = text.substr(appIndex+5, titleIndex-(appIndex+5));
    n->title = text.substr(titleIndex+7, contentsIndex-(titleIndex+7));
    n->contents = text.substr(contentsIndex+10); // runs to end of string

    Serial.printf("Parsed notification successfully\nApp: %s, Title: %s, Contents: %s\n", n->app.c_str(), n->title.c_str(), n->contents.c_str());

    return n;
}

Notification* getNotification(int index) {
    return notifications[index];
}

void deleteNotification(int index) {
    Notification* n = notifications[index];
    if (deletedIdx >= DELETED_SIZE) {
        Notification* old = deletedNotifications[deletedIdx%DELETED_SIZE];
        delete old;
        deletedNotifications[deletedIdx%DELETED_SIZE] = n;
        deletedIdx++;
    } else {
        deletedNotifications[deletedIdx++] = n;
    }
    // delete n;
    if (idx == 1) {
        notifications[idx-1] = NULL;
        idx--;
        return;
    }
    idx--;
    notifications[index] = notifications[idx];
    Serial.printf("moving notif at %d to %d\n", idx, index);
    notifications[idx] = NULL;
}

void printAllNotifications() {
    Serial.printf("%d notifications in queue\n", idx);
    for (int i=0;i<idx;i++) {
        Notification* n = getNotification(i);
        if (n == NULL) {
            return;
        }
        Serial.printf("App: %s, Title: %s, Contents: %s\n", n->app.c_str(), n->title.c_str(), n->contents.c_str());
    }
}