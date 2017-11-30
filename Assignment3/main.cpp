//
//  main.cpp
//  COMP9319 Ass3
//
//  Created by Zoey Brise on 3/5/17.
//  Copyright © 2017 Zoey Brise. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>

#include <algorithm>
#include <map>
#include <vector>
#include <list>
#include <queue>

// to use FILE *, to be Flash!!
#include "stdio.h"
#include <string.h>
#include <sys/stat.h>

#include "porter2_stemmer.h"

using namespace std;

string stop_words[] = {"able","about","across","after","all","almost","also","among","and","any","are","because","been","but","can","cannot","could","dear","did","does","either","else","ever","every","for","from","get","got","had","has","have","her","hers","him","his","how","however","into","its","just","least","let","like","likely","may","might","most","must","neither","nor","not","off","often","only","other","our","own","rather","said","say","says","she","should","since","some","than","that","the","their","them","then","there","these","they","this","tis","too","twas","wants","was","were","what","when","where","which","while","who","whom","why","will","with","would","yet","you","your"};

// file_id -> file_name
// 0 -> "xxx.txt"
vector<string> File_name_list;


// get file names
// input: string target folder path
// return: vector<string> file names under that folder path, sorted and legal.
//
vector<string> get_file_names(string target_folder_path)
{
    vector<string> file_names;
    struct dirent* dir_ent;
    DIR* dir;
    
    if ((dir = opendir(target_folder_path.c_str())) != NULL)
    {
        while ((dir_ent = readdir(dir)) != NULL)
        {
            if (dir_ent->d_name[0] == '.' || strcmp(dir_ent->d_name, "..") == 0)
                //            if (dir_ent->d_name[0] == '.' || string(dir_ent->d_name) == "..")
            {
                continue;
            }
            file_names.push_back(dir_ent->d_name);
        }
    }
    closedir(dir);
    sort(file_names.begin(), file_names.end());
    
    return file_names;
}


// get file size
// input: string file path
// return: int file size
//
int get_file_size(string file_path)
{
    ifstream temp;
    temp.open(file_path, ios::binary);
    temp.seekg(0, ios::end);
    int file_size = (int) temp.tellg();
    temp.close();
    return file_size;
}


// input: string index file path
// input: map<string, string> diction of term -> posting
// do: write as term: posting + \n
//
int write_idx_block(string index_file_path, map<string, string> dictionary)
{
    FILE *out;
    if((out = fopen(index_file_path.c_str(), "w")) != NULL)
    {
        for (auto i = dictionary.begin(); i != dictionary.end(); i++)
        {
            string temp = i->first + ": " + i->second + "\n";
            fputs(temp.c_str(), out);
        }
    }
    fclose(out);
    return 1;
}


// input: string target folder path,
// input: vector<int> file id list of one block
// return: map<string, string> term-posting dictionary of tokenized word of one block
//
map<string, string> tokenize(string target_folder_path, vector<int> file_id_list)
{
    // dictionary <term, posting>
    // term -> [file_id, freqency]
    //
    map<string, string> dictionary;
    
    for (int file_id : file_id_list)
    {
        string file_path = target_folder_path + "/" + File_name_list[file_id];
        
        // get file size
        //
        int file_size = get_file_size(file_path);
        
        //        cout << file_id << ": " << file_path << ", size: " << file_size << endl;
        
        // term_freq <term, frequency>
        // file_id :: term_freq
        //
        map<string, int> term_freq;
        
        // read the file like lighting
        //
        FILE *in;
        char *buffer = new char[file_size];
        
        if((in = fopen(file_path.c_str(), "r")) != NULL)
        {
            fread(buffer, file_size, 1, in);
            
            int i = 0;
            string word = "";
            while (i < file_size)
            {
                // check the char is A-Za-z.
                if ((buffer[i] >= 65 && buffer[i] <= 90) || (buffer[i] >= 97 && buffer[i] <= 122))
                {
                    word += buffer[i];
                }
                else
                {
                    if (word.size() >= 3)
                    {
                        Porter2Stemmer::trim(word);
                        
                        bool find = false;
                        for (string stop_word : stop_words)
                        {
                            if (strcmp(word.c_str(), stop_word.c_str()) == 0)
                                //                            if (word == stop_word)
                            {
                                find = true;
                                break;
                            }
                        }
                        if (find == false)
                        {
                            Porter2Stemmer::stem(word);
                            term_freq[word] += 1;
                        }
                    }
                    word = "";
                }
                i++;
            }
            
            // file_id
            // [term, frequency]
            // term -> [file_id, frequency]
            // term -> posting
            //
            for (auto it = term_freq.begin(); it != term_freq.end(); it++)
            {
                dictionary[it->first] += "[" + to_string(file_id) + "," + to_string(it->second) + "]";
            }
        }
        delete[] buffer;
        fclose(in);
        
    }
    return dictionary;
}


// input: vector<string> block paths for each temp blocks, sorted and legal.
// return: string total block path. we merge blocks into one block using pq, remove block files
//
int merge_block(string final_block_path, vector<string> block_path_list)
{
    vector<FILE *> blocks;
    FILE * out;
    if((out = fopen(final_block_path.c_str(), "w")) != NULL)
    {
        priority_queue<string, vector<string>, greater<string>> pq;
        vector<string> pq_record;
        
        // first step, create FILE * in for each block. open them at the same time.
        // read first line of each block to pq.
        // record each line that we read.
        //
        for (string block_path : block_path_list)
        {
            FILE *in;
            in = fopen(block_path.c_str(), "r");
            blocks.push_back(in);
            
            char buffer[22000];
            
            if (fgets(buffer, 22000, in) > 0)
            {
                string temp;
                temp = string(buffer);
                
                if (temp.back() == '\n')
                {
                    temp.pop_back();
                }
                
                pq.push(temp);
                pq_record.push_back(temp);
            }
        }
        
        string last_term;
        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> last_postings;
        
        while (!pq.empty())
        {
            // for each pq top, the lines. The first line of all the blocks must be the first line of the final block.
            // then apart the line to term - posting.
            //
            string temp = pq.top();
            pq.pop();
            
            size_t pos = temp.find(": ");
            
            string term = temp.substr(0, pos);
            string posting = temp.substr(pos+2);
            
            // if term is different from last term
            // push last posting to final block index file.
            // else, push this line with a priority as the first file id
            // which is 23 in "term: [23,1][24,2]"
            // it means 'term' occured in file id 23 once and in file id 24 twice.
            //
            if (strcmp(term.c_str(), last_term.c_str()) != 0 && last_postings.empty() == false)
                //            if (term != last_term && last_postings.empty() == false)
            {
                string output_line = "";
                output_line += last_term + ": ";
                while (!last_postings.empty())
                {
                    output_line += last_postings.top().second;
                    last_postings.pop();
                }
                fputs((output_line + "\n").c_str(), out);
            }
            last_term = term;
            
            size_t id_pos = posting.find(",");
            int first_file_id = atoi(posting.substr(1, id_pos).c_str());
            
            last_postings.push(make_pair(first_file_id, posting));
            
            //            cout << term << ": " << posting << endl;
            
            
            // recharge the line from blocks.
            // which line? according to the pq_record
            // if block is empty? ignore then
            //
            
            auto i = find(pq_record.begin(), pq_record.end(), temp);
            if (i != pq_record.end())
            {
                int this_block = (int)(i - pq_record.begin());
                
                if (!feof(blocks[this_block]))
                {
                    char buffer[22000];
                    if (fgets(buffer, 22000, blocks[this_block]) > 0)
                    {
                        string new_temp;
                        new_temp = string(buffer);
                        
                        if (new_temp.back() == '\n')
                        {
                            new_temp.pop_back();
                        }
                        
                        pq.push(new_temp);
                        pq_record[this_block] = new_temp;
                    }
                }
            }
        }
        // output the last last posting pq.
        //
        string output_line;
        output_line += last_term + ": ";
        while (!last_postings.empty())
        {
            output_line += last_postings.top().second;
            last_postings.pop();
        }
        fputs((output_line+"\n").c_str(), out);
        
        
        // close file
        //
        for (FILE *in : blocks)
        {
            fclose(in);
        }
        fclose(out);
        
        // remove index block temp file
        //
        for (string block_path : block_path_list)
        {
            remove(block_path.c_str());
        }
    }
    
    return 1;
}


// input: string target folder path
// input: string index folder path
// do build index using BSBI
//
string build_index(string target_folder_path, string index_folder_path)
{
    // read a block of files
    // tokenize them into dictionary
    // write dic to each different block
    //
    vector<int> file_id_list;
    vector<string> block_path_list;
    
    int threshold = 1024*1024*20;
    int accumulate_file_size = 0;
    
    int block_number = 0;
    int file_name_list_size = (int)File_name_list.size();
    for (int file_id = 0; file_id < file_name_list_size; file_id++)
    {
        string target_file_path = target_folder_path + "/" + File_name_list[file_id];
        int this_file_size = get_file_size(target_file_path);
        
        // list of ids. as one block
        if (accumulate_file_size + this_file_size > threshold)
        {
            string index_file_path = index_folder_path + "/block" + to_string(block_number++) + ".idx";
            
            //            cout << "file id range: " << *file_id_list.begin() << " ~ " << *file_id_list.rbegin() << endl;
            map<string, string> dictionary = tokenize(target_folder_path, file_id_list);
            write_idx_block(index_file_path, dictionary);
            
            file_id_list.clear();
            block_path_list.push_back(index_file_path);
            accumulate_file_size = 0;
        }
        
        accumulate_file_size += this_file_size;
        file_id_list.push_back(file_id);
    }
    // the last list of ids. as the last block
    string index_file_path = index_folder_path + "/block" + to_string(block_number++) + ".idx";
    
    //    cout << "file id range: " << *file_id_list.begin() << " ~ " << *file_id_list.rbegin() << endl;
    map<string, string> dictionary = tokenize(target_folder_path, file_id_list);
    write_idx_block(index_file_path, dictionary);
    
    file_id_list.clear();
    block_path_list.push_back(index_file_path);
    accumulate_file_size = 0;
    
    //
    // then, we merge each block. remove all tempary index block file using remove.
    //
    string final_block_path = index_folder_path + "/block_all.idx";
    merge_block(final_block_path, block_path_list);
    
    return final_block_path;
}

bool comp_file_id(const pair<int, int> &a,const pair<int, int> &b)
{
    // used only in main function in set intersection.
    // used for compare pair <file_id, frequency> in vector.
    return a.first < b.first;
}

bool comp_frequency_then_file_id(const pair<int, int> &a,const pair<int, int> &b)
{
    // used only in main function in sorting.
    // used for compare pair <file_id, frequency> in vector.
    if (a.second != b.second)
    {
        return a.second > b.second;
    }
    else
    {
        return a.first < b.first;
    }
}


int main(int argc, const char * argv[])
{
    // handle the folder path
    // remove '/' from its back if so
    //
    string target_folder_path = argv[1];
    if (target_folder_path.back() == '/')
    {
        target_folder_path.pop_back();
    }
    string index_folder_path = argv[2];
    if (index_folder_path.back() == '/')
    {
        index_folder_path.pop_back();
    }
    
    // if index_forder not exist
    //
    DIR* index_dir;
    if ((index_dir = opendir(index_folder_path.c_str())) == NULL)
    {
        mkdir(index_folder_path.c_str(), S_IRWXU);
    }
    
    //
    // distinct search terms
    //
    list<string> search_terms;
    
    if (strcmp(argv[3], "-c") != 0)
        //    if (string(argv[3]) != "-c")
    {
        // not concept search
        //
        for (int i = 3; i < argc; i++)
        {
            // stemming
            string search_term = string(argv[i]);
            Porter2Stemmer::trim(search_term);
            Porter2Stemmer::stem(search_term);
            
            if (find(search_terms.begin(), search_terms.end(), search_term) == search_terms.end())
            {
                search_terms.push_back(search_term);
            }
        }
    }
    else
    {
        // I did not do the concept search
        return 0;
    }
    // get the file name list
    // so file_name -> file_id
    //
    File_name_list = get_file_names(target_folder_path);
    
    
    // check the final block exist
    // not exist, build index block_all.idx
    // exist, using the built index
    //
    string final_block_path = index_folder_path + "/block_all.idx";
    ifstream check;
    check.open(final_block_path);
    if (!check)
    {
        final_block_path = build_index(target_folder_path, index_folder_path);
    }
    check.close();
    
    
    map<string, vector<pair<int, int>>> search_term_posting;
    
    FILE *in;
    in = fopen(final_block_path.c_str(), "r");
    
    char buffer[22000];
    
    while (fgets(buffer, 22000, in) > 0)
    {
        string line;
        line = string(buffer);
        if (line.back() == '\n')
        {
            line.pop_back();
        }
        
        size_t pos = line.find(": ");
        
        string term = line.substr(0, pos);
        string posting = line.substr(pos+2);
        
        if (find(search_terms.begin(), search_terms.end(), term) != search_terms.end())
        {
            size_t left_bracket_pos = posting.find("[");
            size_t id_pos = posting.find(",");
            size_t right_bracket_pos = posting.find("]");
            
            while (id_pos != string::npos)
            {
                left_bracket_pos = posting.find("[");
                id_pos = posting.find(",");
                right_bracket_pos = posting.find("]");
                
                int file_id = atoi(posting.substr(left_bracket_pos + 1, id_pos - left_bracket_pos - 1).c_str());
                int frequency = atoi(posting.substr(id_pos + 1, right_bracket_pos - id_pos - 1).c_str());
                
                posting = posting.substr(right_bracket_pos + 1);
                id_pos = posting.find(",");
                
                search_term_posting[term].push_back(make_pair(file_id, frequency));
            }
            // in case of not sorted posting
            // but, it could be removed.
            //
            sort(search_term_posting[term].begin(), search_term_posting[term].end());
        }
    }
    fclose(in);
    
    // what we searched occurs less than in the files.
    // some search term may not occured in the files
    // this means no matches
    // output a newline character.
    // return 0
    //
    if (search_terms.size() > search_term_posting.size())
    {
        //        cout << search_terms.size() << " != " << search_term_posting.size() << endl;
        cout << endl;
        return 0;
    }
    
    // search_term_posting is a map of <term, posting<fid, freq>>
    //
    vector<pair<int, int>> intersection_v = search_term_posting.begin()->second;
    for (auto term_posting_pair : search_term_posting)
    {
        if (intersection_v.size() > term_posting_pair.second.size())
        {
            intersection_v = term_posting_pair.second;
        }
    }
    
    vector<pair<int, int>> result_fid_freq;
    for (auto fid_freq_pair : intersection_v)
    {
        int freq_all = 0;
        bool find_all = true;
        
        for (auto term_posting_pair : search_term_posting)
        {
            int freq_this = 0;
            for (auto this_fid_freq_pair : term_posting_pair.second)
            {
                if (this_fid_freq_pair.first == fid_freq_pair.first)
                {
                    freq_this = this_fid_freq_pair.second;
                    break;
                }
            }
            if (freq_this != 0)
            {
                freq_all += freq_this;
            }
            else
            {
                find_all = false;
                break;
            }
        }
        if (find_all == true)
        {
            result_fid_freq.push_back(make_pair(fid_freq_pair.first, freq_all));
        }
    }
    // if no intersetion
    // output a newline character.
    //
    if (result_fid_freq.size() == 0)
    {
        cout << endl;
        return 0;
    }
    
    sort(result_fid_freq.begin(), result_fid_freq.end(), comp_frequency_then_file_id);
    
    for (auto fid_freq_pair : result_fid_freq)
    {
        //        cout << fid_freq_pair.first << ", " << fid_freq_pair.second << endl;
        cout << File_name_list[fid_freq_pair.first] << endl;
    }
    
    
    return 0;
}
