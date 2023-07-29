#include "../header/search.h"

vector<string> FindInDirectory(string directory, string& file_name)
{
    DIR* dir = opendir(directory.c_str());
    if (dir == nullptr) 
    return {};

    struct dirent* file;
    
    vector<string> paths;
    paths.push_back(directory);

    while ((file = readdir(dir)) != nullptr)
    {
        string filename = file -> d_name;
        if(strcmp(file -> d_name, ".") == 1 && strcmp(file -> d_name, "..") == 1)
        {
            size_t dot_pos = filename.find_last_of(".");
            if(dot_pos == -1 || dot_pos == 0)
            {
                paths.push_back(filename + "\\");
            }
            if (filename.substr(0, dot_pos) == file_name && dot_pos != -1)
            {
                paths =  {paths[0], file_name, filename};
                break;
            }
        }
    }

    closedir(dir);
    return paths;
}