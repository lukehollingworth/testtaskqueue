
//
//  MainWindow.h
//  testtaskqueue
//
//  Created by Luke Hollingworth on 04/04/2019.
//  Copyright © 2019 midas. All rights reserved.
//

#ifndef __MAINMENU__
#define __MAINMENU__

#include <iostream>
#include <vector>
#include <map>

class MainMenu {
public:
    void start();
    
private:
    void mainMenu(const bool invalidParameter = false);
    
    void changeParamMenu(std::pair<std::string, unsigned int> &parameter);
    
    void runTest();

    std::vector< std::pair<std::string, unsigned int> > parameters_ = {
        {"Consumer gets jobs via pop_try (0) or pop", 0},
        {"FIFO queue length", 10},
        {"Number of producer threads", 5},
        {"Number of jobs to produce per producer thread", 5},
        {"Producer thread time between producing jobs (ms)", 10},
        {"Consumer thread time to execute a job (ms)", 2}
    };
};

#endif
