/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
 *
 * This file is part of libdelorean.
 *
 * libdelorean is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdelorean is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdelorean.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include <cctype>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <delorean/interval/IntervalJar.hpp>
#include <delorean/interval/StringInterval.hpp>
#include <delorean/BasicTypes.hpp>

namespace bfs = boost::filesystem;
namespace balgo = boost::algorithm;

void getIntervalsFromTextFile(const bfs::path& path,
                              std::vector<AbstractInterval::UP>& intervals)
{
    // open file
    bfs::ifstream file {path};
    if (!file.is_open()) {
        return;
    }

    // read lines
    while (true) {
        if (file.eof()) {
            break;
        }

        // get line
        std::string line;
        std::getline(file, line);

        // trim
        balgo::trim(line);

        // ignore if empty
        if (line.size() == 0) {
            continue;
        }

        // ignore if comment
        if (line[0] == '#') {
            continue;
        }

        // extract parts
        std::vector<std::string> parts;
        balgo::split(parts, line, balgo::is_space(), boost::token_compress_on);

        // read range and key
        timestamp_t begin = static_cast<timestamp_t>(std::stoll(parts[0]));
        timestamp_t end = static_cast<timestamp_t>(std::stoll(parts[1]));
        interval_key_t key = static_cast<interval_key_t>(std::stoul(parts[2]));

        // read string value
        std::string value;
        value.reserve(64);
        for (auto it = parts.begin() + 3; it != parts.end(); ++it) {
            auto& part = *it;
            value.append(part);
            if (it < parts.end() - 1) {
                value.append(" ");
            }
        }

        // create string interval
        StringInterval::UP interval {new StringInterval {begin, end, key}};
        interval->setValue(value);

        // add to intervals
        intervals.push_back(std::move(interval));
    }

    // close file now
    file.close();
}
