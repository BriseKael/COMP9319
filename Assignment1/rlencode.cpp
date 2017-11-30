//
//  main.cpp
//  rlencode
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
    // no output file
    // aAAbbbBBBBcccccCCCCCCdDDeeeEEEE
    // aAAb[0]B[1]c[2]C[3]dDDe[0]E[1]
    if (argc == 2)
    {
        string file_path = argv[1];
        
        ifstream file;
        file.open(file_path);
        
        unsigned char ch;
        unsigned char temp = file.get();
        unsigned int count = 0;
        
        while (!file.eof())
        {
            ch = file.get();
            if (file.fail())
            {
                if (count == 0)
                {
                    cout << temp;
                }
                else if (count == 1)
                {
                    cout << temp << temp;
                }
                else
                {
                    cout << temp << '[' << count-2 << ']';
                }
                break;
            }
            
            if (temp == ch)
            {
                count += 1;
            }
            else
            {
                // output
                if (count == 0)
                {
                    cout << temp;
                }
                else if (count == 1)
                {
                    cout << temp << temp;
                }
                else
                {
                    cout << temp <<'[' << count-2 << ']';
                }
                // output end
                // reset count and temp
                temp = ch;
                count = 0;
            }
        }
        // output
        
        // output end
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
        unsigned char temp = infile.get();
        unsigned int count = 0;
        
        while (!infile.eof())
        {
            ch = infile.get();
            if (infile.fail())
            {
                if (count == 0)
                {
                    outfile << temp;
                }
                else if (count == 1)
                {
                    outfile << temp << temp;
                }
                else
                {
                    outfile << temp;
                    
                    unsigned int number = count - 2;
                    while (true)
                    {
                        unsigned char nc = (number & 0b01111111) | 0b10000000;
                        number = number >> 7;
                        outfile << nc;
                        if (number == 0)
                        {
                            break;
                        }
                    }
                    
                }
                break;
            }
            
            if (temp == ch)
            {
                count += 1;
            }
            else
            {
                // output
                if (count == 0)
                {
                    outfile << temp;
                }
                else if (count == 1)
                {
                    outfile << temp << temp;
                }
                else
                {
                    outfile << temp;
                    
                    unsigned int number = count - 2;
                    while (true)
                    {
                        unsigned char nc = (number & 0b01111111) | 0b10000000;
                        number = number >> 7;
                        outfile << nc;
                        if (number == 0)
                        {
                            break;
                        }
                    }
                }
                // output end
                // reset count and temp
                temp = ch;
                count = 0;
                
            }
        }
        // output
        
        // output end
        infile.close();
        outfile.close();
        
        
    }
    return 0;
}
