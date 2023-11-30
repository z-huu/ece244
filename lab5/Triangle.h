//
//  Triangle.h
//  Lab5
//
//  Created by Tarek Abdelrahman on 2020-11-25.
//  Copyright © 2020 Tarek Abdelrahman.
//
//  Permission is hereby granted to use this code in ECE244 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.

#ifndef Triangle_h
#define Triangle_h

#include <iostream>
#include <string>

#include "Shape.h"
using namespace std;


// This class implements the triangle Shape objects.
// ECE244 Student: Write the definition of the class here.

class Triangle : public Shape {
private:

    float xc1, xc2, xc3, yc1, yc2, yc3;

public:
    // Constructor
    Triangle (string n, float x1, float y1, float x2, float y2, float x3, float y3);
    
    // Destructor
    virtual ~Triangle();
    
    // Utility methods
    virtual void draw() const;     
    
    virtual float computeArea() const;   // Computes the area
    
    virtual Shape* clone() const;  // Clones the object
};

#endif /* Triangle_h */


