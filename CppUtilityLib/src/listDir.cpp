/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2019
*
*This library is free software; you can redistribute it and/or
*modify it under the terms of the GNU Lesser General Public
*License as published by the Free Software Foundation; either
*version 2.1 of the License, or (at your option) any later version.
*
*This library is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*Lesser General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with this library; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*
*/

#include "listDir.h"

#include <algorithm>
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int list_dir(const std::string &path, const std::string &ext, StringSet &target) {
    target.clear();

    struct dirent *entry;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL) {
        return 0;
    }

    std::string extU(ext);
    std::transform(extU.begin(), extU.end(), extU.begin(), ::toupper);

    while ((entry = readdir(dir)) != NULL) {
        std::string nameU(entry->d_name);
        std::transform(nameU.begin(), nameU.end(), nameU.begin(), ::toupper);
        if (extU.empty() || ends_with(nameU, extU))
            target.insert(entry->d_name);
    }

    closedir(dir);
    return target.size();
}


