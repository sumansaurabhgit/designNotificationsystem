#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

/*============================
      Notification & Decorators
=============================*/

class INotification {
public:
    virtual string getContent() const = 0;

    virtual ~INotification() {}
};

// Concrete Notification: simple text notification.
class SimpleNotification : public INotification {
private:
    string text;
public:
    SimpleNotification(const string& msg) {
        text = msg;
    }
    string getContent() const override {
        return text;
    }
};

// Abstract Decorator: wraps a Notification object.
class INotificationDecorator : public INotification {
protected:
    INotification* notification;
public:
    INotificationDecorator(INotification* n) {
        notification = n;
    }
    virtual ~INotificationDecorator() {
        delete notification;
    }
};

// Decorator to add a timestamp to the content.
class TimestampDecorator : public INotificationDecorator {
public:
    TimestampDecorator(INotification* n) : INotificationDecorator(n) { }
    
    string getContent() const override {
        return "[2025-04-13 14:22:00] " + notification->getContent();
    }
};

// Decorator to append a signature to the content.
class SignatureDecorator : public INotificationDecorator {
private:
    string signature;
public:
    SignatureDecorator(INotification* n, const string& sig) : INotificationDecorator(n) {
        signature = sig;
    }
    string getContent() const override {
        return notification->getContent() + "\n-- " + signature + "\n\n";
    }
};

/*============================
  Observer Pattern Components
=============================*/

// Observer interface: each observer gets an update with a Notification pointer.
class IObserver {
public:
    virtual void update() = 0;

    virtual ~IObserver() {}
};

class IObservable {
public:
    virtual void addObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    virtual void notifyObservers() = 0;
};

// Concrete Observable
class NotificationObservable :  public IObservable {
private:
    vector<IObserver*> observers;
    INotification* currentNotification;
public:
    NotificationObservable() { 
        currentNotification = nullptr; 
    }

    void addObserver(IObserver* obs) override {
        observers.push_back(obs);
    }

    void removeObserver(IObserver* obs) override {
        observers.erase(remove(observers.begin(), observers.end(), obs), observers.end());
    }

    void notifyObservers() override {
        for (unsigned int i = 0; i < observers.size(); i++) {
            observers[i]->update();
        }
    }

    void setNotification(INotification* notification) {
        if (currentNotification != nullptr) {
            delete currentNotification;
        }
        currentNotification = notification;
        notifyObservers();
    }

    INotification* getNotification() {
        return currentNotification;
    }

    string getNotificationContent() {
        return currentNotification->getContent();
    }

    ~NotificationObservable() {
        if (currentNotification != NULL) {
            delete currentNotification;
        }
    }
};
    
// Concrete Observer 1
class Logger : public IObserver {
private:
    NotificationObservable* notificationObservable;

public:
    Logger(NotificationObservable* observable) {
        this->notificationObservable = observable;
    }

    void update() {
        cout << "Logging New Notification : \n" << notificationObservable->getNotificationContent();
    }
};

/*============================
  Strategy Pattern Components (Concrete Observer 2)
=============================*/

// Abstract class for different Notification Strategies.

class INotificationStrategy{
  public:
  virtual void sendNotification(string content)=0;
};

class EmailStrategy:public INotificationStrategy{
  private:
  string emailId;
  public:
  EmailStrategy(string emailId){
    this->emailId=emailId;
  }
  
  void sendNotification(string content)override{
    cout<<"sending email Notification to"<<emailId<<" "<<content;
  }
};

class SMSStrategy:public INotificationStrategy{
  private:
  string mobileNumber;
  public:
  SMSStrategy(string mobileNumber){
    this->mobileNumber=mobileNumber;
  }
  void sendNotification(string content)override{
    cout<<"sending Notification to"<<mobileNumber<<" "<<content;
  }
};

class NotificationEngine:public IObserver{
  private:
  NotificationObservable* notificationObservable;
  vector<INotificationStrategy*>notificationStrategies;
  public:
  NotificationEngine(NotificationObservable* observable){
    this->notificationObservable=observable;
  }
  
  void addNotificationStrategies(INotificationStrategy* ns){
    notificationStrategies.push_back(ns);
  }
  void update(){
    string notificationContent=notificationObservable->getNotificationContent();
    for(auto it:notificationStrategies){
      it->sendNotification(notificationContent);
    }
  }
};

//service layer
//singleton class
class NotificationService{
  private:
  NotificationObservable* observable;
  static NotificationService* instance;
  vector<INotification*>notifications;
  
  NotificationService(){
    observable=new NotificationObservable();
  }
  public:
  static NotificationService* getInstance(){
    if(instance==nullptr){
      instance=new NotificationService();
    }
    return instance;
  }
  NotificationObservable* getObservable(){
    return observable;
  }
  void sendNotification(INotification* notification){
    observable->setNotification(notification);
  }
};
NotificationService* NotificationService::instance=nullptr;
int main() {
    
    NotificationService* notificationService=NotificationService::getInstance();
    NotificationObservable* notificationObservable=notificationService->getObservable();
    Logger* logger=new Logger(notificationObservable);
    NotificationEngine* notificationEngine=new NotificationEngine(notificationObservable);
    notificationEngine->addNotificationStrategies(new EmailStrategy("a1sumansaurav@gmail.com"));
    notificationEngine->addNotificationStrategies(new SMSStrategy("916428362663"));
    notificationObservable->addObserver(logger);
    notificationObservable->addObserver(notificationEngine);
    
    INotification* notification=new SimpleNotification("Your order has been shipped from the vendor");
    notification=new TimestampDecorator(notification);
    notification=new SignatureDecorator(notification,"customer care");
    notificationService->sendNotification(notification);
    
    delete logger;
    delete notificationEngine;
    return 0;
}