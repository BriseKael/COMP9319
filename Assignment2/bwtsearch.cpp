//
//  bwtsearch.cpp
//  COMP9319_Assignment2
//
//  Created by Yin Quan (Zoey Brise) on 17/4/17.
//  z5042879 Yin Quan
//

#include <iostream>
#include <fstream>

#include <algorithm>
#include <vector>
#include <map>
#include <math.h>

using namespace std;

const int MAX_BLOCK_SIZE = 20480;   // MAX bs, for buffer use.

int bwt_length;
int block_size;                     // must be generated at least in init.
int block_number;                   // generated with math, must small than 16384.

string bwt_file_path;
string index_file_path;

ifstream bwt_file;

//map<char, int> C;
int C2[128] = {};
char last_char;                     // generated when init complete.
//vector<int> Count_Hint[128];
vector<int> Count_Hint2[128];

// C format:
// cpai:    <char, int>
// 'c', (this) +1 = first pos in sorted_s
//
// Count Hint format:
// chpai:   <char, <int, int>>
//          hint:   <int, int>
// 'c', block_number, occ times.
//
// block number     a       b       c       d       Count Hint
// 1                11      0       0       0       CH[a][0] = 11,
// 2                17      13      0       0       CH[a][1] = 17, CH[b][0] = 13,
// 3                19      23      0       29      CH[a][2] = 19, CH[b][1] = 23, CH[d][0] = 29,
// ...
// last_line        29      31      37      41      CH[a][L] = 29, CH[b][L] = 31, CH[c][L] = 37, CH[d][L] = 41.
//                                                  C[a] = 29,     C[b] = 60,     C[c] = 97,     C[d] = 138.

int init_with_bwt()
{
    // given bwt file.
    // generate C and Count_Hint
    //
    ifstream bwt_file;
    bwt_file.open(bwt_file_path, ios::binary);
    
    // step 1. count the char c times, in alphabet order.
    //
    char c;                     // for file length = 157605064:
    int nb = 1;                 // index of block [1 .. 11670]
    int index = 1;              // index of bwt string[1 .. 157605064]
    
    while (!bwt_file.eof())
    {
        c = bwt_file.get();
        if (bwt_file.fail())
        {
            break;
        }
        
        if (index > block_size*nb)
        {
            // index is the block size +1
            // C store last line information for have not update C yet.
            for (int i = 0; i < 128; i ++)
            {
                if (C2[i] != -1)
                {
                    Count_Hint2[i].push_back(C2[i]);
                }
            }
            //
            nb += 1;
        }
        //
        if (C2[int(c)] == -1)
        {
            C2[int(c)] = 0;
        }
        C2[int(c)] += 1;
        //
        index += 1;
    }
    // Count_Hint add the last line.
    for (int i = 0; i < 128; i ++)
    {
        if (C2[i] != -1)
        {
            Count_Hint2[i].push_back(C2[i]);
        }
    }
    bwt_file.close();
    
    // step 2. add them, get the actual index. C[c] + 1 is the index.
    int act_index = 0;
    int temp = 0;
    for (int i = 0; i < 128; i ++)
    {
        if (C2[i] != -1)
        {
            temp = C2[i];
            C2[i] = act_index;
            act_index += temp;
        }
    }
    return 0;
}

int create_idx()
{
    // since we have the Count Hint
    // we create idx
    ofstream index_file;
    index_file.open(index_file_path, ios::binary);
    
    for (int i = 0; i < 128; i++)
    {
        if (Count_Hint2[i].size() != 0)
        {
            for (int occtimes : Count_Hint2[i])
            {
                index_file.write((char *)(&i), sizeof(i));
                index_file.write((char *)(&occtimes), sizeof(occtimes));
            }
        }
    }

    index_file.close();
    return 0;
}

int init_with_idx()
{
    // since we have idx
    // we generate C and CH from idx
    // Count Hint
    // step 1. read index file. get CH
    ifstream index_file;
    index_file.open(index_file_path, ios::binary);
    
    while (!index_file.eof())
    {
        int i;
        int count;
        index_file.read((char *)(&i), sizeof(i));
        index_file.read((char *)(&count), sizeof(count));
        Count_Hint2[i].push_back(count);
    }
    index_file.close();
    
    // C
    // step 2. copy the last line from CH.
    for (int i = 0; i < 128; i++)
    {
        if (Count_Hint2[i].size() != 0)
        {
            C2[i] = Count_Hint2[i].back();
        }
    }
    
    
    // step 3. add them, get the actual index. C[c] + 1 is the index.
    int act_index = 0;
    int temp = 0;
    for (int i = 0; i < 128; i ++)
    {
        if (C2[i] != -1)
        {
            temp = C2[i];
            C2[i] = act_index;
            act_index += temp;
        }
    }
    
    return 0;
}

int init(string flag)
{
    // get bwt_length
    bwt_file.open(bwt_file_path, ios::binary);
    bwt_file.seekg(0, ios::end);
    bwt_length = (int)bwt_file.tellg();
    bwt_file.close();
    
    // use Math to calculate suitable block size    // file size   bs       bn
    //                                              // 167772160 = 20480 * 8192
    //                                              // 157605064 = 19849 * 7941
    
    if (bwt_length > 419430)                        // 419430 = 1024 * 410
    {
        float rate = sqrt((float)bwt_length / 167772160.0);
        block_size = 20480 * rate;                  // 25525 = 1024 * 25
    }
    else if (bwt_length > 1024)                     // 1024 = 1024 * 1
    {
        block_size = 1024;
    }
    else                                            // 553 = 553 * 1
    {
        block_size = bwt_length;
    }
    
    // block_number;
    block_number = (bwt_length - 1) / block_size + 1;
    
    // init the C2 with -1
    for (int i = 0; i < 128; i++)
    {
        C2[i] = -1;
    }
    //
    
    // decide if we need generate index file
    if (bwt_length > 4*1024*1024)                   // > 4 MB
    {
        ifstream check;
        check.open(index_file_path);
        if (check)
        {
            init_with_idx();
        }
        else
        {
            init_with_bwt();
            create_idx();
        }
        check.close();
    }
    else
    {
        init_with_bwt();
    }
    
    // find the last char
    last_char = '\0';
    for (int i = 127; i > -1; i--)
    {
        if (C2[i] != -1)
        {
            last_char = char(i);
            break;
        }
    }
    
    return 0;
}

int occ(char c, int q)
{
    // given char c and range 1:q
    // since we have CH
    // return count c in this range
    // 1 is 1, 10000 is 1, 10001 is 2.
    int n = (q - 1) / block_size + 1;
    
    char block[MAX_BLOCK_SIZE] = {'\0'};            // reset the buffer to NULL
    string block_s;
    
    bwt_file.clear();
    bwt_file.seekg((n - 1)*block_size, ios::beg);
    bwt_file.read(block, q - (n - 1)*block_size);
    
    block_s = string(block);
    int gap = block_number - (int)Count_Hint2[int(c)].size();
    
    if (n - gap < 2)
    {
        return (int)count(block_s.begin(), block_s.end(), c);
    }
    else
    {
        return (int)count(block_s.begin(), block_s.end(), c) + Count_Hint2[int(c)][n - gap - 2];
    }
}

int inv_occ(char c, int times)
{
    // given char c and times
    // since we have CH
    // return find pos at this time

    vector<int> occ_times = Count_Hint2[c];
    int gap = block_number - (int)occ_times.size();
    
    int n = 1;
    
    if (times <= occ_times[0])
    {
        n = gap + 1;
    }
    else
    {
        n = (int)(lower_bound(occ_times.begin(), occ_times.end(), times) - occ_times.begin() + 1 + gap);
    }
    
    char block[MAX_BLOCK_SIZE] = {'\0'};
    string block_s;
    
    bwt_file.clear();
    bwt_file.seekg((n - 1)*block_size, ios::beg);
    bwt_file.read(block, block_size);
    
    block_s = string(block);
    int order;
    
    if (n - gap < 2)
    {
        order = times - 0;
    }
    else
    {
        order = times - Count_Hint2[int(c)][n - gap - 2];
    }
    
    size_t pos = block_s.find(c, 0);
    
    while ( order > 1 && pos != string::npos)
    {
        pos = block_s.find(c, pos+1);
        order--;
    }
    
    return (int)pos + 1 + (n-1)*block_size;
}


char char_in_bwt_s(int pos)
{
    // return a char c at pos of bwt string
    bwt_file.clear();
    bwt_file.seekg(pos, ios::beg);
    
    char c;
    c = bwt_file.get();
    
    return c;
}


char char_in_sorted_s(int pos)
{
    // return c at pos of sorted string
    // since we have C, we do not need sorted string
    char last_c = last_char;
    for (int i = 0; i < 128; i++)
    {
        if (C2[i] != -1)
        {
            if (pos < C2[i])
            {
                return last_c;
            }
            last_c = char(i);
        }
    }
    
    return last_c;
}

int bwt_Search_decode_word(int first, int last, vector<string> search_word)
{
    // given the key word range: first and last
    // given rest search words or not
    // cout the string
    bwt_file.open(bwt_file_path, ios::binary);
    
    int index;
    string result_left;
    
    int order;
    
    string result_right;
    
    string result;
    
    int t_index;
    char c;
    
    for (t_index = first; t_index <= last; t_index++)
    {
        // backward search, looking for the last char.
        index = t_index;
        result_left = "";
        c = char_in_bwt_s(index-1);
        while (c != '[')
        {
            c = char_in_bwt_s(index-1);
            result_left = c + result_left;
            index = C2[c] + 1 + occ(c, index-1);
        }
        
        // we did not find it in the text field, but in the id field
        if (result_left.find("]") == string::npos)
        {
            continue;
        }
        
        // forward search, looking for the next char.
        index = t_index;
        order = 0;
        result_right = "";
        c = char_in_sorted_s(t_index-1);
        while (c != '[')
        {
            order = index - C2[c];
            index = inv_occ(c, order);
            result_right += c;
            c = char_in_sorted_s(index-1);
        }
        
        result = result_left + result_right + '\n';
        
        if (search_word.size() == 1)
        {
            size_t pos = result.find(']');
            if (result.find(search_word[0], pos+1) != string::npos)
            {
                cout << result;
            }
        }
        else if (search_word.size() == 2)
        {
            size_t pos = result.find(']');
            if (result.find(search_word[0], pos+1) != string::npos && result.find(search_word[1], pos+1) != string::npos)
            {
                cout << result;
            }
        }
        else
        {
            cout << result;
        }
    }
    bwt_file.close();
    
    return 0;
}



pair<int, int> bwt_Search(string p)
{
    //    sort(search_word.begin(), search_word.end(), comp_length);  // length from short to long
    //    string p = search_word[search_word.size()-1];               // longgest
    //    search_word.pop_back();
    
    bwt_file.open(bwt_file_path, ios::binary);
    
    int i = (int)p.length();
    char c = p[i-1];
    
    int first = C2[int(c)] + 1;
    int last2 = bwt_length;
    
    if (c != last_char)
    {
        for (int ii = int(c) + 1; ii < 128; ii++)
        {
            if (C2[ii] != -1)
            {
                last2 = C2[ii];
                break;
            }
        }
    }
    
    //
    
    while ((first <= last2) && (i >= 2))
    {
        c = p[i-1-1];
        first = C2[int(c)] + 1 + occ(c, first - 1);
        last2 = C2[int(c)] + occ(c, last2);
        i = i - 1;
    }
    
    bwt_file.close();
    return make_pair(first, last2);
}

bool comp_range(const pair<int, int> &a,const pair<int, int> &b)
{
    // used only in main function.
    // used for compare the result range for each search word.
    return (a.second - a.first) < (b.second - b.first);
}

int main(int argc, const char * argv[])
{
    if (argc < 4)
    {
        cout << "Usage: btwsearch <fileName> <indexFile> <search_terms> \n";
        return 0;
    }
    
    bwt_file_path = argv[1];
    index_file_path = argv[2];
    
    // step 1. init C and OCC.
    init("Read BWT File");
    
    // step 2. handout the multipule search word.
    vector<string> search_word;
    
    // add argv[3], argv[4], argv[5]
    for (int i = 3; i < argc; i++)
    {
        string term = argv[i];
        
        // case 1:
        // the search word include '[' or ']'.
        // as we don't exist the case 'buckets in text field'.
        // it could cost time when we search something that not exist.
        //
        if (term.find('[') != string::npos || term.find(']') != string::npos)
        {
            
            return 0;
        }
        
        // case 2:
        // the search word include a char that not exist in the bwt string.
        // it could cause error when search some char that not exist during the occ function.
        // althought 'the search is case sensitive'.
        //
        for (char a : term)
        {
            if (C2[int(a)] == -1)
            {
                
                return 0;
            }
        }
        
        search_word.push_back(term);
    }
    
    vector<pair<int, int>> search_word_range;           // pair = <string, pair2>, pair2 = <int, int>
    map<pair<int, int>, string> search_word_range_map;
    
    for (string word : search_word)
    {
        pair<int, int> range = bwt_Search(word);
        search_word_range.push_back(range);
        search_word_range_map[range] = word;
        //        cout << range.first << " " << range.second << endl;
    }
    
    // sort, get the small range of them.
    // -1 or smaller means non-result
    // 0 or more means we get the result.
    //
    sort(search_word_range.begin(), search_word_range.end(), comp_range);
    
    auto small = search_word_range[0];
    
    if (small.second < small.first)
    {
        // find nothing.
        return 0;
    }
    
    vector<string> remain;
    auto it = search_word_range.begin();
    it++;
    while (it != search_word_range.end())
    {
        remain.push_back(search_word_range_map[*it]);
        it++;
    }
    
    search_word.clear();
    search_word_range.clear();
    search_word_range_map.clear();
    
    // cout << small.second - small.first + 1 << endl;
    
    // step 3. print the result.
    bwt_Search_decode_word(small.first, small.second, remain);
    
    return 0;
}
