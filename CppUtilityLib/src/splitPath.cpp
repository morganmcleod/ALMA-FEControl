#include <string>
#include <algorithm>
using namespace std;


void splitPath(const string &pathAndFilename, string &path, string &filename) {
    // make a copy of the pathAndFilename:
    string fullPath(pathAndFilename);
    // replace all backslashes with forward slashes.  This should work ok on modern Windows versions:
    replace(fullPath.begin(), fullPath.end(), '\\','/');
    // find the final slash:
    string::size_type lastSlashPos = fullPath.find_last_of('/');
    // if no slash found, return path as empty:
    if (lastSlashPos == std::string::npos) {
        path = "";
        filename = fullPath;
    } else {
        path = fullPath.substr(0, lastSlashPos);
        filename = fullPath.substr(lastSlashPos + 1, fullPath.size() - lastSlashPos - 1);
    }
}

