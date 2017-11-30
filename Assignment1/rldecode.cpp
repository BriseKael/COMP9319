//
//  main.cpp
//  rldecode
//
//  Created by Zoey Brise on 15/3/17.
//  Copyright © 2017 Zoey Brise. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

int main(int argc, const char * argv[])
{
    // reading out.rle
    if (argc == 2)
    {
        string file_path = argv[1];
        
        ifstream file;
        file.open(file_path);
        
        unsigned char ch;
        unsigned int count = 0;
        
        int k = 0;
        unsigned int number = 0;
        
        while (!file.eof())
        {
            ch = file.get();
            if (file.fail())
            {
                if (count != 0)
                {
                    cout << "[" << number << "]";
                }
                
                break;
            }
            
            if ( ch >> 7 == 1)
            {
                number = ((ch & 0b01111111) << 7*k) | number;
                count = number + 2;
                k++;
            }
            else
            {
                if (count != 0)
                {
                    cout << "[" << number << "]";
                }
                
                cout << ch;
                
                count = 0;
                k = 0;
                number = 0;
                
            }
        }
        
        file.close();
    }
    else if (argc == 3)
    {
        string infile_path = argv[1];
        string outfile_path = argv[2];
        
        ifstream infile;
        ofstream outfile;
        infile.open(infile_path);
        outfile.open(outfile_path);
        
        unsigned char ch;
        unsigned char temp = '\0';
        unsigned int count = 0;
        
        int k = 0;
        unsigned int number = 0;
        
        while (!infile.eof())
        {
            ch = infile.get();
            if (infile.fail())
            {
                int i;
                for (i = 0; i < count; i++)
                {
                    outfile << temp;
                }
                
                break;
            }
            
            if ( ch >> 7 == 1)
            {
                number = ((ch & 0b01111111) << 7*k) | number;
                count = number + 2;
                k++;
            }
            else
            {
                int i;
                for (i = 0; i < count; i++)
                {
                    outfile << temp;
                }
                outfile << ch;
                temp = ch;
                
                count = 0;
                k = 0;
                number = 0;
                
            }
        }
        infile.close();
        outfile.close();
    }
    
    
    return 0;
}
