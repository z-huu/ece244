#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Customer.h"
#include "QueueList.h"
#include "Register.h"
#include "RegisterList.h"

using namespace std;

// Function Declarations:

// Set mode of the simulation
string getMode();

// Register
void parseRegisterAction(stringstream &lineStream, string mode, bool &closedRegister, int &regID, bool &openedRegister);
void openRegister(
    stringstream &lineStream,
    string mode, bool &openedRegister);  // register opens (it is upto customers to join)
void closeRegister(stringstream &lineStream,
                   string mode, bool &closedRegister, int &regID);  // register closes 

// Customer
void addCustomer(stringstream &lineStream,
                 string mode, int &lastID, bool &addedCustomer, bool &freeReg);  // customer wants to join


// Helper functions
bool getInt(stringstream &lineStream, int &iValue);
bool getDouble(stringstream &lineStream, double &dValue);
bool foundMoreArgs(stringstream &lineStream);
void updateSingle();
void updateMultiple();

void queueCustomers(QueueList* singleQueue );

// Global variables
RegisterList *registerList; // holding the list of registers
QueueList *doneList; // holding the list of customers served
QueueList *singleQueue; // holding customers in a single virtual queue
double expTimeElapsed; // time elapsed since the beginning of the simulation
bool addedCustomer, closedRegister, freeReg, openedRegister;
int lastID, regID;
// List of commands:
// To open a register
// register open <ID> <secPerItem> <setupTime> <timeElapsed>
// To close register
// register close <ID> <timeElapsed>
// To add a customer
// customer <items> <timeElapsed>

int main() {
  registerList = new RegisterList();
  doneList = new QueueList();
  singleQueue = new QueueList();
  expTimeElapsed = 0;

  // Set mode by the user
  string mode = getMode();

  string line;
  string command;

  cout << "> ";  // Prompt for input
  getline(cin, line);

  while (!cin.eof()) {
    addedCustomer = false;
    closedRegister = false;
    openedRegister = false;
    freeReg = true;
    stringstream lineStream(line);
    lineStream >> command;
    if (command == "register") {
      parseRegisterAction(lineStream, mode, closedRegister, regID, openedRegister);
      
    } else if (command == "customer") {
      addCustomer(lineStream, mode, lastID, addedCustomer, freeReg);
      
    } else {
      cout << "Invalid operation" << endl;
    }
    if (mode == "single") { //BEGIN SINGLE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



      ////////////// Debugging Processes ///////////////////////

      // Print the register list.
      bool printRegList = 0; //IF YOU WANT TO PRINT THE REGISTER LIST ~~~~~~~~~~~~~~~~~~~~~~~~~~
      if (printRegList) {

        cout <<"Printing Register List."<<endl;
        Register* printer = registerList->get_head();

        while (printer != NULL) {
          cout << "Register ID: "<<printer->get_ID() <<endl;
          printer = printer->get_next();
        }

        cout <<"End Register List"<<endl;
      }
    
    // END SINGLE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else if (mode == "multiple") {

      
    } // END MULTIPLE ~~~~~~~~~~~~~~~~~~~~~~~~~

    cout << "> ";  // Prompt for input
    getline(cin, line);
  }

  //Outputting statistics.
    double maxWait = 0, avgWait = 0, stdDev = 0, numCustomers = 0, tempWait,
    sumWait = 0, avgWaitDiff = 0;

    Customer* yup = doneList->get_head();
    while (yup != NULL) {
      tempWait = (yup->get_departureTime() - yup->get_arrivalTime());
      if (tempWait > maxWait) {
        maxWait = tempWait;
      }
      sumWait += tempWait;
      numCustomers++;
      yup = yup->get_next();
    }

    if (  (numCustomers != 0)&&(doneList->get_head() != NULL)  ) {
      avgWait = sumWait/numCustomers;
    }

    yup = doneList->get_head();
    while (yup != nullptr) {
      avgWaitDiff += pow( ((yup->get_departureTime() - yup->get_arrivalTime())-(avgWait)), 2 ); //formula here
      yup = yup->get_next();
    }

    if (  (numCustomers != 0)&&(doneList->get_head() != NULL)  ) {
      stdDev = sqrt((avgWaitDiff)/numCustomers);
    }

    cout << endl;
    cout << "Finished at time " <<expTimeElapsed << endl;
    cout << "Statistics: " << endl;
    cout << "Maximum wait time: " <<maxWait <<endl;
    cout << "Average wait time: "<<abs(avgWait)<<endl;
    cout << "Standard Deviation of wait time: " << stdDev << endl;

  // You have to make sure all dynamically allocated memory is freed 
  // before return 0
  delete registerList;
  delete singleQueue;
  delete doneList;

  return 0;
}

string getMode() {
  string mode;
  cout << "Welcome to ECE 244 Grocery Store Queue Simulation!" << endl;
  cout << "Enter \"single\" if you want to simulate a single queue or "
          "\"multiple\" to "
          "simulate multiple queues: \n> ";
  getline(cin, mode);

  if (mode == "single") {
    cout << "Simulating a single queue ..." << endl;
  } else if (mode == "multiple") {
    cout << "Simulating multiple queues ..." << endl;
  }

  return mode;
}

void addCustomer(stringstream &lineStream, string mode, int &lastID, bool &addedCustomer, bool &freeReg) {
  int items;
  double timeElapsed;
  if (!getInt(lineStream, items) || !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments." << endl;
    return;
  }
  // Depending on the mode of the simulation (single or multiple),
  // add the customer to the single queue or to the register with
  // fewest items

  expTimeElapsed += timeElapsed;
  Customer* dude = new Customer(expTimeElapsed, items); //make our customer to enqueue
  dude->set_arrivalTime(expTimeElapsed);


  if (mode == "single") { //enqueue customer in singleQueue
    
    updateSingle(); //processes all departs and queues.

    //Now we add the customer.
    cout << "A customer entered "<< endl;
    singleQueue->enqueue(dude);
    if (registerList->get_free_register() == nullptr) {
      cout << "No free registers" << endl;
    } else { updateSingle(); }

  } else if (mode == "multiple") { //need to enqueue the customer at the register with###################################
                                   //least number of items.

    updateMultiple();
    cout << "A customer entered" << endl;    
    Register* bestReg = registerList->get_min_items_register();
    bestReg->get_queue_list()->enqueue(dude);
    lastID = bestReg->get_ID();

    cout << "Queued a customer with quickest register "<< bestReg->get_ID()<<endl;
    
  }
  
}

void parseRegisterAction(stringstream &lineStream, string mode, bool &closedRegister, int &regID, bool &openedRegister) {
  string operation;
  lineStream >> operation;
  if (operation == "open") {
    openRegister(lineStream, mode, openedRegister);
  } else if (operation == "close") {
    closeRegister(lineStream, mode, closedRegister, regID);
  } else {
    cout << "Invalid operation" << endl;
  }
}

void openRegister(stringstream &lineStream, string mode, bool &openedRegister) {
  int ID;
  double secPerItem, setupTime, timeElapsed;
  // convert strings to int and double
  if (!getInt(lineStream, ID) || !getDouble(lineStream, secPerItem) ||
      !getDouble(lineStream, setupTime) ||
      !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments" << endl;
    return;
  }

  bool registerExists = registerList->foundRegister(ID);

  if (registerExists) { //register already exists

    cout << "Error: register "<<ID<< " is already open" << endl; //print error message
    return;

  } else { //doesn't exist. open the register

    expTimeElapsed += timeElapsed;
    Register* newRegister = new Register(ID, secPerItem, setupTime, expTimeElapsed);
    if (mode == "single") { //adding a customer as we open the register

      updateSingle();
      cout << "Opened register " << ID<< endl;

      registerList->enqueue(newRegister);

      if (singleQueue->get_head() != NULL) {
        cout << "Queued a customer with free register "<<newRegister->get_ID()<<endl;
        newRegister->get_queue_list()->enqueue(singleQueue->dequeue());
        newRegister->get_queue_list()->get_head()->set_departureTime(newRegister->calculateDepartTime());
      }

    } else if (mode == "multiple") { //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
      
      updateMultiple();
      cout << "Opened register " << ID << endl;
      registerList->enqueue(newRegister);
 
    }

  }
  // Check if the register is already open
  // If it's open, print an error message
  // Otherwise, open the register
  // If we were simulating a single queue, 
  // and there were customers in line, then 
  // assign a customer to the new register
  
}

void closeRegister(stringstream &lineStream, string mode, bool &closedRegister, int &regID) {
  int ID;
  double timeElapsed;
  // convert string to int
  if (!getInt(lineStream, ID) || !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments" << endl;
    return;
  }

  // Check if the register is open
  bool foundRegister = registerList->foundRegister(ID);
  if (!foundRegister) { //if register doesn't exist

    cout << "Error: register "<< ID << " is not open" << endl;
  } else { //register does exist

    expTimeElapsed += timeElapsed;

    if (mode == "single") {

      updateSingle();

      //Now delete the register.
      Register* dequeued = registerList->dequeue(ID);
      cout << "Closed register " << ID<<endl;
      delete dequeued;

    } else if (mode == "multiple") { //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      
      updateMultiple();

      //Delete da registah

      Register* dequeued = registerList->dequeue(ID);
      cout << "Closed register "<< ID << endl;
      delete dequeued;

    }


  }

}

bool getInt(stringstream &lineStream, int &iValue) {
  // Reads an int from the command line
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  }
  iValue = stoi(command);
  return true;
}

bool getDouble(stringstream &lineStream, double &dvalue) {
  // Reads a double from the command line
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  }
  dvalue = stod(command);
  return true;
}

bool foundMoreArgs(stringstream &lineStream) {
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  } else {
    return true;
  }
}

void updateSingle() {

  //New code skeleton

      // Update the system.
        //Depart all departable customers, queue customers into free registers
          //Condition for departable customers
            //  if (register)->get_queue_list()->get_head()->get_departureTime()
            //  <= expTimeElapsed
          //Condition for customers to queue
            //  if singleQueue->get_head() != NULL     if there is at least one customer in q
        //Repeat until no more departable customers, or no more customers to queue.

      Register* handler;
      Register* scanner = registerList->get_head();
      Register* toQueue;
      bool queuable = true;
      bool departable = true;
      //Outer while loop, to update the system
      while (queuable||departable) { //Start of Update the system.
        //Updating queuable and departable conditions. Note: they're set to true
        //every time a command is input.
          if (singleQueue->get_head()==NULL) queuable = false; //If no customers
          if (registerList->get_free_register() == NULL) queuable = false; //If all registers occupied.

          if (registerList->get_head() == NULL)  {
              departable = false;
          } else { //If there are registers
              //
            //Scan over the register list to see if there are any customers
            //whose departure times are smaller than expTimeElapsed.
            scanner = registerList->get_head();
            while (scanner !=NULL) {
              if (scanner->get_queue_list()->get_head() == NULL) { 
                //Scanner lands on an unoccupied register
                //Send the scanner to the next reg. Looking for registers WITH customers.
                scanner = scanner->get_next();

              } else if (scanner->get_queue_list()->get_head()->get_departureTime() <= expTimeElapsed) {
                departable = true; break;
                //If we find a customer who should be departed, break out of the loop.
              } else {
                scanner = scanner->get_next();
              }

              //If we reach the end of the regList and we are still in the loop, there
              //are no departable customers.
              if (scanner == NULL) {
                departable = false;
              }

            } //end scanner while
          }
        //End of updating queuable and departable.
          //Departing customers from the registers
          if (departable) {

            //End of scanning
            //At this point, we've scanned and departable has been updated.
            //Begin departing customers in order of departure time.
            
              //Sends handler to the register with the smallest departure time.
              //We can assume handler is NOT nullptr as we have scanned and saw that
              //there are departable customers (nullptr is if all registers are free)

              handler = registerList->calculateMinDepartTimeRegister(0);
              //Departure message.
                cout << "Departed a customer at register ID "<<handler->get_ID()<<" at "
                << handler->calculateDepartTime()<<endl;

              //Now that handler is pointing to a register, start departing.
              handler->departCustomer(doneList);
                
              
          } //End departable.

          //Update queuable again after departing finishes.
          queuable = true;
          if (singleQueue->get_head()==NULL) queuable = false; //If no customers
          if (registerList->get_free_register() == NULL) queuable = false; //If all registers occupied.

          //Queuing customers into free registers
          if (queuable) {

            //Sets the register of interest to the first free one.
            //First referring to closest to the head of the registerList.
            toQueue = registerList->get_free_register();
            toQueue->get_queue_list()->enqueue(singleQueue->dequeue());
            //Dequeues a customer from the singleQueue and queues it in the
            //register of interest.

            //Output message.
            cout << "Queued a customer with free register "<< toQueue->get_ID() << endl;
            //Set the customer's departure time. 
            toQueue->get_queue_list()->get_head()->set_departureTime
            ( toQueue->calculateDepartTime()  );

          } //End queuable.

          //Updating queuable and departable conditions. Note: they're set to true
        //every time a command is input.
          if (singleQueue->get_head()==NULL) queuable = false; //If no customers
          if (registerList->get_free_register() == NULL) queuable = false; //If all registers occupied.
          if (registerList->get_head() == NULL)  {
              departable = false;
          } else { //If there are registers
            //Scan over the register list to see if there are any customers
            //whose departure times are smaller than expTimeElapsed.
            scanner = registerList->get_head();
            while (scanner !=NULL) {
              if (scanner->get_queue_list()->get_head() == NULL) { 
                //Scanner lands on an unoccupied register
                //Send the scanner to the next reg. Looking for registers WITH customers.
                scanner = scanner->get_next();

              } else if (scanner->get_queue_list()->get_head()->get_departureTime() <= expTimeElapsed) {
                departable = true; break;
                //If we find a customer who should be departed, break out of the loop.
              } else {
                scanner = scanner->get_next();
              }

              //If we reach the end of the regList and we are still in the loop, there
              //are no departable customers.
              if (scanner == NULL) {
                departable = false;
              }

            } //end scanner while
          }
        //End of updating queuable and departable.
        
      } //END OUTER WHILE ~~~~~~~~~~~~~~~~~ End system update.


}

void updateMultiple() {

  //Jump from register to register, departing customers in order of their departure time.

  //Once a customer is added, immediately queue them to the register with the least amount of items. 

  Register* handler;
  Register* scanner = registerList->get_head();
  Register* toQueue;

  bool departable = true;

    //Outer while loop, to update the system
    while (departable) { //Start of Update the system.
      //Updating queuable and departable conditions. Note: they're set to true
      //every time a command is input.

        //Traverse the registerList, setting each head's departure time.

        Register* setter = registerList->get_head();
        if (setter == NULL) return;
        while (setter != NULL) {
          if (setter->get_queue_list()->get_head() != NULL)
          setter->get_queue_list()->get_head()->set_departureTime(setter->calculateDepartTime());
          
          setter = setter->get_next();
        }

        if (registerList->get_head() == NULL)  {
            return;
        } else { //If there are registers
            //
          //Scan over the register list to see if there are any customers
          //whose departure times are smaller than expTimeElapsed.
          scanner = registerList->get_head();
          while (scanner !=NULL) {
            if (scanner->get_queue_list()->get_head() == NULL) { 
              //Scanner lands on an unoccupied register
              //Send the scanner to the next reg. Looking for registers WITH customers.
              scanner = scanner->get_next();

            } else if (scanner->get_queue_list()->get_head()->get_departureTime() <= expTimeElapsed) {
              departable = true; break;
              //If we find a customer who should be departed, break out of the loop.
            } else {
              scanner = scanner->get_next();
            }

            //If we reach the end of the regList and we are still in the loop, there
            //are no departable customers.
            if (scanner == NULL) {
              departable = false;
            }

          } //end scanner while
        }
        //End of updating departable.
        //Departing customers from the registers
        if (departable) {

          //End of scanning
          //At this point, we've scanned and departable has been updated.
          //Begin departing customers in order of departure time.
          
            //Sends handler to the register with the smallest departure time.
            //We can assume handler is NOT nullptr as we have scanned and saw that
            //there are departable customers (nullptr is if all registers are free)

            handler = registerList->calculateMinDepartTimeRegister(0);
            if (handler == nullptr) {departable = false;} else {

              //Departure message.
              cout << "Departed a customer at register ID "<<handler->get_ID()<<" at "
              << handler->calculateDepartTime()<<endl;

              //Now that handler is pointing to a register, start departing.
              handler->departCustomer(doneList);
            
            }

        } //End departable.
 
        //Updating departable condition Note: it is set to true
      //every time a command is input.

        if (registerList->get_head() == NULL)  {
            departable = false;
        } else { //If there are registers

          //Scan over the register list to see if there are any customers
          //whose departure times are smaller than expTimeElapsed.
          scanner = registerList->get_head();
          while (scanner !=NULL) {

            if (scanner->get_queue_list()->get_head() == NULL) { 
              //Scanner lands on an unoccupied register
              //Send the scanner to the next reg. Looking for registers WITH customers.
              scanner = scanner->get_next();

            } else if (scanner->get_queue_list()->get_head()->get_departureTime() <= expTimeElapsed) {
              departable = true; break;
              //If we find a customer who should be departed, break out of the loop.
            } else {
              scanner = scanner->get_next();
            }

            //If we reach the end of the regList and we are still in the loop, there
            //are no departable customers.
            if (scanner == NULL) {
              departable = false;
            }

          } //end scanner while
        }
      //End of updating queuable and departable.
      
    } //END OUTER WHILE ~~~~~~~~~~~~~~~~~ End system update.  

}