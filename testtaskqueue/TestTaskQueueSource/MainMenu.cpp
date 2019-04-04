
//
//  MainMenu.cpp
//  testtaskqueue
//
//  Created by Luke Hollingworth on 04/04/2019.
//  Copyright © 2019 midas. All rights reserved.
//

#include "MainMenu.h"
#include "TaskQueueTest.h"
#include <sstream>

void MainMenu::start() {
    while(1) {
        mainMenu();
    }
}

void MainMenu::mainMenu(const bool invalidParameter) {
    std::cout << "\nTest Parameter Values\n";
    unsigned int count = 1;
    for(auto param : parameters_) {
        std::cout << count++ << ")" << param.first << ": " << param.second << "\n";
    }
    if(invalidParameter) {
        std::cout << "Invalid parameter value, all parameters must be an integer greater than zero";
    }
    std::cout << "\nPlease select a parameter to change, type run to start run the test" << std::endl;
    std::string input;
    std::cin >> input;
    if(input.find("run") != std::string::npos) {
       // run anywhere in the string starts the test for simplicity
        runTest();
    } else {
        std::stringstream ss(input);
        unsigned int new_val = 0;
        ss >> new_val;
        if(new_val > 0 && new_val <= parameters_.size()) {
            changeParamMenu(parameters_.at(new_val - 1));
        } else {
            std::cout << "Invalid selection, please try again" << std::endl;
        }
    }
}

void MainMenu::changeParamMenu(std::pair<std::string, unsigned int> &parameter) {
    std::cout << "\nPlease enter a new value for parameter " << parameter.first << std::endl;
    unsigned int val;
    std::cin >> val;
    parameter.second = val;
}

void MainMenu::runTest() {
    TaskQueueTest tester;
    tester.runTest(parameters_[0].second, parameters_[1].second, parameters_[2].second,
        parameters_[3].second,  std::chrono::milliseconds(parameters_[4].second),
        std::chrono::milliseconds(parameters_[5].second));
}
