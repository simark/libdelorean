#!/usr/bin/env python
#
# Copyright (c) 2014 Philippe Proulx <eepp.ca>
#
# This file is part of libdelorean.
#
# libdelorean is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libdelorean is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libdelorean.  If not, see <http://www.gnu.org/licenses/>.
import fileinput
import sys


class Interval:
    def __init__(self, begin, end, key, value, orig_line):
        self.begin = begin
        self.end = end
        self.key = key
        self.value = value
        self.orig_line = orig_line


def _check_overlaps(intervals):
    # last end timestamps per key
    last_ends = {}

    for interval in intervals:
        begin = interval.begin
        end = interval.end
        key = interval.key
        if key not in last_ends:
            if int(begin) > int(end):
                print('begin > end in following line:', file=sys.stderr)
                print('  {}'.format(interval.orig_line), file=sys.stderr)
                return False
            last_ends[key] = int(end)
            continue
        last_end = last_ends[key]
        if last_end >= int(begin):
            print('the following line creates an overlapping interval:',
                  file=sys.stderr)
            print('  {}'.format(interval.orig_line), file=sys.stderr)
            return False
        last_ends[key] = int(end)

    return True


def _parse_line(line):
    line = line.strip()

    # skip meaningless lines
    if not line:
        return None
    if line.startswith('#'):
        return None

    # get tokens
    tokens = line.split(' ')
    begin = tokens[0]
    end = tokens[1]
    key = tokens[2]
    value = ' '.join(tokens[3:])

    return Interval(begin, end, key, value, line)


def _format_interval_line(interval):
    fmt = '{} {} {} {}'
    line = fmt.format(interval.begin, interval.end, interval.key,
                      interval.value)

    return line


def _order_intervals():
    intervals = []

    # parse lines
    for line in fileinput.input():
        interval = _parse_line(line)
        if interval is not None:
            intervals.append(interval)

    # order intervals (ascending end timestamp)
    intervals.sort(key=lambda i: int(i.end))

    # make sure there's no overlap
    overlaps = _check_overlaps(intervals)
    if not overlaps:
        return False

    # output intervals
    for interval in intervals:
        print(_format_interval_line(interval))

    return True


if __name__ == '__main__':
    ret = _order_intervals()
    if not ret:
        sys.exit(1)
