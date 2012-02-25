/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 *
 * This file is part of librbntrvll.
 *
 * librbntrvll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * librbntrvll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with librbntrvll.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <fstream>
 
#include "OutHistoryTree.hpp"
#include "Interval.hpp"
#include "HistoryTreeConfig.hpp"
 
using namespace std;
 
OutHistoryTree::OutHistoryTree()
: AbstractHistoryTree() {
}

OutHistoryTree::OutHistoryTree(HistoryTreeConfig config)
: AbstractHistoryTree(config) {
}

void OutHistoryTree::open(void) {
	// is this history tree already opened?
	if (this->_opened) {
		// TODO: throw something baaad (already opened)
	}
	
	// open stream
	this->openStream();
	
	// do stuff...
	
	// update internal status
	this->_opened = true;
}
void OutHistoryTree::close(void) {
	// is this history tree at least opened?
	if (!this->_opened) {
		// TODO: throw something baaad (tree must be opened)
	}
	
	// close tree (flush last nodes)
	// ...
	
	// close stream
	this->closeStream();
	
	// update internal status
	this->_opened = false;
}

void OutHistoryTree::openStream(void) {
	if (this->_stream.is_open()) {
		// TODO: throw something baaad (file already opened)
	}
	this->_stream.open(this->_config._stateFile.c_str(), fstream::out | fstream::binary);
	if (!this->_stream) {
		// TODO: throw something baaad (invalid/inexisting file)
	}
}

void OutHistoryTree::closeStream(void) {
	if (this->_stream.is_open()) {
		this->_stream.close();
	}
}

OutHistoryTree::~OutHistoryTree() {
	// TODO: put this in AbstractHistoryTree?
	// close if we forgot
	if (this->_opened) {
		this->close();
	}
}
