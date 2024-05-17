#ifndef NOTIFICATIONS_H_
#define NOTIFICATIONS_H_

#include <Arduino.h>
#include <string>

class Notification {
    public:
        std::string title;
        std::string app;
        std::string contents;
};

extern Notification* notifications[64];

void updateNotifications(Notification* notif);
Notification* parseNotification(std::string text);

void deleteNotification(int index);
Notification* getNotification(int index);

#endif