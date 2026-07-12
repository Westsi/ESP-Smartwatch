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
#define DELETED_SIZE 32
extern Notification* notifications[64];
extern Notification* deletedNotifications[DELETED_SIZE];

void updateNotifications(Notification* notif);
Notification* parseNotification(std::string text);

void deleteNotification(int index);
Notification* getNotification(int index);
void printAllNotifications();

#endif