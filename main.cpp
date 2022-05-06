
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <list>

using namespace std;

// on and off states
enum State { off = 0, on = 1 }; 

//- COMMAND PATTERN -//

// Receivers:
// class Light (parent: Device)

class Device{
protected:
    int id;
    State currentState = off;
public:
    State toggle(){
        if (currentState == off)
            currentState = on;
        else
            currentState = off;
        return currentState;
    }
    State getCurrentState(){
        return currentState;
    }
    void printCurrentState(){
        printf("Device State: %s\n", (getCurrentState()) ? "on" : "off" );
    }
    int getId(){
        return id;
    }
};

class Light : public Device{
private:
    string color;
    int maxIntensity;
    int minIntensity;
    int currentIntensity;

public:
    Light(int _id, string _color){id = _id; minIntensity = 0; maxIntensity = 100; currentIntensity = 0;};

    void printCurrentIntensity(){
        printf("Device Intensity: %d%\n", currentIntensity);
    }

    int getCurrentIntensity(){
        return currentIntensity;
    }

    bool increaseIntensity(int amount){
        if ((currentIntensity + amount) < maxIntensity)
            currentIntensity += amount;
        else
            currentIntensity = maxIntensity;

        if (getCurrentState() == off){
            toggle();
        }
        return 1;
    }

    bool decreaseIntensity(int amount){
        if ((currentIntensity - amount) > minIntensity)
            currentIntensity -= amount;
        else
            currentIntensity = minIntensity;

        if (currentIntensity == minIntensity && getCurrentState() == on){
            toggle();
        }
        return 1;
    }
};

// Command parent class
class Command{
public:
    virtual bool execute() const = 0;
};

// Command implementations
class toggleCommand : public Command{
protected:
    Device* d;
public:
    toggleCommand(Device* _d){ d = _d;};

    bool execute() const override{
        d->toggle();
        return true;
    }
};

class increaseCommand : public Command{
protected:
    Light* d;
    int amount;
public:
    increaseCommand(Light* _d, int _amount){ d = _d; amount = _amount;};
    
    bool execute() const override{
        d->increaseIntensity(amount);
        return true;
    }
};

class decreaseCommand : public Command{
protected:
    Light* d;
    int amount;
public:
    decreaseCommand(Light* _d, int _amount){ d = _d; amount = _amount;};
    
    bool execute() const override{
        d->decreaseIntensity(amount);
        return true;
    }
};

// Invoker
class Button{
private:
    Command* onPress;
public:
    Button(Command* _action){
        onPress = _action;
    }
    bool press(){
        return onPress->execute();
    }
};

// Stores and creates buttons
class ControlPanel{ // should use a ID to identify each button instead of label 
protected:
    map<string, Button*> Layout;
    list<string> Labels;
public:
    string addButton(string label, Button* _button){
        Layout.insert(make_pair(label, _button));
        Labels.push_back(label);
        return label;
    }
    list<string> getLabels(){
        return Labels;
    }

    Button* getButton(string label){
        return Layout[label]; // if not exist, exception and handle.
    }
};


//- SINGLETON PATTERN -//

// Application / Client
class ControlSystem{
private:
    ControlSystem(){};
    ControlPanel Panel;
    map<int, Device*> deviceMap;
    int nextID = 0;
public:
    ControlSystem(const ControlSystem&) = delete; 

    static ControlSystem& getInstance(){ 
        static ControlSystem Instance;
        return Instance;
    }

    Command* newToggleCommand(Device* device){
        return new toggleCommand(device);
    }

    Command* newIncreaseCommand(Light* light, int amount){
        return new increaseCommand(light, amount);
    }

    Command* newDecreaseCommand(Light* light, int amount){
        return new decreaseCommand(light, amount);
    }

    string createButton(string name, Command* command){
        return Panel.addButton(name, new Button(command));
    }

    int createLight(string color){
        int currentID = nextID++;
        Light* light  = new Light(currentID, color);
        deviceMap.insert(make_pair(currentID, light));
        return currentID;
    }

    Light* getLightDevice(int ID){
        return (Light*)deviceMap[ID]; // if not exist, exception and handle.
    }

    Button* getButton(string label){
        return Panel.getButton(label); // if not exist, exception and handle.
    }
};

int main(){
    
    ControlSystem& a = ControlSystem::getInstance();

    auto light1ID = a.createLight("Blue");
    auto light2ID = a.createLight("Red");

    auto Button1ID = a.createButton("Blue Light Toggle",  a.newToggleCommand(a.getLightDevice(light1ID)));
    auto Button2ID = a.createButton("Red Light Toggle",   a.newToggleCommand(a.getLightDevice(light2ID)));
    auto Button3ID = a.createButton("Green Light Toggle", a.newToggleCommand(a.getLightDevice(a.createLight("Green"))));

    auto Button4ID = a.createButton("Blue Light Increase Intensity by 20%", a.newIncreaseCommand(a.getLightDevice(light1ID), 20));
    auto Button5ID = a.createButton("Blue Light Decrease Intensity by 20%", a.newDecreaseCommand(a.getLightDevice(light1ID), 20));

    // Simple test
  
    printf("\n turning blue light on and off\n");
    a.getLightDevice(light1ID)->printCurrentState();
    printf("\n");

    a.getButton(Button1ID)->press();
    
    a.getLightDevice(light1ID)->printCurrentState();
    printf("\n");
    
    a.getButton(Button1ID)->press();
    
    a.getLightDevice(light1ID)->printCurrentState();
    printf("\n");
    
    printf("\n increasing light intensity\n");
    a.getLightDevice(light1ID)->printCurrentState(); 
    a.getLightDevice(light1ID)->printCurrentIntensity();
    printf("\n");

    a.getButton(Button4ID)->press();

    a.getLightDevice(light1ID)->printCurrentState(); 
    a.getLightDevice(light1ID)->printCurrentIntensity();
    printf("\n");
    
    a.getButton(Button4ID)->press();
    
    a.getLightDevice(light1ID)->printCurrentState(); 
    a.getLightDevice(light1ID)->printCurrentIntensity();
    printf("\n");

    for (auto i = 0; i <= 10; ++i )
        a.getButton(Button4ID)->press();

    a.getLightDevice(light1ID)->printCurrentState(); 
    a.getLightDevice(light1ID)->printCurrentIntensity();
    printf("\n");
    
    printf("\n decreasing light intensity\n");
    a.getButton(Button5ID)->press();

    a.getLightDevice(light1ID)->printCurrentState(); 
    a.getLightDevice(light1ID)->printCurrentIntensity();
    printf("\n");

    a.getButton(Button5ID)->press();

    a.getLightDevice(light1ID)->printCurrentState(); 
    a.getLightDevice(light1ID)->printCurrentIntensity();
    printf("\n");
    
    for (auto i = 0; i <= 10; ++i )
        a.getButton(Button5ID)->press();

    a.getLightDevice(light1ID)->printCurrentState(); 
    a.getLightDevice(light1ID)->printCurrentIntensity();
    printf("\n");
}